#define NOMINMAX
#include <cassert>
#include <cstdio>
#include "exelight_mp3.cpp"
std::atomic<std::uintptr_t> cgArray{0};
std::atomic<bool> bPointersInitialized{false};
namespace hooks {bool is_match_loaded(){return false;}}
static unsigned inputCalls=0;
static UINT WINAPI DenyInput(UINT,LPINPUT,int){++inputCalls;return 0;}
extern "C" {decltype(&SendInput) __imp_SendInput=DenyInput;}
int main(){
    using namespace Exelight::MP3;
    ImGui::CreateContext();auto& io=ImGui::GetIO();io.IniFilename=nullptr;io.DisplaySize={1920,1080};io.DeltaTime=1.f/60;
    io.Fonts->AddFontDefault();unsigned char* pixels{};int w{},h{};io.Fonts->GetTexDataAsRGBA32(&pixels,&w,&h);
    ImGui::NewFrame();
    config=Config{};config.esp=true;selfTeam=1;
    const Camera camera{{0,0,0},{1,0,0},{0,1,0},{0,0,1},1,1};
    Player target;target.id=3;target.team=2;target.health=75;target.origin={200,0,0};target.top={200,0,72};strcpy_s(target.name,"Synthetic enemy");
    auto list=ImGui::GetBackgroundDrawList();const auto before=list->VtxBuffer.Size;
    DrawPlayer(target,camera,{1920,1080});assert(list->VtxBuffer.Size>before);
    const auto after=list->VtxBuffer.Size;
    target.team=1;DrawPlayer(target,camera,{1920,1080});assert(list->VtxBuffer.Size==after);
    config.enemiesOnly=false;DrawPlayer(target,camera,{1920,1080});assert(list->VtxBuffer.Size>after);
    const auto beforeInvalid=list->VtxBuffer.Size;target.origin.x=std::numeric_limits<float>::quiet_NaN();DrawPlayer(target,camera,{1920,1080});assert(list->VtxBuffer.Size==beforeInvalid);
    for(const auto& vertex:list->VtxBuffer)assert(std::isfinite(vertex.pos.x)&&std::isfinite(vertex.pos.y));
    // Exercise the production frame's no-window gate with both requested functions enabled.
    config.aim=config.esp=true;Frame(nullptr,false);assert((state==State::Focus||state==State::Off)&&inputCalls==0);
    ImGui::Render();assert(ImGui::GetDrawData()->TotalVtxCount>0);ImGui::DestroyContext();
    puts("PASS: actual production ESP drawing emitted finite boxes/names/health/distance, excluded teammates, supported the optional teammate view, and rejected invalid positions. Production frame paused without a game window; zero input calls.");
    puts("Rendered synthetic player data only; no game or server connection.");
}
