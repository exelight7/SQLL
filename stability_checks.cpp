#define NOMINMAX
#include <windows.h>
#include <cassert>
#include <atomic>
#include <cstdio>
#include <cstring>
#include <thread>
#include <limits>
#include "exelight_stability_core.hpp"
#include "exelight_safe_memory.hpp"
#include "exelight_graphics_guard.hpp"
struct Point {float x,y,z;};
int main(){
    int reads=0;
    auto valid=[&](std::uintptr_t address,int& id){++reads;assert(address==0x10000);id=4;return true;};
    assert(!Exelight::ClientBaseCandidate(0,valid)&&!Exelight::ClientBaseCandidate(-1,valid)&&reads==0);
    assert(Exelight::ClientBaseCandidate(3294544+0x10000,valid)==0x10000&&reads==1);
    assert(!Exelight::ClientBaseCandidate(3294544+0x10000,[](auto,int&){return false;}));
    assert(!Exelight::ClientBaseCandidate(3294544+0x10000,[](auto,int& id){id=18;return true;}));
    Point p{1,2,3};reads=0;
    auto local=[&](int& id){++reads;id=4;return true;};
    assert(!Exelight::AcceptTracer(false,true,4,p,p,local)&&reads==0);
    assert(!Exelight::AcceptTracer(true,true,-1,p,p,local)&&reads==0);
    auto bad=p;bad.x=std::numeric_limits<float>::quiet_NaN();
    assert(!Exelight::AcceptTracer(true,true,4,bad,p,local)&&reads==0);
    assert(Exelight::AcceptTracer(true,false,4,p,p,local));
    assert(!Exelight::AcceptTracer(true,false,5,p,p,local));
    assert(Exelight::AcceptTracer(true,true,5,p,p,local));
    assert(!Exelight::AcceptTracer(true,true,5,p,p,[](int&){return false;}));
    std::vector<int> tracers;std::recursive_mutex tracerLock;std::vector<std::thread> threads;std::atomic<int> complete{};
    for(int i=0;i<8;++i)threads.emplace_back([&,i]{for(int n=0;n<1000;++n)Exelight::AppendBoundedTracer(tracers,tracerLock,i);++complete;});
    while(complete!=8){std::lock_guard<std::recursive_mutex> lock(tracerLock);assert(tracers.size()<=256);for(auto n:tracers)assert(n>=0&&n<8);}
    for(auto& t:threads)t.join();assert(tracers.size()==256);
    const char* first=Exelight::StableNumberText(7);
    for(int n=-2000;n<2000;++n)assert(std::string(Exelight::StableNumberText(n))==std::to_string(n));
    assert(first==Exelight::StableNumberText(7)&&!strcmp(first,"7"));
    assert(std::string(Exelight::StableNumberText(1))+Exelight::StableNumberText(2)=="12");
    assert(!Exelight::graphicsResizing);
    {Exelight::GraphicsResizeScope firstResize;assert(Exelight::graphicsResizing==1);{Exelight::GraphicsResizeScope nested;assert(Exelight::graphicsResizing==2);}assert(Exelight::graphicsResizing==1);}
    assert(!Exelight::graphicsResizing);
    auto page=VirtualAlloc(nullptr,8192,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);assert(page);
    auto address=reinterpret_cast<std::uintptr_t>(page);DWORD old{};int value=77,copy{};
    assert(Exelight::WriteData(address,value)&&Exelight::ReadObject(address,copy)&&copy==77);
    assert(VirtualProtect(static_cast<char*>(page)+4096,4096,PAGE_READONLY,&old));
    assert(!Exelight::WriteData(address+4094,value));
    assert(VirtualProtect(page,4096,PAGE_NOACCESS,&old));assert(!Exelight::ReadObject(address,copy));
    assert(VirtualFree(page,0,MEM_RELEASE));
    puts("PASS: absent match pointers rejected before subtraction/read; disabled tracers perform no player reads; finite and player filters; 8000 concurrent traces bounded to 256; stable number pointers; overlapping resize scopes; guarded data access.");
}
