#include "includes.h"
#include "exelight_mp3.hpp"
#include "exelight_mp3_core.hpp"
#include "exelight_runtime.hpp"
#include <array>
#include <filesystem>

namespace Exelight::MP3 {
namespace {
static_assert(sizeof(centity_s)==0x900&&offsetof(centity_s,nextState)==0x3f8&&offsetof(centity_s,alive)==0x8f4);
static_assert(sizeof(client_info_s)==0xed0&&offsetof(client_info_s,health)==0xf4);
static_assert(offsetof(cg_s,refDef)==0x131cf0&&offsetof(cg_s,clientInfo)==0x2e7a40&&offsetof(cg_s,centity)==0x8aaae0);
struct ClientHeader {int valid,nextValid,id;char name[32];int team;};
static_assert(sizeof(ClientHeader)==0x30);
struct Player {int id=-1,team=0,health=0;char name[33]{};std::uint32_t identity=0;Vec3 origin,top,aim;};
// Reuse bulk read buffers; do not place the large native arrays on the game stack.
struct NativeBuffers {std::array<client_info_s,18> clients;std::array<centity_s,18> entities;};
NativeBuffers buffers;
std::array<Player,18> players{};int count=0,selfId=-1,selfTeam=0,selected=-1,retained=-1;
std::uintptr_t capturedCg=0;std::uint64_t capturedAt=0,lastScan=0;
int lastGameTime=0;Config config;AimMath::Controller controller;
std::wstring preferences;bool saveFailed=false;
enum class State {Off,Menu,Focus,Match,Data,Ready,Active,InputFailed,Unsupported};
State state=State::Off;
constexpr int keys[]={VK_RBUTTON,VK_XBUTTON1,VK_XBUTTON2,VK_SHIFT};
const char* keyNames[]={"Right mouse","Mouse 4","Mouse 5","Shift"};
const char* pointNames[]={"Upper body (estimated)","Center mass"};
Vec3 V(ImVec3 p){return {p.x,p.y,p.z};}
const char* Status() {
    switch(state){
    case State::Menu:return "Paused while the mod menu is open";
    case State::Focus:return "Paused outside the game window";
    case State::Match:return "Enter a multiplayer match and finish spawning";
    case State::Data:return "Waiting for valid player and camera data";
    case State::Ready:return "Ready; hold the selected aim key when aim is enabled";
    case State::Active:return "Aim assistance active";
    case State::InputFailed:return "Mouse input failed; aim assistance stopped";
    case State::Unsupported:return "MP3 requires the supported September 2026 game build";
    default:return "Aim and player overlay are disabled";}
}
void Wrapped(const char* text){ImGui::PushTextWrapPos(0);Ui::TextUnformatted(text);ImGui::PopTextWrapPos();}
void Reset(State reason) {controller.Reset();retained=selected=-1;capturedCg=0;capturedAt=lastScan=0;count=0;state=reason;}
bool Supported() {
    if(DetectBuild()!=3)return false;
    IMAGE_DOS_HEADER dos{};IMAGE_NT_HEADERS64 nt{};const auto base=GameBase();
    return ReadObject(base,dos)&&dos.e_lfanew>0&&dos.e_lfanew<1024*1024&&ReadObject(base+dos.e_lfanew,nt)&&nt.Signature==IMAGE_NT_SIGNATURE&&nt.FileHeader.TimeDateStamp==0x6a7b6355&&nt.OptionalHeader.SizeOfImage==0x1d75bc00&&nt.OptionalHeader.CheckSum==0x6531394;
}
bool Scan(std::uintptr_t cg,std::uint64_t now) {
    int local=-1,gameTime=0;
    if(!ReadObject(cg,local)||local<0||local>=18||!ReadObject(cg+offsetof(cg_s,time),gameTime))return false;
    if(cg!=capturedCg||gameTime<lastGameTime){controller.Reset();retained=-1;}
    lastGameTime=gameTime;
    if(!ReadBytes(cg+offsetof(cg_s,clientInfo),buffers.clients.data(),sizeof(buffers.clients))||!ReadBytes(cg+offsetof(cg_s,centity),buffers.entities.data(),sizeof(buffers.entities)))return false;
    const auto& self=buffers.clients[local];
    if(!self.infoValid||self.clientNum!=local||self.health<=0||self.health>100000||self.team<0||self.team>2||cgArray.load()!=cg)return false;
    selfId=local;selfTeam=self.team;count=0;
    for(int i=0;i<18;++i) {
        if(i==local)continue;
        const auto& info=buffers.clients[i];const auto& entity=buffers.entities[i];
        if(!info.infoValid||info.clientNum!=i||info.health<=0||info.health>100000||info.team<0||info.team>2||!entity.alive||entity.nextState.eType!=ET_PLAYER||entity.nextState.number!=i)continue;
        Player player;player.id=i;player.team=info.team;player.health=info.health;std::memcpy(player.name,info.clientName,32);player.name[32]=0;player.identity=Identity(player.name,i);
        player.origin=V(entity.pose.origin);
        if(!BodyPoint(player.origin,V(entity.pose.absmin),V(entity.pose.absmax),config.point==0,player.aim)||!BodyPoint(player.origin,V(entity.pose.absmin),V(entity.pose.absmax),true,player.top))continue;
        player.top.z=player.origin.z+(player.top.z-player.origin.z)/.88f;
        if(!Finite(player.top))continue;
        players[count++]=player;
    }
    capturedCg=cg;capturedAt=now;lastScan=now;return true;
}
bool Recheck(const Player& saved,std::uintptr_t cg,const Camera& camera,Vec2 display,Vec2& screen) {
    ClientHeader raw{},self{};int health=0,selfHealth=0,local=-1;centity_s entity{};
    const auto selfAddress=cg+offsetof(cg_s,clientInfo)+selfId*sizeof(client_info_s);
    if(!ReadObject(cg,local)||local!=selfId||!ReadObject(selfAddress,self)||!self.valid||self.id!=selfId||self.team!=selfTeam||!ReadObject(selfAddress+offsetof(client_info_s,health),selfHealth)||selfHealth<=0)return false;
    const auto address=cg+offsetof(cg_s,clientInfo)+saved.id*sizeof(client_info_s);
    if(!ReadObject(address,raw)||!raw.valid||raw.id!=saved.id||!ReadObject(address+offsetof(client_info_s,health),health)||health<=0||health>100000||!Enemy(selfTeam,raw.team))return false;
    char name[33]{};std::memcpy(name,raw.name,32);
    if(Identity(name,saved.id)!=saved.identity||!ReadObject(cg+offsetof(cg_s,centity)+saved.id*sizeof(centity_s),entity)||!entity.alive||entity.nextState.eType!=ET_PLAYER||entity.nextState.number!=saved.id)return false;
    Vec3 point{};return BodyPoint(V(entity.pose.origin),V(entity.pose.absmin),V(entity.pose.absmax),config.point==0,point)&&Project(camera,point,display,screen);
}
void DrawPlayer(const Player& player,const Camera& camera,Vec2 display) {
    if(config.enemiesOnly&&!Enemy(selfTeam,player.team))return;
    const Vec3 d{player.origin.x-camera.origin.x,player.origin.y-camera.origin.y,player.origin.z-camera.origin.z};
    const auto distance=std::sqrt(Dot(d,d));if(!std::isfinite(distance)||distance>config.range)return;
    Box box{};if(!PlayerBox(camera,player.origin,player.top,display,box))return;
    auto draw=ImGui::GetBackgroundDrawList();const auto color=Enemy(selfTeam,player.team)?IM_COL32(255,105,100,230):IM_COL32(75,210,235,220);
    if(config.boxes){draw->AddRect({box.minimum.x,box.minimum.y},{box.maximum.x,box.maximum.y},IM_COL32(0,0,0,220),0,0,3);draw->AddRect({box.minimum.x,box.minimum.y},{box.maximum.x,box.maximum.y},color);}
    if(config.names)draw->AddText({box.minimum.x,box.minimum.y-19},color,player.name);
    if(config.health) {
        const float h=box.maximum.y-box.minimum.y,ratio=std::clamp(player.health/100.f,0.f,1.f);
        draw->AddRectFilled({box.minimum.x-6,box.minimum.y},{box.minimum.x-3,box.maximum.y},IM_COL32(0,0,0,180));
        draw->AddRectFilled({box.minimum.x-6,box.maximum.y-h*ratio},{box.minimum.x-3,box.maximum.y},IM_COL32(95,235,135,230));
    }
    if(config.distance){char text[64]{};snprintf(text,sizeof(text),"%.0f u | %d HP",distance,player.health);draw->AddText({box.minimum.x,box.maximum.y+2},color,text);}
}
}
void LoadPreferences() {
    if(!settingsFile.empty())preferences=(std::filesystem::path(settingsFile).parent_path()/L"mp3-assist.ini").wstring();
    config=ReadConfig(preferences);Reset(State::Off);
}
void DrawTab() {
    if(!Ui::BeginTabItem("MP Aim and Player Overlay",nullptr))return;
    if(!ImGui::BeginChild("##mp3_tools",{0,0},false)){ImGui::EndChild();ImGui::EndTabItem();return;}
    Wrapped("Multiplayer tools. F8 turns aim and overlay off immediately.");
    Wrapped("Flight is not supported as a non-host player in public multiplayer.");
    if(Ui::Checkbox("Enable aimbot assistance",&config.aim))Reset(State::Off);
    Ui::Combo("Aim activation key",&config.key,keyNames,4);Ui::Combo("Aim point",&config.point,pointNames,2);
    Ui::SliderFloat("Aim target radius",&config.radius,30,400,"%.0f",ImGuiSliderFlags_AlwaysClamp);
    Ui::SliderFloat("Aim speed",&config.speed,2,25,"%.1f",ImGuiSliderFlags_AlwaysClamp);
    Ui::SliderFloat("Mouse response",&config.gain,.2f,3,"%.2f",ImGuiSliderFlags_AlwaysClamp);
    Ui::Checkbox("Show aim circle",&config.circle);ImGui::SameLine();Ui::Checkbox("Show chosen target",&config.target);
    ImGui::Separator();Ui::TextUnformatted("Wallhack style player overlay");
    if(Ui::Checkbox("Enable player overlay",&config.esp))Reset(State::Off);
    Ui::Checkbox("Enemies only",&config.enemiesOnly);Ui::Checkbox("Player boxes",&config.boxes);ImGui::SameLine();Ui::Checkbox("Player names",&config.names);
    Ui::Checkbox("Health bars",&config.health);ImGui::SameLine();Ui::Checkbox("Distance and health text",&config.distance);
    Ui::SliderFloat("Overlay range in game units",&config.range,100,50000,"%.0f",ImGuiSliderFlags_AlwaysClamp);
    Wrapped("Overlay uses available player positions and can appear through cover.");
    Wrapped("Aim excludes your player, teammates, dead players and spectators. Cover visibility is not checked.");
    if(Ui::Button("Save MP3 settings"))saveFailed=!WriteConfig(preferences,config);
    ImGui::SameLine();if(Ui::Button("Stop aim and overlay")){config.aim=config.esp=false;Reset(State::Off);}
    if(saveFailed)Wrapped("Could not save MP3 settings beside the DLL");
    ImGui::Separator();Wrapped(Status());Ui::Text("Players read: %d",count);
    Wrapped("Aim and overlay start disabled each launch. Mouse response depends on game sensitivity.");
    Ui::TextUnformatted("exelight7 MP3 / 3.2.4.7");
    ImGui::EndChild();ImGui::EndTabItem();
}
void Frame(HWND gameWindow,bool menuOpen) {
    if(GetAsyncKeyState(VK_F8)&0x8000){config.aim=config.esp=false;Reset(State::Off);return;}
    if(!config.aim&&!config.esp){Reset(State::Off);return;}
    if(menuOpen){Reset(State::Menu);return;}
    if(!gameWindow||GetForegroundWindow()!=gameWindow||IsIconic(gameWindow)){Reset(State::Focus);return;}
    static const bool supported=Supported();if(!supported){Reset(State::Unsupported);return;}
    const auto cg=cgArray.load(std::memory_order_acquire);
    if(!bPointersInitialized||!cg||Com_SessionMode_GetMode()!=MODE_MULTIPLAYER||!hooks::is_match_loaded()){Reset(State::Match);return;}
    Normalize(config);const auto now=GetTickCount64();
    if(!lastScan||cg!=capturedCg||now-lastScan>=20)if(!Scan(cg,now)){Reset(State::Data);return;}
    if(!Fresh(capturedCg,cgArray.load(std::memory_order_acquire),capturedAt,now)){Reset(State::Data);return;}
    CameraRaw raw{};const auto ioSize=ImGui::GetIO().DisplaySize;const Vec2 display{ioSize.x,ioSize.y};
    if(!ReadObject(cg+offsetof(cg_s,refDef),raw)||raw.width<128||raw.height<128||raw.width>16384||raw.height>16384||raw.x!=0||raw.y!=0||display.x<128||display.y<128){Reset(State::Data);return;}
    const Camera camera{raw.origin,raw.axis[0],raw.axis[1],raw.axis[2],raw.fovX,raw.fovY};
    if(!ValidCamera(camera)||std::abs((float(raw.width)/raw.height)/(display.x/display.y)-1)>.15f){Reset(State::Data);return;}
    thread_local std::vector<Candidate> candidates;candidates.clear();if(candidates.capacity()<18)candidates.reserve(18);
    const bool held=config.aim&&(GetAsyncKeyState(keys[config.key])&0x8000);
    for(int i=0;i<count;++i){const auto& player=players[i];if(config.esp)DrawPlayer(player,camera,display);Vec2 point{};if(config.aim&&Project(camera,player.aim,display,point))candidates.push_back({player.id,player.team,player.health,true,point});}
    const Vec2 center{display.x*.5f,display.y*.5f};const float radius=config.radius*display.y/1080.f;
    selected=config.aim?Select(candidates,selfId,selfTeam,center,radius,held?retained:-1):-1;
    state=State::Ready;
    if(config.aim&&config.circle)ImGui::GetBackgroundDrawList()->AddCircle({center.x,center.y},radius,IM_COL32(65,215,235,140),64);
    if(!config.aim||selected<0){controller.Reset();retained=-1;return;}
    const Player* chosen=nullptr;for(int i=0;i<count;++i)if(players[i].id==selected){chosen=&players[i];break;}
    Vec2 target{};if(!chosen||!Project(camera,chosen->aim,display,target)){controller.Reset();retained=-1;return;}
    if(config.target)ImGui::GetBackgroundDrawList()->AddCircle({target.x,target.y},6,IM_COL32(255,210,90,230),16,1.5f);
    if(!held){controller.Reset();retained=-1;return;}
    if(!Recheck(*chosen,cg,camera,display,target)||DistanceSquared(target,center)>radius*radius){controller.Reset();retained=-1;return;}
    retained=selected;
    const auto step=controller.Step({target.x-center.x,target.y-center.y},ImGui::GetIO().DeltaTime,config.speed,config.gain,selected);
    state=State::Active;if(!step.x&&!step.y)return;
    if(GetForegroundWindow()!=gameWindow||!(GetAsyncKeyState(keys[config.key])&0x8000)||(GetAsyncKeyState(VK_F8)&0x8000)||!bPointersInitialized||cgArray.load()!=cg||!Fresh(capturedCg,cg,capturedAt,GetTickCount64())){controller.Reset();retained=-1;return;}
    INPUT input{};input.type=INPUT_MOUSE;input.mi.dx=step.x;input.mi.dy=step.y;input.mi.dwFlags=MOUSEEVENTF_MOVE;
    if(SendInput(1,&input,sizeof(input))!=1){config.aim=false;Reset(State::InputFailed);Log("MP3 mouse input failed (%lu).",GetLastError());}
}
}
