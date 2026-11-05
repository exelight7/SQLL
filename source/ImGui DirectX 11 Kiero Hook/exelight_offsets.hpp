#pragma once
#include <windows.h>
#include <cstdint>
#include <cstddef>
namespace Exelight {
// Version/offset mapping reconstructed from the user-supplied DLL.
inline int DetectBuild() {
 const auto base=reinterpret_cast<const unsigned char*>(GetModuleHandleW(nullptr));
 if(!base)return 0;
 const auto dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
 if(dos->e_magic!=IMAGE_DOS_SIGNATURE || dos->e_lfanew<=0 || dos->e_lfanew>0x100000)return 0;
 const auto nt=reinterpret_cast<const IMAGE_NT_HEADERS64*>(base+dos->e_lfanew);
 if(nt->Signature!=IMAGE_NT_SIGNATURE || nt->FileHeader.Machine!=IMAGE_FILE_MACHINE_AMD64)return 0;
 const auto size=(nt->OptionalHeader.SizeOfImage+0xfffU)&0xfffff000U;
 const auto stamp=nt->FileHeader.TimeDateStamp;
 if(size==0x1fab7000 && stamp==0x63f95e3c)return 1;
 if(size==0x1d74b000 && stamp==0x693d731e)return 2;
 if(size==0x1d75c000 && stamp==0x6a7b6355)return 3;
 return 0;
}
/* Address: 18000afb0 | FUN_18000afb0 */

inline unsigned long long MapOffset(int param_1,unsigned long long param_2)

{
  unsigned int uVar1;
  unsigned long long uVar2;
  unsigned long long uVar3;
  
  if (0x1ebf6cd < param_2) {
    if (param_2 < 0x227ca01) {
      if (param_2 == 0x227ca00) {
        if (param_1 == 1) {
          return 0x22e9410;
        }
        if (param_1 == 2) {
          return 0x227ca00;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x227c340;
        }
        return uVar3;
      }
      if (param_2 < 0x20eb325) {
        if (param_2 == 0x20eb324) {
          if (param_1 == 1) {
            return 0x20f73e4;
          }
          if (param_1 == 2) {
            return 0x20eb324;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x20eac64;
          }
          return uVar3;
        }
        if (0x1fc6760 < param_2) {
          if (0x20e74f0 < param_2) {
            if (0x20ead5d < param_2) {
              if (param_2 == 0x20ead97) {
                if (param_1 == 1) {
                  return 0x20f6e57;
                }
                if (param_1 == 2) {
                  return 0x20ead97;
                }
                uVar3 = 0;
                if (param_1 == 3) {
                  uVar3 = 0x20ea6d7;
                }
                return uVar3;
              }
              if (param_2 == 0x20eb2f0) {
                if (param_1 == 1) {
                  return 0x20f73b0;
                }
                if (param_1 == 2) {
                  return 0x20eb2f0;
                }
                uVar3 = 0;
                if (param_1 == 3) {
                  uVar3 = 0x20eac30;
                }
                return uVar3;
              }
              if (param_2 != 0x20eb2f4) {
                return 0;
              }
              if (param_1 != 1) {
                if (param_1 == 2) {
                  return 0x20eb2f4;
                }
                uVar3 = 0;
                if (param_1 == 3) {
                  uVar3 = 0x20eac34;
                }
                return uVar3;
              }
              return 0x20f73b4;
            }
            if (param_2 == 0x20ead5d) {
              if (param_1 == 1) {
                return 0x20f6e1d;
              }
              if (param_1 == 2) {
                return 0x20ead5d;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x20ea69d;
              }
              return uVar3;
            }
            if (param_2 != 0x20e8f30) {
              if (param_2 == 0x20ea7f0) {
                if (param_1 == 1) {
                  return 0x20f68b0;
                }
                if (param_1 == 2) {
                  return 0x20ea7f0;
                }
                uVar3 = 0;
                if (param_1 == 3) {
                  uVar3 = 0x20ea130;
                }
                return uVar3;
              }
              if (param_2 != 0x20eac70) {
                return 0;
              }
              if (param_1 != 1) {
                if (param_1 == 2) {
                  return 0x20eac70;
                }
                uVar3 = 0;
                if (param_1 == 3) {
                  uVar3 = 0x20ea5b0;
                }
                return uVar3;
              }
              return 0x20f6d30;
            }
            if (param_1 == 1) {
              return 0x20f4ff0;
            }
            if (param_1 == 2) {
              return 0x20e8f30;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x20e8870;
            }
            return uVar3;
          }
          if (param_2 == 0x20e74f0) {
            if (param_1 == 1) {
              return 0x20f35b0;
            }
            if (param_1 == 2) {
              return 0x20e74f0;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x20e6e30;
            }
            return uVar3;
          }
          if (0x20e3070 < param_2) {
            if (param_2 == 0x20e3890) {
              if (param_1 == 1) {
                return 0x20ef950;
              }
              if (param_1 == 2) {
                return 0x20e3890;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x20e31d0;
              }
              return uVar3;
            }
            if (param_2 != 0x20e7340) {
              if (param_2 != 0x20e7420) {
                return 0;
              }
              if (param_1 != 1) {
                if (param_1 == 2) {
                  return 0x20e7420;
                }
                uVar3 = 0;
                if (param_1 == 3) {
                  uVar3 = 0x20e6d60;
                }
                return uVar3;
              }
              return 0x20f34e0;
            }
            if (param_1 == 1) {
              return 0x20f3400;
            }
            if (param_1 == 2) {
              return 0x20e7340;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x20e6c80;
            }
            return uVar3;
          }
          if (param_2 == 0x20e3070) {
            if (param_1 == 1) {
              return 0x20ef130;
            }
            if (param_1 == 2) {
              return 0x20e3070;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x20e29b0;
            }
            return uVar3;
          }
          if (param_2 != 0x20c8f60) {
            if (param_2 == 0x20c9ae0) {
              if (param_1 == 1) {
                return 0x20d5ba0;
              }
              if (param_1 == 2) {
                return 0x20c9ae0;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x20c9420;
              }
              return uVar3;
            }
            if (param_2 != 0x20dff50) {
              return 0;
            }
            if (param_1 != 1) {
              if (param_1 == 2) {
                return 0x20dff50;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x20df890;
              }
              return uVar3;
            }
            return 0x20ec010;
          }
          if (param_1 == 1) {
            return 0x20d5020;
          }
          if (param_1 == 2) {
            return 0x20c8f60;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x20c88a0;
          }
          return uVar3;
        }
        if (param_2 == 0x1fc6760) {
          if (param_1 == 1) {
            return 0x1fd2820;
          }
          if (param_1 == 2) {
            return 0x1fc6760;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1fc60a0;
          }
          return uVar3;
        }
        if (0x1eea320 < param_2) {
          if (0x1ef32e0 < param_2) {
            if (param_2 == 0x1ef3600) {
              if (param_1 == 1) {
                return 0x1eff6c0;
              }
              if (param_1 == 2) {
                return 0x1ef3600;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x1ef2f40;
              }
              return uVar3;
            }
            if (param_2 != 0x1f28860) {
              if (param_2 != 0x1f28963) {
                return 0;
              }
              if (param_1 != 1) {
                if (param_1 == 2) {
                  return 0x1f28963;
                }
                uVar3 = 0;
                if (param_1 == 3) {
                  uVar3 = 0x1f282a3;
                }
                return uVar3;
              }
              return 0x1f34a23;
            }
            if (param_1 == 1) {
              return 0x1f34920;
            }
            if (param_1 == 2) {
              return 0x1f28860;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1f281a0;
            }
            return uVar3;
          }
          if (param_2 == 0x1ef32e0) {
            if (param_1 == 1) {
              return 0x1eff3a0;
            }
            if (param_1 == 2) {
              return 0x1ef32e0;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1ef2c20;
            }
            return uVar3;
          }
          if (param_2 == 0x1eea4f0) {
            if (param_1 == 1) {
              return 0x1ef69c0;
            }
            if (param_1 == 2) {
              return 0x1eea4f0;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1ee9e30;
            }
            return uVar3;
          }
          if (param_2 == 0x1eea520) {
            if (param_1 == 1) {
              return 0x1ef69f0;
            }
            if (param_1 == 2) {
              return 0x1eea520;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1ee9e60;
            }
            return uVar3;
          }
          if (param_2 != 0x1eea560) {
            return 0;
          }
          if (param_1 != 1) {
            if (param_1 == 2) {
              return 0x1eea560;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1ee9ea0;
            }
            return uVar3;
          }
          return 0x1ef6a30;
        }
        if (param_2 == 0x1eea320) {
          if (param_1 == 1) {
            return 0x1ef65c0;
          }
          if (param_1 == 2) {
            return 0x1eea320;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1ee9c60;
          }
          return uVar3;
        }
        if (0x1ed9ad0 < param_2) {
          if (param_2 == 0x1edfa00) {
            if (param_1 == 1) {
              return 0x1eebcb0;
            }
            if (param_1 == 2) {
              return 0x1edfa00;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1edf340;
            }
            return uVar3;
          }
          if (param_2 != 0x1eea130) {
            if (param_2 != 0x1eea260) {
              return 0;
            }
            if (param_1 != 1) {
              if (param_1 == 2) {
                return 0x1eea260;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x1ee9ba0;
              }
              return uVar3;
            }
            return 0x1ef6510;
          }
          if (param_1 == 1) {
            return 0x1ef63e0;
          }
          if (param_1 == 2) {
            return 0x1eea130;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1ee9a70;
          }
          return uVar3;
        }
        if (param_2 == 0x1ed9ad0) {
          if (param_1 == 1) {
            return 0x1ee5d80;
          }
          if (param_1 == 2) {
            return 0x1ed9ad0;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1ed9410;
          }
          return uVar3;
        }
        if (param_2 != 0x1ecd180) {
          if (param_2 == 0x1ed5b40) {
            if (param_1 == 1) {
              return 0x1ee1e60;
            }
            if (param_1 == 2) {
              return 0x1ed5b40;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1ed5480;
            }
            return uVar3;
          }
          if (param_2 != 0x1ed8210) {
            if (param_2 != 0x1ed91c0) {
              return 0;
            }
            if (param_1 != 1) {
              if (param_1 == 2) {
                return 0x1ed91c0;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x1ed8b00;
              }
              return uVar3;
            }
            return 0x1ee5470;
          }
          if (param_1 == 1) {
            return 0x1ee4500;
          }
          if (param_1 == 2) {
            return 0x1ed8210;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1ed7b50;
          }
          return uVar3;
        }
        if (param_1 == 1) {
          return 0x1ed94d0;
        }
        if (param_1 == 2) {
          return 0x1ecd180;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x1eccac0;
        }
        return uVar3;
      }
      if (0x21f3110 < param_2) {
        if (0x2252200 < param_2) {
          if (0x2254470 < param_2) {
            if (param_2 == 0x2254800) {
              if (param_1 == 1) {
                return 0x22b0c60;
              }
              if (param_1 == 2) {
                return 0x2254800;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x2254140;
              }
              return uVar3;
            }
            if (param_2 != 0x22548c0) {
              if (param_2 != 0x226b0a0) {
                return 0;
              }
              if (param_1 != 1) {
                if (param_1 == 2) {
                  return 0x226b0a0;
                }
                uVar3 = 0;
                if (param_1 == 3) {
                  uVar3 = 0x226a9e0;
                }
                return uVar3;
              }
              return 0x22c7500;
            }
            if (param_1 == 1) {
              return 0x22b0d20;
            }
            if (param_1 == 2) {
              return 0x22548c0;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x2254200;
            }
            return uVar3;
          }
          if (param_2 == 0x2254470) {
            if (param_1 == 1) {
              return 0x22b08d0;
            }
            if (param_1 == 2) {
              return 0x2254470;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x2253db0;
            }
            return uVar3;
          }
          if (param_2 != 0x2252390) {
            if (param_2 == 0x2253750) {
              if (param_1 == 1) {
                return 0x22afbb0;
              }
              if (param_1 == 2) {
                return 0x2253750;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x2253090;
              }
              return uVar3;
            }
            if (param_2 != 0x22538b0) {
              return 0;
            }
            if (param_1 != 1) {
              if (param_1 == 2) {
                return 0x22538b0;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x22531f0;
              }
              return uVar3;
            }
            return 0x22afd10;
          }
          if (param_1 == 1) {
            return 0x22ae7f0;
          }
          if (param_1 == 2) {
            return 0x2252390;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x2251cd0;
          }
          return uVar3;
        }
        if (param_2 == 0x2252200) {
          if (param_1 == 1) {
            return 0x22ae660;
          }
          if (param_1 == 2) {
            return 0x2252200;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x2251b40;
          }
          return uVar3;
        }
        if (0x221d0b0 < param_2) {
          if (param_2 == 0x2230b50) {
            if (param_1 == 1) {
              return 0x228cfb0;
            }
            if (param_1 == 2) {
              return 0x2230b50;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x2230490;
            }
            return uVar3;
          }
          if (param_2 != 0x22328f0) {
            if (param_2 != 0x2251f70) {
              return 0;
            }
            if (param_1 != 1) {
              if (param_1 == 2) {
                return 0x2251f70;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x22518b0;
              }
              return uVar3;
            }
            return 0x22ae3d0;
          }
          if (param_1 == 1) {
            return 0x228ed50;
          }
          if (param_1 == 2) {
            return 0x22328f0;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x2232230;
          }
          return uVar3;
        }
        if (param_2 == 0x221d0b0) {
          if (param_1 == 1) {
            return 0x2279510;
          }
          if (param_1 == 2) {
            return 0x221d0b0;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x221c9f0;
          }
          return uVar3;
        }
        if (param_2 != 0x221a640) {
          if (param_2 == 0x221a710) {
            if (param_1 == 1) {
              return 0x2276b80;
            }
            if (param_1 == 2) {
              return 0x221a710;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x221a050;
            }
            return uVar3;
          }
          if (param_2 != 0x221b4f0) {
            return 0;
          }
          if (param_1 != 1) {
            if (param_1 == 2) {
              return 0x221b4f0;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x221ae30;
            }
            return uVar3;
          }
          return 0x2277960;
        }
        if (param_1 == 1) {
          return 0x2276ab0;
        }
        if (param_1 == 2) {
          return 0x221a640;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x2219f80;
        }
        return uVar3;
      }
      if (param_2 == 0x21f3110) {
        if (param_1 == 1) {
          return 0x224f580;
        }
        if (param_1 == 2) {
          return 0x21f3110;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x21f2a50;
        }
        return uVar3;
      }
      if (param_2 < 0x20fcc11) {
        if (param_2 == 0x20fcc10) {
          if (param_1 == 1) {
            return 0x2155010;
          }
          if (param_1 == 2) {
            return 0x20fcc10;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x20fc550;
          }
          return uVar3;
        }
        if (0x20f0140 < param_2) {
          if (param_2 == 0x20f0227) {
            if (param_1 == 1) {
              return 0x21485c7;
            }
            if (param_1 == 2) {
              return 0x20f0227;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x20efb67;
            }
            return uVar3;
          }
          if (param_2 != 0x20f5d40) {
            if (param_2 != 0x20fc900) {
              return 0;
            }
            if (param_1 != 1) {
              if (param_1 == 2) {
                return 0x20fc900;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x20fc240;
              }
              return uVar3;
            }
            return 0x2154d00;
          }
          if (param_1 == 1) {
            return 0x214e140;
          }
          if (param_1 == 2) {
            return 0x20f5d40;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x20f5680;
          }
          return uVar3;
        }
        if (param_2 == 0x20f0140) {
          if (param_1 == 1) {
            return 0x21484e0;
          }
          if (param_1 == 2) {
            return 0x20f0140;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x20efa80;
          }
          return uVar3;
        }
        if (param_2 == 0x20eb3c4) {
          if (param_1 == 1) {
            return 0x20f7484;
          }
          if (param_1 == 2) {
            return 0x20eb3c4;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x20ead04;
          }
          return uVar3;
        }
        if (param_2 == 0x20eb3e4) {
          if (param_1 == 1) {
            return 0x20f74a4;
          }
          if (param_1 == 2) {
            return 0x20eb3e4;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x20ead24;
          }
          return uVar3;
        }
        if (param_2 != 0x20ec0b0) {
          if (param_2 != 0x20eff20) {
            return 0;
          }
          if (param_1 != 1) {
            if (param_1 == 2) {
              return 0x20eff20;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x20ef860;
            }
            return uVar3;
          }
          return 0x21482c0;
        }
        if (param_1 == 1) {
          return 0x20f8170;
        }
        if (param_1 == 2) {
          return 0x20ec0b0;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x20eb9f0;
        }
        return uVar3;
      }
      if (0x211b310 < param_2) {
        if (param_2 == 0x211f7c0) {
          if (param_1 == 1) {
            return 0x2177ba0;
          }
          if (param_1 == 2) {
            return 0x211f7c0;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x211f100;
          }
          return uVar3;
        }
        if (param_2 != 0x212b3d0) {
          if (param_2 != 0x21eb3c0) {
            return 0;
          }
          if (param_1 != 1) {
            if (param_1 == 2) {
              return 0x21eb3c0;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x21ead00;
            }
            return uVar3;
          }
          return 0x2247850;
        }
        if (param_1 == 1) {
          return 0x21837b0;
        }
        if (param_1 == 2) {
          return 0x212b3d0;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x212ad10;
        }
        return uVar3;
      }
      if (param_2 == 0x211b310) {
        if (param_1 == 1) {
          return 0x2173710;
        }
        if (param_1 == 2) {
          return 0x211b310;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x211ac50;
        }
        return uVar3;
      }
      if (param_2 == 0x20fd050) {
        if (param_1 == 1) {
          return 0x2155450;
        }
        if (param_1 == 2) {
          return 0x20fd050;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x20fc990;
        }
        return uVar3;
      }
      if (param_2 == 0x20fd0b0) {
        if (param_1 == 1) {
          return 0x21554b0;
        }
        if (param_1 == 2) {
          return 0x20fd0b0;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x20fc9f0;
        }
        return uVar3;
      }
      if (param_2 != 0x20fed40) {
        return 0;
      }
      if (param_1 != 1) {
        if (param_1 == 2) {
          return 0x20fed40;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x20fe680;
        }
        return uVar3;
      }
      return 0x2157140;
    }
    if (param_2 < 0x2634cf1) {
      if (param_2 == 0x2634cf0) {
        if (param_1 == 1) {
          return 0x26ad7a0;
        }
        if (param_1 == 2) {
          return 0x2634cf0;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x2634630;
        }
        return uVar3;
      }
      if (0x25ef520 < param_2) {
        if (0x262d960 < param_2) {
          if (0x2631c10 < param_2) {
            if (param_2 == 0x2634990) {
              if (param_1 == 1) {
                return 0x26ad440;
              }
              if (param_1 == 2) {
                return 0x2634990;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x26342d0;
              }
              return uVar3;
            }
            if (param_2 != 0x2634a20) {
              if (param_2 != 0x2634bf0) {
                return 0;
              }
              if (param_1 != 1) {
                if (param_1 == 2) {
                  return 0x2634bf0;
                }
                uVar3 = 0;
                if (param_1 == 3) {
                  uVar3 = 0x2634530;
                }
                return uVar3;
              }
              return 0x26ad6a0;
            }
            if (param_1 == 1) {
              return 0x26ad4d0;
            }
            if (param_1 == 2) {
              return 0x2634a20;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x2634360;
            }
            return uVar3;
          }
          if (param_2 == 0x2631c10) {
            if (param_1 == 1) {
              return 0x26aa6c0;
            }
            if (param_1 == 2) {
              return 0x2631c10;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x2631550;
            }
            return uVar3;
          }
          if (param_2 != 0x262e740) {
            if (param_2 == 0x262ec60) {
              if (param_1 == 1) {
                return 0x26a7710;
              }
              if (param_1 == 2) {
                return 0x262ec60;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x262e5a0;
              }
              return uVar3;
            }
            if (param_2 != 0x262ee20) {
              return 0;
            }
            if (param_1 != 1) {
              if (param_1 == 2) {
                return 0x262ee20;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x262e760;
              }
              return uVar3;
            }
            return 0x26a78d0;
          }
          if (param_1 == 1) {
            return 0x26a71f0;
          }
          if (param_1 == 2) {
            return 0x262e740;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x262e080;
          }
          return uVar3;
        }
        if (param_2 == 0x262d960) {
          if (param_1 == 1) {
            return 0x26a6410;
          }
          if (param_1 == 2) {
            return 0x262d960;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x262d2a0;
          }
          return uVar3;
        }
        if (0x25f32e0 < param_2) {
          if (param_2 == 0x26276d0) {
            if (param_1 == 1) {
              return 0x26a0180;
            }
            if (param_1 == 2) {
              return 0x26276d0;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x2627010;
            }
            return uVar3;
          }
          if (param_2 != 0x262cac0) {
            if (param_2 != 0x262d750) {
              return 0;
            }
            if (param_1 != 1) {
              if (param_1 == 2) {
                return 0x262d750;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x262d090;
              }
              return uVar3;
            }
            return 0x26a6200;
          }
          if (param_1 == 1) {
            return 0x26a5570;
          }
          if (param_1 == 2) {
            return 0x262cac0;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x262c400;
          }
          return uVar3;
        }
        if (param_2 == 0x25f32e0) {
          if (param_1 == 1) {
            return 0x266bd90;
          }
          if (param_1 == 2) {
            return 0x25f32e0;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x25f2c20;
          }
          return uVar3;
        }
        if (param_2 != 0x25f1410) {
          if (param_2 == 0x25f1780) {
            if (param_1 == 1) {
              return 0x266a230;
            }
            if (param_1 == 2) {
              return 0x25f1780;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x25f10c0;
            }
            return uVar3;
          }
          if (param_2 != 0x25f3270) {
            return 0;
          }
          if (param_1 != 1) {
            if (param_1 == 2) {
              return 0x25f3270;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x25f2bb0;
            }
            return uVar3;
          }
          return 0x266bd20;
        }
        if (param_1 == 1) {
          return 0x2669ec0;
        }
        if (param_1 == 2) {
          return 0x25f1410;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x25f0d50;
        }
        return uVar3;
      }
      if (param_2 == 0x25ef520) {
        if (param_1 == 1) {
          return 0x2667fd0;
        }
        if (param_1 == 2) {
          return 0x25ef520;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x25eee60;
        }
        return uVar3;
      }
      if (0x24a99b0 < param_2) {
        if (0x24a9ec0 < param_2) {
          if (param_2 == 0x2591050) {
            if (param_1 == 1) {
              return 0x2609b00;
            }
            if (param_1 == 2) {
              return 0x2591050;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x2590990;
            }
            return uVar3;
          }
          if (param_2 != 0x2591590) {
            if (param_2 != 0x2591691) {
              return 0;
            }
            if (param_1 != 1) {
              if (param_1 == 2) {
                return 0x2591691;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x2590fd1;
              }
              return uVar3;
            }
            return 0x260a141;
          }
          if (param_1 == 1) {
            return 0x260a040;
          }
          if (param_1 == 2) {
            return 0x2591590;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x2590ed0;
          }
          return uVar3;
        }
        if (param_2 == 0x24a9ec0) {
          if (param_1 == 1) {
            return 0x2522970;
          }
          if (param_1 == 2) {
            return 0x24a9ec0;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x24a9800;
          }
          return uVar3;
        }
        if (param_2 != 0x24a99c0) {
          if (param_2 == 0x24a9cc0) {
            if (param_1 == 1) {
              return 0x2522770;
            }
            if (param_1 == 2) {
              return 0x24a9cc0;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x24a9600;
            }
            return uVar3;
          }
          if (param_2 != 0x24a9d40) {
            return 0;
          }
          if (param_1 != 1) {
            if (param_1 == 2) {
              return 0x24a9d40;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x24a9680;
            }
            return uVar3;
          }
          return 0x25227f0;
        }
        if (param_1 == 1) {
          return 0x2522470;
        }
        if (param_1 == 2) {
          return 0x24a99c0;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x24a9300;
        }
        return uVar3;
      }
      if (param_2 == 0x24a99b0) {
        if (param_1 == 1) {
          return 0x2522460;
        }
        if (param_1 == 2) {
          return 0x24a99b0;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x24a92f0;
        }
        return uVar3;
      }
      if (0x22c9650 < param_2) {
        if (param_2 == 0x24a9290) {
          if (param_1 == 1) {
            return 0x2521d40;
          }
          if (param_1 == 2) {
            return 0x24a9290;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x24a8bd0;
          }
          return uVar3;
        }
        if (param_2 != 0x24a9480) {
          if (param_2 != 0x24a99a0) {
            return 0;
          }
          if (param_1 != 1) {
            if (param_1 == 2) {
              return 0x24a99a0;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x24a92e0;
            }
            return uVar3;
          }
          return 0x2522450;
        }
        if (param_1 == 1) {
          return 0x2521f30;
        }
        if (param_1 == 2) {
          return 0x24a9480;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x24a8dc0;
        }
        return uVar3;
      }
      if (param_2 == 0x22c9650) {
        if (param_1 == 1) {
          return 0x2342100;
        }
        if (param_1 == 2) {
          return 0x22c9650;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x22c8f90;
        }
        return uVar3;
      }
      if (param_2 != 0x227cf30) {
        if (param_2 == 0x22b9900) {
          if (param_1 == 1) {
            return 0x23323b0;
          }
          if (param_1 == 2) {
            return 0x22b9900;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x22b9240;
          }
          return uVar3;
        }
        if (param_2 != 0x22c4935) {
          if (param_2 != 0x22c49fe) {
            return 0;
          }
          if (param_1 != 1) {
            if (param_1 == 2) {
              return 0x22c49fe;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x22c433e;
            }
            return uVar3;
          }
          return 0x233d4ae;
        }
        if (param_1 == 1) {
          return 0x233d3e5;
        }
        if (param_1 == 2) {
          return 0x22c4935;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x22c4275;
        }
        return uVar3;
      }
      if (param_1 == 1) {
        return 0x22e9940;
      }
      if (param_1 == 2) {
        return 0x227cf30;
      }
      uVar3 = 0;
      if (param_1 == 3) {
        uVar3 = 0x227c870;
      }
      return uVar3;
    }
    uVar3 = 0x11287dd0;
    if (param_2 < 0x11287dd1) {
      if (param_2 == 0x11287dd0) {
        if (param_1 == 1) {
          return 0x11306d40;
        }
      }
      else {
        if (param_2 < 0x2d3a5b1) {
          if (param_2 == 0x2d3a5b0) {
            if (param_1 == 1) {
              return 0x2db3060;
            }
            if (param_1 == 2) {
              return 0x2d3a5b0;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x2d39a60;
            }
            return uVar3;
          }
          if (0x279c317 < param_2) {
            if (param_2 == 0x2891c40) {
              if (param_1 == 1) {
                return 0x290a6f0;
              }
              if (param_1 == 2) {
                return 0x2891c40;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x2891580;
              }
              return uVar3;
            }
            if (param_2 != 0x2bc53b0) {
              if (param_2 != 0x2bcb1fc) {
                return 0;
              }
              if (param_1 != 1) {
                if (param_1 == 2) {
                  return 0x2bcb1fc;
                }
                uVar3 = 0;
                if (param_1 == 3) {
                  uVar3 = 0x2bcab3c;
                }
                return uVar3;
              }
              return 0x2c43cac;
            }
            if (param_1 == 1) {
              return 0x2c3de60;
            }
            if (param_1 == 2) {
              return 0x2bc53b0;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x2bc4cf0;
            }
            return uVar3;
          }
          if (param_2 == 0x279c317) {
            if (param_1 == 1) {
              return 0x2814dc7;
            }
            if (param_1 == 2) {
              return 0x279c317;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x279bc57;
            }
            return uVar3;
          }
          if (param_2 != 0x264a547) {
            if (param_2 == 0x264a580) {
              if (param_1 == 1) {
                return 0x26c3030;
              }
              if (param_1 == 2) {
                return 0x264a580;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x2649ec0;
              }
              return uVar3;
            }
            if (param_2 != 0x266faf0) {
              if (param_2 != 0x268cc60) {
                return 0;
              }
              if (param_1 != 1) {
                if (param_1 == 2) {
                  return 0x268cc60;
                }
                uVar3 = 0;
                if (param_1 == 3) {
                  uVar3 = 0x268c5a0;
                }
                return uVar3;
              }
              return 0x2705710;
            }
            if (param_1 == 1) {
              return 0x26e85a0;
            }
            if (param_1 == 2) {
              return 0x266faf0;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x266f430;
            }
            return uVar3;
          }
          if (param_1 == 1) {
            return 0x26c2ff7;
          }
          if (param_1 == 2) {
            return 0x264a547;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x2649e87;
          }
          return uVar3;
        }
        uVar3 = 0xf4378d8;
        if (param_2 < 0xf4378d9) {
          if (param_2 == 0xf4378d8) {
            if (param_1 == 1) {
              return 0xf4b6858;
            }
          }
          else {
            uVar3 = 0x41da710;
            if (param_2 == 0x41da710) {
              if (param_1 == 1) {
                return 0x4259710;
              }
            }
            else {
              uVar3 = 0x52ca460;
              if (param_2 == 0x52ca460) {
                if (param_1 == 1) {
                  return 0x5349460;
                }
              }
              else {
                uVar3 = 0x9b35878;
                if (param_2 != 0x9b35878) {
                  return 0;
                }
                if (param_1 == 1) {
                  return 0x9bb4878;
                }
              }
            }
          }
        }
        else {
          uVar3 = 0x10b3dc20;
          if (param_2 != 0x10b3dc20) {
            if (param_2 == 0x10b3dc40) {
              if (param_1 == 1) {
                return 0x10bbcbc0;
              }
              uVar3 = 0x10b3dc40;
            }
            else {
              if (param_2 != 0x10b3dc50) {
                return 0;
              }
              if (param_1 == 1) {
                return 0x10bbcbd0;
              }
              uVar3 = 0x10b3dc50;
            }
            goto LAB_18000b0ed;
          }
          if (param_1 == 1) {
            return 0x10bbcba0;
          }
        }
      }
    }
    else {
      uVar3 = 0x16deaeb0;
      if (param_2 < 0x16deaeb1) {
        if (param_2 == 0x16deaeb0) {
          if (param_1 == 1) {
            return 0x16e69e20;
          }
        }
        else {
          uVar3 = 0x15e056c8;
          if (param_2 < 0x15e056c9) {
            if (param_2 == 0x15e056c8) {
              if (param_1 == 1) {
                return 0x15e84638;
              }
            }
            else {
              uVar3 = 0x11392044;
              if (param_2 == 0x11392044) {
                if (param_1 == 1) {
                  return 0x11410fb4;
                }
              }
              else {
                uVar3 = 0x113af1e8;
                if (param_2 == 0x113af1e8) {
                  if (param_1 == 1) {
                    return 0x1142e158;
                  }
                }
                else {
                  uVar3 = 0x14f344b0;
                  if (param_2 != 0x14f344b0) {
                    return 0;
                  }
                  if (param_1 == 1) {
                    return 0x14fb3420;
                  }
                }
              }
            }
          }
          else {
            uVar3 = 0x1686e874;
            if (param_2 == 0x1686e874) {
              if (param_1 == 1) {
                return 0x168ed7f4;
              }
            }
            else {
              uVar3 = 0x1686e948;
              if (param_2 == 0x1686e948) {
                if (param_1 == 1) {
                  return 0x168ed8c8;
                }
              }
              else {
                uVar3 = 0x16deaca0;
                if (param_2 != 0x16deaca0) {
                  return 0;
                }
                if (param_1 == 1) {
                  return 0x16e69c10;
                }
              }
            }
          }
        }
      }
      else {
        uVar3 = 0x17df03d8;
        if (param_2 < 0x17df03d9) {
          if (param_2 == 0x17df03d8) {
            if (param_1 == 1) {
              return 0x17e6f308;
            }
          }
          else {
            uVar3 = 0x1768e578;
            if (param_2 != 0x1768e578) {
              if (param_2 == 0x1795d258) {
                if (param_1 == 1) {
                  return 0x179dc158;
                }
                uVar3 = 0x1795d258;
              }
              else {
                if (param_2 != 0x1795d25c) {
                  return 0;
                }
                if (param_1 == 1) {
                  return 0x179dc15c;
                }
                uVar3 = 0x1795d25c;
              }
              goto LAB_18000b0ed;
            }
            if (param_1 == 1) {
              return 0x1770d4e8;
            }
          }
        }
        else {
          uVar3 = 0x17df0405;
          if (param_2 == 0x17df0405) {
            if (param_1 == 1) {
              return 0x17e6f335;
            }
          }
          else {
            uVar3 = 0x1976ca80;
            if (param_2 == 0x1976ca80) {
              if (param_1 == 1) {
                return 0x197eba00;
              }
            }
            else {
              uVar3 = 0x19babf00;
              if (param_2 != 0x19babf00) {
                return 0;
              }
              if (param_1 == 1) {
                return 0x19c2ae80;
              }
            }
          }
        }
      }
    }
    goto LAB_18000b056;
  }
  if (param_2 == 0x1ebf6cd) {
    if (param_1 == 1) {
      return 0x1ecba9d;
    }
    if (param_1 == 2) {
      return 0x1ebf6cd;
    }
    uVar3 = 0;
    if (param_1 == 3) {
      uVar3 = 0x1ebf00d;
    }
    return uVar3;
  }
  uVar3 = 0x1c81c30;
  if (0x1c81c30 < param_2) {
    if (0x1e94e50 < param_2) {
      if (0x1ea2cc0 < param_2) {
        if (0x1eaad60 < param_2) {
          if (0x1eb4960 < param_2) {
            if (param_2 == 0x1ebaac0) {
              if (param_1 == 1) {
                return 0x1ec6e90;
              }
              if (param_1 == 2) {
                return 0x1ebaac0;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x1eba400;
              }
              return uVar3;
            }
            if (param_2 != 0x1ebaf40) {
              if (param_2 != 0x1ebb200) {
                return 0;
              }
              if (param_1 != 1) {
                if (param_1 == 2) {
                  return 0x1ebb200;
                }
                uVar3 = 0;
                if (param_1 == 3) {
                  uVar3 = 0x1ebab40;
                }
                return uVar3;
              }
              return 0x1ec75d0;
            }
            if (param_1 == 1) {
              return 0x1ec7310;
            }
            if (param_1 == 2) {
              return 0x1ebaf40;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1eba880;
            }
            return uVar3;
          }
          if (param_2 == 0x1eb4960) {
            if (param_1 == 1) {
              return 0x1ec0d30;
            }
            if (param_1 == 2) {
              return 0x1eb4960;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1eb42a0;
            }
            return uVar3;
          }
          if (param_2 != 0x1eb0620) {
            if (param_2 == 0x1eb0760) {
              if (param_1 == 1) {
                return 0x1ebcb30;
              }
              if (param_1 == 2) {
                return 0x1eb0760;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x1eb00a0;
              }
              return uVar3;
            }
            if (param_2 != 0x1eb0e44) {
              return 0;
            }
            if (param_1 != 1) {
              if (param_1 == 2) {
                return 0x1eb0e44;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x1eb0784;
              }
              return uVar3;
            }
            return 0x1ebd214;
          }
          if (param_1 == 1) {
            return 0x1ebc9f0;
          }
          if (param_1 == 2) {
            return 0x1eb0620;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1eaff60;
          }
          return uVar3;
        }
        if (param_2 == 0x1eaad60) {
          if (param_1 == 1) {
            return 0x1eb7130;
          }
          if (param_1 == 2) {
            return 0x1eaad60;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1eaa6a0;
          }
          return uVar3;
        }
        if (0x1ea4960 < param_2) {
          if (param_2 == 0x1ea4a1c) {
            if (param_1 == 1) {
              return 0x1eb0dec;
            }
            if (param_1 == 2) {
              return 0x1ea4a1c;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1ea435c;
            }
            return uVar3;
          }
          if (param_2 == 0x1ea5650) {
            if (param_1 == 1) {
              return 0x1eb1a20;
            }
            if (param_1 == 2) {
              return 0x1ea5650;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1ea4f90;
            }
            return uVar3;
          }
          if (param_2 != 0x1ea5669) {
            return 0;
          }
          if (param_1 != 1) {
            if (param_1 == 2) {
              return 0x1ea5669;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1ea4fa9;
            }
            return uVar3;
          }
          return 0x1eb1a39;
        }
        if (param_2 == 0x1ea4960) {
          if (param_1 == 1) {
            return 0x1eb0d30;
          }
          if (param_1 == 2) {
            return 0x1ea4960;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1ea42a0;
          }
          return uVar3;
        }
        if (param_2 != 0x1ea2ce0) {
          if (param_2 == 0x1ea3df0) {
            if (param_1 == 1) {
              return 0x1eb01c0;
            }
            if (param_1 == 2) {
              return 0x1ea3df0;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1ea3730;
            }
            return uVar3;
          }
          if (param_2 != 0x1ea3ef0) {
            return 0;
          }
          if (param_1 != 1) {
            if (param_1 == 2) {
              return 0x1ea3ef0;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1ea3830;
            }
            return uVar3;
          }
          return 0x1eb02c0;
        }
        if (param_1 == 1) {
          return 0x1eaf0b0;
        }
        if (param_1 == 2) {
          return 0x1ea2ce0;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x1ea2620;
        }
        return uVar3;
      }
      if (param_2 == 0x1ea2cc0) {
        if (param_1 == 1) {
          return 0x1eaf090;
        }
        if (param_1 == 2) {
          return 0x1ea2cc0;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x1ea2600;
        }
        return uVar3;
      }
      if (0x1e9d360 < param_2) {
        if (0x1ea0280 < param_2) {
          if (param_2 == 0x1ea09a0) {
            if (param_1 == 1) {
              return 0x1eacd70;
            }
            if (param_1 == 2) {
              return 0x1ea09a0;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1ea02e0;
            }
            return uVar3;
          }
          if (param_2 != 0x1ea0a80) {
            if (param_2 != 0x1ea0dd0) {
              return 0;
            }
            if (param_1 != 1) {
              if (param_1 == 2) {
                return 0x1ea0dd0;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x1ea0710;
              }
              return uVar3;
            }
            return 0x1ead1a0;
          }
          if (param_1 == 1) {
            return 0x1eace50;
          }
          if (param_1 == 2) {
            return 0x1ea0a80;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1ea03c0;
          }
          return uVar3;
        }
        if (param_2 == 0x1ea0280) {
          if (param_1 == 1) {
            return 0x1eac650;
          }
          if (param_1 == 2) {
            return 0x1ea0280;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1e9fbc0;
          }
          return uVar3;
        }
        if (param_2 != 0x1e9dfe0) {
          if (param_2 == 0x1e9f050) {
            if (param_1 == 1) {
              return 0x1eab420;
            }
            if (param_1 == 2) {
              return 0x1e9f050;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1e9e990;
            }
            return uVar3;
          }
          if (param_2 != 0x1ea0230) {
            return 0;
          }
          if (param_1 != 1) {
            if (param_1 == 2) {
              return 0x1ea0230;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1e9fb70;
            }
            return uVar3;
          }
          return 0x1eac600;
        }
        if (param_1 == 1) {
          return 0x1eaa3b0;
        }
        if (param_1 == 2) {
          return 0x1e9dfe0;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x1e9d920;
        }
        return uVar3;
      }
      if (param_2 == 0x1e9d360) {
        if (param_1 == 1) {
          return 0x1ea9730;
        }
        if (param_1 == 2) {
          return 0x1e9d360;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x1e9cca0;
        }
        return uVar3;
      }
      if (0x1e9cde0 < param_2) {
        if (param_2 == 0x1e9ce90) {
          if (param_1 == 1) {
            return 0x1ea9260;
          }
          if (param_1 == 2) {
            return 0x1e9ce90;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1e9c7d0;
          }
          return uVar3;
        }
        if (param_2 == 0x1e9d290) {
          if (param_1 == 1) {
            return 0x1ea9660;
          }
          if (param_1 == 2) {
            return 0x1e9d290;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1e9cbd0;
          }
          return uVar3;
        }
        if (param_2 != 0x1e9d300) {
          return 0;
        }
        if (param_1 != 1) {
          if (param_1 == 2) {
            return 0x1e9d300;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1e9cc40;
          }
          return uVar3;
        }
        return 0x1ea96d0;
      }
      if (param_2 == 0x1e9cde0) {
        if (param_1 == 1) {
          return 0x1ea91b0;
        }
        if (param_1 == 2) {
          return 0x1e9cde0;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x1e9c720;
        }
        return uVar3;
      }
      if (param_2 == 0x1e990b0) {
        if (param_1 == 1) {
          return 0x1ea5480;
        }
        if (param_1 == 2) {
          return 0x1e990b0;
        }
        uVar1 = 0;
        if (param_1 == 3) {
          uVar1 = 0x1e989f0;
        }
        return (unsigned long long)uVar1;
      }
      if (param_2 == 0x1e99770) {
        if (param_1 == 1) {
          return 0x1ea5b40;
        }
        if (param_1 == 2) {
          return 0x1e99770;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x1e990b0;
        }
        return uVar3;
      }
      if (param_2 != 0x1e99b80) {
        if (param_2 != 0x1e9c8ae) {
          return 0;
        }
        if (param_1 != 1) {
          if (param_1 == 2) {
            return 0x1e9c8ae;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1e9c1ee;
          }
          return uVar3;
        }
        return 0x1ea8c7e;
      }
      if (param_1 == 1) {
        return 0x1ea5f50;
      }
      if (param_1 == 2) {
        return 0x1e99b80;
      }
      uVar3 = 0;
      if (param_1 == 3) {
        uVar3 = 0x1e994c0;
      }
      return uVar3;
    }
    if (param_2 == 0x1e94e50) {
      if (param_1 == 1) {
        return 0x1ea1220;
      }
      if (param_1 == 2) {
        return 0x1e94e50;
      }
      uVar3 = 0;
      if (param_1 == 3) {
        uVar3 = 0x1e94790;
      }
      return uVar3;
    }
    if (0x1e72490 < param_2) {
      if (0x1e87910 < param_2) {
        if (0x1e90a50 < param_2) {
          if (param_2 == 0x1e93180) {
            if (param_1 == 1) {
              return 0x1e9f550;
            }
            if (param_1 == 2) {
              return 0x1e93180;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1e92ac0;
            }
            return uVar3;
          }
          if (param_2 != 0x1e9432b) {
            if (param_2 != 0x1e94860) {
              return 0;
            }
            if (param_1 != 1) {
              if (param_1 == 2) {
                return 0x1e94860;
              }
              uVar3 = 0;
              if (param_1 == 3) {
                uVar3 = 0x1e941a0;
              }
              return uVar3;
            }
            return 0x1ea0c30;
          }
          if (param_1 == 1) {
            return 0x1ea06fb;
          }
          if (param_1 == 2) {
            return 0x1e9432b;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1e93c6b;
          }
          return uVar3;
        }
        if (param_2 == 0x1e90a50) {
          if (param_1 == 1) {
            return 0x1e9ce20;
          }
          if (param_1 == 2) {
            return 0x1e90a50;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1e90390;
          }
          return uVar3;
        }
        if (param_2 != 0x1e8a4a0) {
          if (param_2 == 0x1e8a570) {
            if (param_1 == 1) {
              return 0x1e96940;
            }
            if (param_1 == 2) {
              return 0x1e8a570;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1e89eb0;
            }
            return uVar3;
          }
          if (param_2 != 0x1e906e0) {
            return 0;
          }
          if (param_1 != 1) {
            if (param_1 == 2) {
              return 0x1e906e0;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1e90020;
            }
            return uVar3;
          }
          return 0x1e9cab0;
        }
        if (param_1 == 1) {
          return 0x1e96870;
        }
        if (param_1 == 2) {
          return 0x1e8a4a0;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x1e89de0;
        }
        return uVar3;
      }
      if (param_2 == 0x1e87910) {
        if (param_1 == 1) {
          return 0x1e93ce0;
        }
        if (param_1 == 2) {
          return 0x1e87910;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x1e87250;
        }
        return uVar3;
      }
      if (0x1e76cb0 < param_2) {
        if (param_2 == 0x1e77490) {
          if (param_1 == 1) {
            return 0x1e83860;
          }
          if (param_1 == 2) {
            return 0x1e77490;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1e76dd0;
          }
          return uVar3;
        }
        if (param_2 != 0x1e7b060) {
          if (param_2 != 0x1e85450) {
            return 0;
          }
          if (param_1 != 1) {
            if (param_1 == 2) {
              return 0x1e85450;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1e84d90;
            }
            return uVar3;
          }
          return 0x1e91820;
        }
        if (param_1 == 1) {
          return 0x1e87430;
        }
        if (param_1 == 2) {
          return 0x1e7b060;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x1e7a9a0;
        }
        return uVar3;
      }
      if (param_2 == 0x1e76cb0) {
        if (param_1 == 1) {
          return 0x1e83080;
        }
        if (param_1 == 2) {
          return 0x1e76cb0;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x1e765f0;
        }
        return uVar3;
      }
      if (param_2 != 0x1e75f90) {
        if (param_2 == 0x1e760b0) {
          if (param_1 == 1) {
            return 0x1e82480;
          }
          if (param_1 == 2) {
            return 0x1e760b0;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1e759f0;
          }
          return uVar3;
        }
        if (param_2 != 0x1e76880) {
          return 0;
        }
        if (param_1 != 1) {
          if (param_1 == 2) {
            return 0x1e76880;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1e761c0;
          }
          return uVar3;
        }
        return 0x1e82c50;
      }
      if (param_1 == 1) {
        return 0x1e82360;
      }
      if (param_1 == 2) {
        return 0x1e75f90;
      }
      uVar3 = 0;
      if (param_1 == 3) {
        uVar3 = 0x1e758d0;
      }
      return uVar3;
    }
    if (param_2 == 0x1e72490) {
      if (param_1 == 1) {
        return 0x1e7e860;
      }
      if (param_1 == 2) {
        return 0x1e72490;
      }
      uVar3 = 0;
      if (param_1 == 3) {
        uVar3 = 0x1e71dd0;
      }
      return uVar3;
    }
    if (0x1dfc580 < param_2) {
      if (0x1e01380 < param_2) {
        if (param_2 == 0x1e013d0) {
          if (param_1 == 1) {
            return 0x1e0d7a0;
          }
          if (param_1 == 2) {
            return 0x1e013d0;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1e00d10;
          }
          return uVar3;
        }
        if (param_2 != 0x1e06110) {
          if (param_2 != 0x1e199f0) {
            return 0;
          }
          if (param_1 != 1) {
            if (param_1 == 2) {
              return 0x1e199f0;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1e19330;
            }
            return uVar3;
          }
          return 0x1e25dc0;
        }
        if (param_1 == 1) {
          return 0x1e124e0;
        }
        if (param_1 == 2) {
          return 0x1e06110;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x1e05a50;
        }
        return uVar3;
      }
      if (param_2 == 0x1e01380) {
        if (param_1 == 1) {
          return 0x1e0d750;
        }
        if (param_1 == 2) {
          return 0x1e01380;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x1e00cc0;
        }
        return uVar3;
      }
      if (param_2 != 0x1dfcc60) {
        if (param_2 == 0x1dfcdd0) {
          if (param_1 == 1) {
            return 0x1e091a0;
          }
          if (param_1 == 2) {
            return 0x1dfcdd0;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1dfc710;
          }
          return uVar3;
        }
        if (param_2 != 0x1dfdfe0) {
          return 0;
        }
        if (param_1 != 1) {
          if (param_1 == 2) {
            return 0x1dfdfe0;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1dfd920;
          }
          return uVar3;
        }
        return 0x1e0a3b0;
      }
      if (param_1 == 1) {
        return 0x1e09030;
      }
      if (param_1 == 2) {
        return 0x1dfcc60;
      }
      uVar3 = 0;
      if (param_1 == 3) {
        uVar3 = 0x1dfc5a0;
      }
      return uVar3;
    }
    if (param_2 == 0x1dfc580) {
      if (param_1 == 1) {
        return 0x1e08950;
      }
      if (param_1 == 2) {
        return 0x1dfc580;
      }
      uVar3 = 0;
      if (param_1 == 3) {
        uVar3 = 0x1dfbec0;
      }
      return uVar3;
    }
    if (0x1de0b70 < param_2) {
      if (param_2 == 0x1de65c0) {
        if (param_1 == 1) {
          return 0x1df2990;
        }
        if (param_1 == 2) {
          return 0x1de65c0;
        }
        uVar3 = 0;
        if (param_1 == 3) {
          uVar3 = 0x1de5f00;
        }
        return uVar3;
      }
      if (param_2 != 0x1de9e10) {
        if (param_2 != 0x1decfd0) {
          return 0;
        }
        if (param_1 != 1) {
          if (param_1 == 2) {
            return 0x1decfd0;
          }
          uVar3 = 0;
          if (param_1 == 3) {
            uVar3 = 0x1dec910;
          }
          return uVar3;
        }
        return 0x1df93a0;
      }
      if (param_1 == 1) {
        return 0x1df61e0;
      }
      if (param_1 == 2) {
        return 0x1de9e10;
      }
      uVar3 = 0;
      if (param_1 == 3) {
        uVar3 = 0x1de9750;
      }
      return uVar3;
    }
    if (param_2 == 0x1de0b70) {
      if (param_1 == 1) {
        return 0x1decf40;
      }
      if (param_1 == 2) {
        return 0x1de0b70;
      }
      uVar3 = 0;
      if (param_1 == 3) {
        uVar3 = 0x1de04b0;
      }
      return uVar3;
    }
    uVar3 = 0x1ccced7;
    if (param_2 == 0x1ccced7) {
      if (param_1 == 1) {
        return 0x1cd92a7;
      }
    }
    else {
      uVar3 = 0x1ccdd8c;
      if (param_2 == 0x1ccdd8c) {
        if (param_1 == 1) {
          return 0x1cda15c;
        }
      }
      else {
        uVar3 = 0x1d10840;
        if (param_2 != 0x1d10840) {
          if (param_2 != 0x1de0970) {
            return 0;
          }
          if (param_1 != 1) {
            if (param_1 == 2) {
              return 0x1de0970;
            }
            uVar3 = 0;
            if (param_1 == 3) {
              uVar3 = 0x1de02b0;
            }
            return uVar3;
          }
          return 0x1decd40;
        }
        if (param_1 == 1) {
          return 0x1d1cc10;
        }
      }
    }
    goto LAB_18000b056;
  }
  if (param_2 == 0x1c81c30) {
    if (param_1 == 1) {
      return 0x1c8e000;
    }
    goto LAB_18000b056;
  }
  uVar3 = 0x13ca5d0;
  if (0x13ca5d0 < param_2) {
    uVar3 = 0x143e140;
    if (param_2 < 0x143e141) {
      if (param_2 == 0x143e140) {
        if (param_1 == 1) {
          return 0x143e120;
        }
      }
      else {
        uVar3 = 0x13ca9c0;
        if (param_2 < 0x13ca9c1) {
          if (param_2 == 0x13ca9c0) {
            if (param_1 == 1) {
              return 0x13ca9a0;
            }
          }
          else {
            uVar3 = 0x13ca780;
            if (param_2 < 0x13ca781) {
              if (param_2 == 0x13ca780) {
                if (param_1 == 1) {
                  return 0x13ca760;
                }
              }
              else {
                uVar3 = 0x13ca5e0;
                if (param_2 != 0x13ca5e0) {
                  if (param_2 == 0x13ca640) {
                    if (param_1 == 1) {
                      return 0x13ca620;
                    }
                    uVar3 = 0x13ca640;
                  }
                  else if (param_2 == 0x13ca6a0) {
                    if (param_1 == 1) {
                      return 0x13ca680;
                    }
                    uVar3 = 0x13ca6a0;
                  }
                  else {
                    if (param_2 != 0x13ca700) {
                      return 0;
                    }
                    if (param_1 == 1) {
                      return 0x13ca6e0;
                    }
                    uVar3 = 0x13ca700;
                  }
                  goto LAB_18000b0ed;
                }
                if (param_1 == 1) {
                  return 0x13ca5c0;
                }
              }
            }
            else {
              uVar3 = 0x13ca810;
              if (param_2 == 0x13ca810) {
                if (param_1 == 1) {
                  return 0x13ca7f0;
                }
              }
              else {
                uVar3 = 0x13ca8a0;
                if (param_2 == 0x13ca8a0) {
                  if (param_1 == 1) {
                    return 0x13ca880;
                  }
                }
                else {
                  uVar3 = 0x13ca920;
                  if (param_2 != 0x13ca920) {
                    return 0;
                  }
                  if (param_1 == 1) {
                    return 0x13ca900;
                  }
                }
              }
            }
          }
        }
        else {
          uVar3 = 0x141f9d0;
          if (param_2 < 0x141f9d1) {
            if (param_2 == 0x141f9d0) {
              if (param_1 == 1) {
                return 0x141f9b0;
              }
            }
            else {
              uVar3 = 0x13caa20;
              if (param_2 == 0x13caa20) {
                if (param_1 == 1) {
                  return 0x13caa00;
                }
              }
              else {
                uVar3 = 0x13cad20;
                if (param_2 == 0x13cad20) {
                  if (param_1 == 1) {
                    return 0x13cad00;
                  }
                }
                else {
                  uVar3 = 0x13e6a74;
                  if (param_2 != 0x13e6a74) {
                    return 0;
                  }
                  if (param_1 == 1) {
                    return 0x13e6a54;
                  }
                }
              }
            }
          }
          else {
            uVar3 = 0x1427a20;
            if (param_2 == 0x1427a20) {
              if (param_1 == 1) {
                return 0x1427a00;
              }
            }
            else {
              uVar3 = 0x143a620;
              if (param_2 == 0x143a620) {
                if (param_1 == 1) {
                  return 0x143a600;
                }
              }
              else {
                uVar3 = 0x143c380;
                if (param_2 != 0x143c380) {
                  return 0;
                }
                if (param_1 == 1) {
                  return 0x143c360;
                }
              }
            }
          }
        }
      }
    }
    else {
      uVar3 = 0x15f1fd9;
      if (param_2 < 0x15f1fda) {
        if (param_2 == 0x15f1fd9) {
          if (param_1 == 1) {
            return 0x15f1fb9;
          }
        }
        else {
          uVar3 = 0x15e7edb;
          if (param_2 < 0x15e7edc) {
            if (param_2 == 0x15e7edb) {
              if (param_1 == 1) {
                return 0x15e7ebb;
              }
            }
            else {
              uVar3 = 0x144b2f0;
              if (param_2 == 0x144b2f0) {
                if (param_1 == 1) {
                  return 0x144b2d0;
                }
              }
              else {
                uVar3 = 0x144ca20;
                if (param_2 == 0x144ca20) {
                  if (param_1 == 1) {
                    return 0x144ca00;
                  }
                }
                else {
                  uVar3 = 0x144d300;
                  if (param_2 != 0x144d300) {
                    return 0;
                  }
                  if (param_1 == 1) {
                    return 0x144d2e0;
                  }
                }
              }
            }
          }
          else {
            uVar3 = 0x15e87db;
            if (param_2 == 0x15e87db) {
              if (param_1 == 1) {
                return 0x15e87bb;
              }
            }
            else {
              uVar3 = 0x15ebae9;
              if (param_2 == 0x15ebae9) {
                if (param_1 == 1) {
                  return 0x15ebac9;
                }
              }
              else {
                uVar3 = 0x15f1f29;
                if (param_2 != 0x15f1f29) {
                  return 0;
                }
                if (param_1 == 1) {
                  return 0x15f1f09;
                }
              }
            }
          }
        }
      }
      else {
        uVar3 = 0x1a77150;
        if (param_2 < 0x1a77151) {
          if (param_2 == 0x1a77150) {
            if (param_1 == 1) {
              return 0x1a83520;
            }
          }
          else {
            uVar3 = 0x15f20a0;
            if (param_2 == 0x15f20a0) {
              if (param_1 == 1) {
                return 0x15f2080;
              }
            }
            else {
              uVar3 = 0x15f7f60;
              if (param_2 == 0x15f7f60) {
                if (param_1 == 1) {
                  return 0x15f7f40;
                }
              }
              else {
                uVar3 = 0x1980980;
                if (param_2 != 0x1980980) {
                  return 0;
                }
                if (param_1 == 1) {
                  return 0x1980960;
                }
              }
            }
          }
        }
        else {
          uVar3 = 0x1a80d1d;
          if (param_2 == 0x1a80d1d) {
            if (param_1 == 1) {
              return 0x1a8d0ed;
            }
          }
          else {
            uVar3 = 0x1a9ad29;
            if (param_2 == 0x1a9ad29) {
              if (param_1 == 1) {
                return 0x1aa70f9;
              }
            }
            else {
              uVar3 = 0x1c81370;
              if (param_2 != 0x1c81370) {
                return 0;
              }
              if (param_1 == 1) {
                return 0x1c8d740;
              }
            }
          }
        }
      }
    }
    goto LAB_18000b056;
  }
  if (param_2 == 0x13ca5d0) {
    if (param_1 == 1) {
      return 0x13ca5b0;
    }
    goto LAB_18000b056;
  }
  uVar3 = 0x1335910;
  if (0x1335910 < param_2) {
    uVar3 = 0x13667e0;
    if (param_2 < 0x13667e1) {
      if (param_2 == 0x13667e0) {
        if (param_1 == 1) {
          return 0x13667c0;
        }
      }
      else {
        uVar3 = 0x134cd70;
        if (param_2 < 0x134cd71) {
          if (param_2 == 0x134cd70) {
            if (param_1 == 1) {
              return 0x134cd50;
            }
          }
          else {
            uVar3 = 0x1335b90;
            if (param_2 == 0x1335b90) {
              if (param_1 == 1) {
                return 0x1335b70;
              }
            }
            else {
              uVar3 = 0x1336080;
              if (param_2 == 0x1336080) {
                if (param_1 == 1) {
                  return 0x1336060;
                }
              }
              else {
                uVar3 = 0x134cae7;
                if (param_2 != 0x134cae7) {
                  return 0;
                }
                if (param_1 == 1) {
                  return 0x134cac7;
                }
              }
            }
          }
        }
        else {
          uVar3 = 0x134da0f;
          if (param_2 == 0x134da0f) {
            if (param_1 == 1) {
              return 0x134d9ef;
            }
          }
          else {
            uVar3 = 0x1359900;
            if (param_2 == 0x1359900) {
              if (param_1 == 1) {
                return 0x13598e0;
              }
            }
            else {
              uVar3 = 0x135d4e0;
              if (param_2 != 0x135d4e0) {
                return 0;
              }
              if (param_1 == 1) {
                return 0x135d4c0;
              }
            }
          }
        }
      }
    }
    else {
      uVar3 = 0x13ca3e0;
      if (param_2 < 0x13ca3e1) {
        if (param_2 == 0x13ca3e0) {
          if (param_1 == 1) {
            return 0x13ca3c0;
          }
        }
        else {
          uVar3 = 0x13ca2d0;
          if (param_2 != 0x13ca2d0) {
            if (param_2 == 0x13ca340) {
              if (param_1 == 1) {
                return 0x13ca320;
              }
              uVar3 = 0x13ca340;
            }
            else {
              if (param_2 != 0x13ca3b0) {
                return 0;
              }
              if (param_1 == 1) {
                return 0x13ca390;
              }
              uVar3 = 0x13ca3b0;
            }
            goto LAB_18000b0ed;
          }
          if (param_1 == 1) {
            return 0x13ca2b0;
          }
        }
      }
      else {
        uVar3 = 0x13ca4a0;
        if (param_2 != 0x13ca4a0) {
          if (param_2 == 0x13ca510) {
            if (param_1 == 1) {
              return 0x13ca4f0;
            }
            uVar3 = 0x13ca510;
          }
          else {
            if (param_2 != 0x13ca570) {
              return 0;
            }
            if (param_1 == 1) {
              return 0x13ca550;
            }
            uVar3 = 0x13ca570;
          }
LAB_18000b0ed:
          uVar2 = uVar3;
          if ((param_1 != 2) && (uVar2 = 0, param_1 == 3)) {
            uVar2 = uVar3;
          }
          return uVar2;
        }
        if (param_1 == 1) {
          return 0x13ca480;
        }
      }
    }
    goto LAB_18000b056;
  }
  if (param_2 == 0x1335910) {
    if (param_1 == 1) {
      return 0x13358f0;
    }
    goto LAB_18000b056;
  }
  uVar3 = 0x1154530;
  if (0x1154530 < param_2) {
    uVar3 = 0x1321130;
    if (param_2 < 0x1321131) {
      if (param_2 == 0x1321130) {
        if (param_1 == 1) {
          return 0x1321110;
        }
      }
      else {
        uVar3 = 0x1189e10;
        if (param_2 != 0x1189e10) {
          if (param_2 == 0x11d2580) {
            if (param_1 == 1) {
              return 0x11d2560;
            }
            uVar3 = 0x11d2580;
          }
          else {
            if (param_2 != 0x11d2592) {
              return 0;
            }
            if (param_1 == 1) {
              return 0x11d2572;
            }
            uVar3 = 0x11d2592;
          }
          goto LAB_18000b0ed;
        }
        if (param_1 == 1) {
          return 0x1189df0;
        }
      }
    }
    else {
      uVar3 = 0x1335400;
      if (param_2 == 0x1335400) {
        if (param_1 == 1) {
          return 0x13353e0;
        }
      }
      else {
        uVar3 = 0x13356b0;
        if (param_2 == 0x13356b0) {
          if (param_1 == 1) {
            return 0x1335690;
          }
        }
        else {
          uVar3 = 0x13357f0;
          if (param_2 != 0x13357f0) {
            return 0;
          }
          if (param_1 == 1) {
            return 0x13357d0;
          }
        }
      }
    }
    goto LAB_18000b056;
  }
  if (param_2 == 0x1154530) {
    if (param_1 == 1) {
      return 0x1154510;
    }
    goto LAB_18000b056;
  }
  uVar3 = 0x574140;
  if (param_2 < 0x574141) {
    if ((((param_2 != 0x574140) && (uVar3 = 0x16c8a0, param_2 != 0x16c8a0)) &&
        (uVar3 = 0x19e1f0, param_2 != 0x19e1f0)) && (uVar3 = 0x1abcf0, param_2 != 0x1abcf0)) {
      uVar3 = 0x1c4d70;
LAB_18000b048:
      if (param_2 != uVar3) {
        return 0;
      }
    }
  }
  else {
    uVar3 = 0x60f920;
    if ((param_2 != 0x60f920) && (uVar3 = 0x8c4c80, param_2 != 0x8c4c80)) {
      uVar3 = 0x8cb400;
      goto LAB_18000b048;
    }
  }
  if (param_1 == 1) {
    return uVar3;
  }
LAB_18000b056:
  uVar2 = uVar3;
  if ((param_1 != 2) && (uVar2 = 0, param_1 == 3)) {
    uVar2 = uVar3;
  }
  return uVar2;
}


inline std::uintptr_t ResolveOffset(std::uintptr_t value) {
 static const int build=DetectBuild();
 return build?static_cast<std::uintptr_t>(MapOffset(build,value)):0;
}
inline std::uintptr_t GameBase(){return reinterpret_cast<std::uintptr_t>(GetModuleHandleW(nullptr));}
inline bool Readable(const void* ptr,std::size_t length) {
 if(!ptr || !length)return false;
 MEMORY_BASIC_INFORMATION info{};
 if(VirtualQuery(ptr,&info,sizeof(info))!=sizeof(info) || info.State!=MEM_COMMIT)return false;
 if(info.Protect&(PAGE_GUARD|PAGE_NOACCESS))return false;
 const auto start=reinterpret_cast<std::uintptr_t>(ptr);
 const auto region=reinterpret_cast<std::uintptr_t>(info.BaseAddress);
 return start>=region && start+length>=start && start+length<=region+info.RegionSize;
}
}
