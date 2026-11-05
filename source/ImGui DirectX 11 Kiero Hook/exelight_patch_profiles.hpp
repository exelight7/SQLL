#pragma once
#include "exelight_runtime.hpp"
#include "exelight_crc_profiles.hpp"
namespace Exelight {
inline std::vector<CodePatch> ProfilePatches(int build) {
    std::vector<CodePatch> result;
    result.reserve(1365);
    const auto append=[&](const auto& a,const auto& b,const auto& c) {
        for(auto rva:a)result.push_back({rva,{0x8b,0x0c,0x8b,0x33,0x0c,0x82},{0x48,0x31,0xc9,0x90,0x90,0x90},6,6});
        for(auto rva:b)result.push_back({rva,{0x8b,0x0c,0x8b,0xf7,0xd9,0x03,0x0c,0x82},{0x48,0x31,0xc9,0x90,0x90,0x90,0x90,0x90},8,8});
        for(auto rva:c)result.push_back({rva,{0x8b,0x04,0x82,0x8b,0x14,0x8b,0x3b,0xc2},{0x48,0x31,0xc0,0x48,0x31,0xd2},8,6});
    };
    if(build==2)append(crc_2_1,crc_2_2,crc_2_3);
    if(build==3)append(crc_3_1,crc_3_2,crc_3_3);
    // Older protected builds need their own runtime guard, absent from the public source.
    // Do not fall back to another executable's instruction addresses.
    return result;
}
}
