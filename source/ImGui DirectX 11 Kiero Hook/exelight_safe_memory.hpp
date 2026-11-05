#pragma once
#include <windows.h>
#include <cstdint>
#include <type_traits>
namespace Exelight {
inline bool ReadBytes(std::uintptr_t address,void* out,std::size_t size) {
    if(!address||!out||!size||size>32*1024*1024||address>UINTPTR_MAX-size)return false;
    SIZE_T copied{};
    return ReadProcessMemory(GetCurrentProcess(),reinterpret_cast<const void*>(address),out,size,&copied)&&copied==size;
}
template<class T> inline bool ReadObject(std::uintptr_t address,T& value) {
    static_assert(std::is_trivially_copyable_v<T>);
    return ReadBytes(address,&value,sizeof(value));
}
template<class T> inline bool WriteData(std::uintptr_t address,const T& value) {
    static_assert(std::is_trivially_copyable_v<T>);
    if(!address||address>UINTPTR_MAX-sizeof(T))return false;
    MEMORY_BASIC_INFORMATION info{};
    if(VirtualQuery(reinterpret_cast<const void*>(address),&info,sizeof(info))!=sizeof(info)||info.State!=MEM_COMMIT||(info.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return false;
    const auto region=reinterpret_cast<std::uintptr_t>(info.BaseAddress);
    if(address<region||address-region>info.RegionSize||sizeof(T)>info.RegionSize-(address-region))return false;
    const auto mode=info.Protect&0xff;
    if(mode!=PAGE_READWRITE&&mode!=PAGE_WRITECOPY&&mode!=PAGE_EXECUTE_READWRITE&&mode!=PAGE_EXECUTE_WRITECOPY)return false;
    SIZE_T copied{};
    return WriteProcessMemory(GetCurrentProcess(),reinterpret_cast<void*>(address),&value,sizeof(value),&copied)&&copied==sizeof(value);
}
}
