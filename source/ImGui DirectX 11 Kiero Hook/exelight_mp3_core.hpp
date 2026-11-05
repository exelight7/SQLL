#pragma once
#include <windows.h>
#include <array>
#include <cstdint>
#include <cstddef>
#include <algorithm>
#include <string>
#include "exelight_aim_math.hpp"
namespace Exelight::MP3 {
using namespace AimMath;
struct CameraRaw {
    int reserved,x,y,width,height;unsigned char pad14[0x64];
    float fovX,fovY;unsigned char pad80[8];
    Vec3 origin;unsigned char pad94[0x10];Vec3 axis[3];
};
static_assert(sizeof(CameraRaw)==0xc8&&offsetof(CameraRaw,width)==0xc&&offsetof(CameraRaw,fovX)==0x78&&offsetof(CameraRaw,origin)==0x88&&offsetof(CameraRaw,axis)==0xa4);
struct Config {
    bool aim=false,esp=false,circle=true,target=true,boxes=true,names=true,health=true,distance=true,enemiesOnly=true;
    int key=0,point=0;
    float radius=140,speed=9,gain=1,range=10000;
};
inline void Normalize(Config& c) {
    c.key=std::clamp(c.key,0,3);c.point=std::clamp(c.point,0,1);
    auto finite=[](float v,float fallback,float low,float high){return std::isfinite(v)?std::clamp(v,low,high):fallback;};
    c.radius=finite(c.radius,140,30,400);c.speed=finite(c.speed,9,2,25);c.gain=finite(c.gain,1,.2f,3);c.range=finite(c.range,10000,100,50000);
}
inline Config ReadConfig(const std::wstring& path) {
    Config c;if(path.empty())return c;
    auto n=[&](const wchar_t* name,int fallback){return static_cast<int>(GetPrivateProfileIntW(L"MP3",name,fallback,path.c_str()));};
    c.key=n(L"Key",0);c.point=n(L"Point",0);c.radius=n(L"Radius",140);c.speed=n(L"Speed",900)/100.f;c.gain=n(L"Gain",100)/100.f;c.range=n(L"Range",10000);
    c.circle=n(L"Circle",1)!=0;c.target=n(L"Target",1)!=0;c.boxes=n(L"Boxes",1)!=0;c.names=n(L"Names",1)!=0;c.health=n(L"Health",1)!=0;c.distance=n(L"Distance",1)!=0;c.enemiesOnly=n(L"EnemiesOnly",1)!=0;
    Normalize(c);c.aim=c.esp=false;return c;
}
inline bool WriteConfig(const std::wstring& path,Config c) {
    if(path.empty())return false;Normalize(c);
    auto n=[&](const wchar_t* name,int value){const auto text=std::to_wstring(value);return WritePrivateProfileStringW(L"MP3",name,text.c_str(),path.c_str())!=0;};
    bool ok=true;
    for(const auto pair:{std::pair{L"Key",c.key},std::pair{L"Point",c.point},std::pair{L"Radius",int(std::lround(c.radius))},std::pair{L"Speed",int(std::lround(c.speed*100))},std::pair{L"Gain",int(std::lround(c.gain*100))},std::pair{L"Range",int(std::lround(c.range))},std::pair{L"Circle",int(c.circle)},std::pair{L"Target",int(c.target)},std::pair{L"Boxes",int(c.boxes)},std::pair{L"Names",int(c.names)},std::pair{L"Health",int(c.health)},std::pair{L"Distance",int(c.distance)},std::pair{L"EnemiesOnly",int(c.enemiesOnly)}})ok=n(pair.first,pair.second)&&ok;
    return ok;
}
inline bool Enemy(int localTeam,int otherTeam){return localTeam>=0&&localTeam<=2&&otherTeam>=0&&otherTeam<=2&&(localTeam==0||localTeam!=otherTeam);}
struct Box {Vec2 minimum,maximum;};
inline bool PlayerBox(const Camera& camera,Vec3 feet,Vec3 top,Vec2 display,Box& box) {
    Vec2 a{},b{};if(!Project(camera,feet,display,a)||!Project(camera,top,display,b))return false;
    const auto height=a.y-b.y;if(!std::isfinite(height)||height<2||height>display.y*2)return false;
    const float width=height*.44f;box={{b.x-width*.5f,b.y},{b.x+width*.5f,a.y}};
    return Finite(box.minimum)&&Finite(box.maximum);
}
inline bool Fresh(std::uintptr_t saved,std::uintptr_t current,std::uint64_t captured,std::uint64_t now) {
    return saved&&saved==current&&captured&&now>=captured&&now-captured<=100;
}
inline std::uint32_t Identity(const char* name,int id) {
    std::uint32_t hash=2166136261u;
    for(int i=0;i<32&&name[i];++i)hash=(hash^static_cast<unsigned char>(name[i]))*16777619u;
    return (hash^static_cast<std::uint32_t>(id))*16777619u;
}
}
