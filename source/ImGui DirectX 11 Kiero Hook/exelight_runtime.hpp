#pragma once
#include <windows.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <array>
#include <vector>
#include "exelight_offsets.hpp"

namespace Exelight {
inline wchar_t runtimeLogPath[32768]{};
inline void InitializeLog(HMODULE module) {
    const auto length=GetModuleFileNameW(module,runtimeLogPath,32768);
    if(!length || length>=32768) {runtimeLogPath[0]=0;return;}
    auto slash=wcsrchr(runtimeLogPath,L'\\');
    if(!slash) {runtimeLogPath[0]=0;return;}
    wcscpy_s(slash+1,32768-(slash+1-runtimeLogPath),L"exelight7-MXT111-MP3-load.log");
}
inline void Log(const char* format,...) {
    if(!runtimeLogPath[0])return;
    char line[1536]{};SYSTEMTIME time{};GetLocalTime(&time);
    const int prefix=snprintf(line,sizeof(line),"[%04u-%02u-%02u %02u:%02u:%02u] ",time.wYear,time.wMonth,time.wDay,time.wHour,time.wMinute,time.wSecond);
    va_list args;va_start(args,format);vsnprintf(line+prefix,sizeof(line)-prefix-3,format,args);va_end(args);
    const auto end=strlen(line);line[end]='\r';line[end+1]='\n';
    HANDLE file=CreateFileW(runtimeLogPath,FILE_APPEND_DATA,FILE_SHARE_READ|FILE_SHARE_WRITE,nullptr,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,nullptr);
    if(file!=INVALID_HANDLE_VALUE) {DWORD written{};WriteFile(file,line,static_cast<DWORD>(end+2),&written,nullptr);CloseHandle(file);}
}
inline bool Executable(const void* pointer) {
    MEMORY_BASIC_INFORMATION info{};
    if(!pointer || VirtualQuery(pointer,&info,sizeof(info))!=sizeof(info) || info.State!=MEM_COMMIT || (info.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return false;
    const auto p=info.Protect&0xff;
    return p==PAGE_EXECUTE || p==PAGE_EXECUTE_READ || p==PAGE_EXECUTE_READWRITE || p==PAGE_EXECUTE_WRITECOPY;
}
struct PointerChange {void** slot;void* replacement;void** original;};
template<std::size_t N> inline bool ReplacePointers(const std::array<PointerChange,N>& changes) {
    std::array<DWORD,N> protections{};
    std::array<void*,N> previous{};
    for(std::size_t i=0;i<N;++i) {
        const auto& c=changes[i];
        if(!Readable(c.slot,sizeof(void*)) || !Executable(*c.slot) || !Executable(c.replacement) || !c.original)return false;
        previous[i]=*c.slot;
    }
    std::size_t ready=0;
    for(;ready<N;++ready) {
        if(!VirtualProtect(changes[ready].slot,sizeof(void*),PAGE_READWRITE,&protections[ready]))break;
    }
    if(ready!=N) {
        while(ready) {--ready;DWORD ignored{};VirtualProtect(changes[ready].slot,sizeof(void*),protections[ready],&ignored);}
        return false;
    }
    // Publish all original targets before exposing any callback to another thread.
    for(std::size_t i=0;i<N;++i)*changes[i].original=previous[i];
    for(const auto& c:changes)InterlockedExchangePointer(c.slot,c.replacement);
    // Restore in reverse order: several slots can share the same protection region.
    for(std::size_t i=N;i-->0;) {DWORD ignored{};VirtualProtect(changes[i].slot,sizeof(void*),protections[i],&ignored);}
    return true;
}
struct CodePatch {
    std::uintptr_t rva;
    std::array<unsigned char,8> expected;
    std::array<unsigned char,8> replacement;
    std::size_t readSize;
    std::size_t writeSize;
};
inline bool ApplyValidatedPatches(std::uintptr_t base,std::size_t imageSize,const std::vector<CodePatch>& patches) {
    // Validate the entire profile before changing even one byte.
    for(const auto& p:patches) {
        if(!p.readSize || p.readSize>8 || !p.writeSize || p.writeSize>p.readSize || p.rva>imageSize || p.readSize>imageSize-p.rva || p.rva>UINTPTR_MAX-base)return false;
        const auto address=reinterpret_cast<const unsigned char*>(base+p.rva);
        if(!Readable(address,p.readSize) || !Executable(address))return false;
        if(memcmp(address,p.expected.data(),p.readSize)!=0) {
            const bool alreadyApplied=memcmp(address,p.replacement.data(),p.writeSize)==0 &&
                memcmp(address+p.writeSize,p.expected.data()+p.writeSize,p.readSize-p.writeSize)==0;
            if(!alreadyApplied) {Log("Profile validation failed at RVA 0x%llX; no code patches applied.",static_cast<unsigned long long>(p.rva));return false;}
        }
    }
    struct Applied {const CodePatch* patch;std::array<unsigned char,8> before;};
    std::vector<Applied> applied;applied.reserve(patches.size());
    auto write=[](void* address,const void* data,std::size_t size) {
        DWORD old{},ignored{};
        if(!VirtualProtect(address,size,PAGE_EXECUTE_READWRITE,&old))return false;
        memcpy(address,data,size);
        const bool flushed=FlushInstructionCache(GetCurrentProcess(),address,size)!=0;
        const bool restored=VirtualProtect(address,size,old,&ignored)!=0;
        return flushed&&restored;
    };
    for(const auto& p:patches) {
        auto address=reinterpret_cast<unsigned char*>(base+p.rva);
        if(memcmp(address,p.replacement.data(),p.writeSize)==0)continue;
        Applied entry{&p,{}};memcpy(entry.before.data(),address,p.writeSize);applied.push_back(entry);
        if(!write(address,p.replacement.data(),p.writeSize)) {
            bool rollback=true;
            for(auto it=applied.rbegin();it!=applied.rend();++it)rollback=write(reinterpret_cast<void*>(base+it->patch->rva),it->before.data(),it->patch->writeSize)&&rollback;
            Log("Code patch write failed; rollback %s.",rollback?"complete":"incomplete");return false;
        }
    }
    Log("Validated profile: %zu addresses; changed %zu sites.",patches.size(),applied.size());
    return true;
}
}
