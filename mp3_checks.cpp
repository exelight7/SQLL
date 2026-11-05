#define NOMINMAX
#include <cassert>
#include <cstdio>
#include <filesystem>
#include <limits>
#include <cstring>
#include "exelight_mp3_core.hpp"
#include "exelight_safe_memory.hpp"
using namespace Exelight::AimMath;
int main(){
    Camera camera{{0,0,0},{1,0,0},{0,1,0},{0,0,1},1,1};Vec2 point{};
    assert(Project(camera,{100,0,0},{1920,1080},point)&&point.x==960&&point.y==540);
    assert(!Project(camera,{-100,0,0},{1920,1080},point));
    auto bad=camera;bad.forward={0,0,0};assert(!ValidCamera(bad));
    bad=camera;bad.tanHalfFovX=std::numeric_limits<float>::quiet_NaN();assert(!ValidCamera(bad));
    Exelight::MP3::CameraRaw raw{};int prefix[]={0,0,0,1920,1080};memcpy(&raw,prefix,sizeof(prefix));
    assert(raw.width==1920&&raw.height==1080&&raw.x==0&&raw.y==0);
    Vec3 aim{};assert(BodyPoint({100,0,0},{-16,-16,0},{16,16,72},true,aim));assert(std::abs(aim.z-63.36f)<.01f);
    Exelight::MP3::Box box{};assert(Exelight::MP3::PlayerBox(camera,{100,0,0},{100,0,72},{1920,1080},box));assert(box.minimum.x<box.maximum.x&&box.minimum.y<box.maximum.y);
    assert(!Exelight::MP3::PlayerBox(camera,{100,0,72},{100,0,0},{1920,1080},box));
    std::vector<Candidate> people{{0,1,100,true,{960,540}},{1,1,100,true,{961,540}},{2,2,0,true,{962,540}},{3,2,100,true,{980,540}},{4,3,100,true,{963,540}},{5,2,100,false,{964,540}},{20,2,100,true,{960,540}}};
    assert(Select(people,0,1,{960,540},140)==3);assert(Select(people,0,3,{960,540},140)==-1);
    assert(Select(people,0,1,{960,540},5)==-1);
    assert(Exelight::MP3::Enemy(0,0)&&Exelight::MP3::Enemy(1,2)&&!Exelight::MP3::Enemy(1,1)&&!Exelight::MP3::Enemy(1,3));
    const auto simulate=[](int fps){Controller c;float error=160;for(int i=0;i<fps;++i){const auto step=c.Step({error,0},1.f/fps,9,1,3);error-=step.x;assert(step.y==0);}return error;};
    assert(std::abs(simulate(60)-simulate(144))<=2&&std::abs(simulate(144))<=2);
    Controller c;auto move=c.Step({10000,-10000},.01f,25,3,1);assert(std::hypot(float(move.x),float(move.y))<=21);
    move=c.Step({100,100},.5f,9,1,1);assert(!move.x&&!move.y&&c.target==-1);
    assert(Exelight::MP3::Fresh(1,1,1000,1100)&&!Exelight::MP3::Fresh(1,1,1000,1101));
    assert(!Exelight::MP3::Fresh(1,2,1000,1001)&&!Exelight::MP3::Fresh(1,1,1000,999)&&!Exelight::MP3::Fresh(1,1,0,1));
    assert(Exelight::MP3::Identity("first",1)!=Exelight::MP3::Identity("second",1));
    int value=77,copy=0;assert(Exelight::ReadObject(reinterpret_cast<std::uintptr_t>(&value),copy)&&copy==77);
    assert(!Exelight::ReadObject(0,copy));auto inaccessible=VirtualAlloc(nullptr,4096,MEM_RESERVE|MEM_COMMIT,PAGE_NOACCESS);assert(inaccessible);assert(!Exelight::ReadObject(reinterpret_cast<std::uintptr_t>(inaccessible),copy));assert(VirtualFree(inaccessible,0,MEM_RELEASE));
    wchar_t path[32768]{};GetModuleFileNameW(nullptr,path,32768);const auto file=(std::filesystem::path(path).parent_path()/L"qa-mp3.ini").wstring();
    Exelight::MP3::Config config;config.aim=config.esp=true;config.radius=260;config.speed=12.5f;config.gain=1.3f;config.key=3;config.range=12345;
    assert(Exelight::MP3::WriteConfig(file,config));assert(WritePrivateProfileStringW(L"MP3",L"Aim",L"1",file.c_str()));assert(WritePrivateProfileStringW(L"MP3",L"ESP",L"1",file.c_str()));
    auto loaded=Exelight::MP3::ReadConfig(file);assert(!loaded.aim&&!loaded.esp&&loaded.radius==260&&loaded.key==3&&loaded.speed==12.5f&&std::abs(loaded.gain-1.3f)<.001f&&loaded.range==12345);
    config.radius=std::numeric_limits<float>::quiet_NaN();config.key=999;config.range=-1;Exelight::MP3::Normalize(config);assert(config.radius==140&&config.key==3&&config.range==100);
    puts("PASS: corrected camera layout; projection and player boxes; self/team/dead/spectator/invalid-slot filters; FFA; estimated body points; bounded input math across frame rates; fresh-context and player identity checks; guarded memory; settings with mandatory aim/ESP default-off.");
    puts("No game was started, no mouse input was sent, and no multiplayer connection was made.");
}
