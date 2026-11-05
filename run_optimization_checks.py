from pathlib import Path
import os,subprocess
r=Path(__file__).resolve().parent;s=r/'source/ImGui DirectX 11 Kiero Hook';b=r/'build';tool=Path(os.environ['EXELIGHT_LLVM'])/'bin'
subprocess.run([str(tool/'clang++.exe'),'-std=c++20','-O2','-static','-fstack-protector-strong','-Wno-everything','-I'+str(s),str(r/'optimization_checks.cpp'),'-o',str(b/'optimization-checks.exe')],check=True)
p=subprocess.run([str(b/'optimization-checks.exe')],capture_output=True,text=True,check=True,timeout=60)
(b/'optimization-checks.txt').write_text(p.stdout+p.stderr,encoding='utf-8');print(p.stdout,end='')
