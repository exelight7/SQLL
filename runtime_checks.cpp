#include <cassert>
#include <cstdio>
#include <set>
#include "exelight_patch_profiles.hpp"

__declspec(noinline) int OriginalA() {return 10;}
__declspec(noinline) int OriginalB() {return 11;}
__declspec(noinline) int ReplacementA() {return 20;}
__declspec(noinline) int ReplacementB() {return 21;}

void CheckProfile(int build) {
    const auto patches=Exelight::ProfilePatches(build);
    assert(patches.size()==1365);
    std::size_t size=0;
    std::set<std::uintptr_t> pages;
    std::set<std::uintptr_t> bytes;
    for(const auto& p:patches) {
        assert(p.rva>0 && p.readSize<=8 && p.writeSize<=p.readSize);
        if(p.rva+p.readSize>size)size=p.rva+p.readSize;
        for(std::size_t i=0;i<p.readSize;++i) {
            pages.insert((p.rva+i)&~std::uintptr_t(4095));
            assert(bytes.insert(p.rva+i).second);
        }
    }
    size=(size+4095)&~std::size_t(4095);
    auto memory=static_cast<unsigned char*>(VirtualAlloc(nullptr,size,MEM_RESERVE,PAGE_NOACCESS));
    assert(memory);
    for(auto page:pages)assert(VirtualAlloc(memory+page,4096,MEM_COMMIT,PAGE_EXECUTE_READWRITE));
    for(const auto& p:patches)memcpy(memory+p.rva,p.expected.data(),p.readSize);
    const auto& last=patches.back();
    // A mismatch at the end must prevent every earlier write.
    memory[last.rva]^=1;
    assert(!Exelight::ApplyValidatedPatches(reinterpret_cast<std::uintptr_t>(memory),size,patches));
    assert(memcmp(memory+patches.front().rva,patches.front().expected.data(),patches.front().readSize)==0);
    memory[last.rva]^=1;
    for(auto page:pages) {DWORD ignored{};assert(VirtualProtect(memory+page,4096,PAGE_EXECUTE_READ,&ignored));}
    assert(Exelight::ApplyValidatedPatches(reinterpret_cast<std::uintptr_t>(memory),size,patches));
    for(const auto& p:patches) {
        assert(memcmp(memory+p.rva,p.replacement.data(),p.writeSize)==0);
        assert(memcmp(memory+p.rva+p.writeSize,p.expected.data()+p.writeSize,p.readSize-p.writeSize)==0);
    }
    // Reapplying a verified profile is harmless and preserves original protection.
    assert(Exelight::ApplyValidatedPatches(reinterpret_cast<std::uintptr_t>(memory),size,patches));
    MEMORY_BASIC_INFORMATION info{};assert(VirtualQuery(memory+patches.front().rva,&info,sizeof(info)));
    assert(info.Protect==PAGE_EXECUTE_READ);
    auto invalid=patches;invalid.front().rva=size;
    assert(!Exelight::ApplyValidatedPatches(reinterpret_cast<std::uintptr_t>(memory),size,invalid));
    assert(VirtualFree(memory,0,MEM_RELEASE));
    std::printf("PASS: profile %d: 1365 sites, mismatch refuses all writes, suffix retained, repeat safe, protection restored, bounds checked.\n",build);
}

int main() {
    assert(Exelight::ProfilePatches(0).empty());
    assert(Exelight::ProfilePatches(1).empty());
    CheckProfile(2);CheckProfile(3);
    auto slots=static_cast<void**>(VirtualAlloc(nullptr,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE));assert(slots);
    slots[0]=reinterpret_cast<void*>(OriginalA);slots[1]=reinterpret_cast<void*>(OriginalB);
    void* originalA{};void* originalB{};
    DWORD old{};assert(VirtualProtect(slots,4096,PAGE_READONLY,&old));
    std::array<Exelight::PointerChange,2> changes{{
        {slots,reinterpret_cast<void*>(ReplacementA),&originalA},
        {slots+1,reinterpret_cast<void*>(ReplacementB),&originalB}
    }};
    auto invalid=changes;invalid[1].slot=nullptr;
    assert(!Exelight::ReplacePointers(invalid));assert(slots[0]==reinterpret_cast<void*>(OriginalA));assert(!originalA);
    assert(Exelight::ReplacePointers(changes));
    assert(originalA==reinterpret_cast<void*>(OriginalA) && originalB==reinterpret_cast<void*>(OriginalB));
    assert(reinterpret_cast<int(*)()>(slots[0])()==20 && reinterpret_cast<int(*)()>(slots[1])()==21);
    MEMORY_BASIC_INFORMATION info{};assert(VirtualQuery(slots,&info,sizeof(info)));assert(info.Protect==PAGE_READONLY);
    assert(VirtualFree(slots,0,MEM_RELEASE));
    std::puts("PASS: pointer batch rejects invalid input without writes; originals published; callback targets and shared-page protection correct.");
}
