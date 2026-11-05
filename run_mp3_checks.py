from pathlib import Path
import os,subprocess
r=Path(__file__).resolve().parent;s=r/'source/ImGui DirectX 11 Kiero Hook';b=r/'build';tool=Path(os.environ['EXELIGHT_LLVM'])/'bin'
flags=['-std=c++20','-O1','-static','-fstack-protector-strong','-fms-extensions','-D_NO_OLDNAMES','-D_AMD64_','-Wno-everything','-Werror=non-pod-varargs','-Werror=format','-I'+str(s)]
imgui=[b/('imgui_'+name+'.cpp.o') for name in ['imgui','imgui_draw','imgui_widgets','imgui_tables','imgui_stdlib']]
for source,objects,name in [('mp3_checks.cpp',[],'mp3-checks'),('mp3_ui_checks.cpp',[b/'exelight_mp3.cpp.o',*imgui],'mp3-ui-checks'),('mp3_overlay_checks.cpp',imgui,'mp3-overlay-checks')]:
    subprocess.run([str(tool/'clang++.exe'),*flags,str(r/source),*map(str,objects),'-luser32','-limm32','-o',str(b/(name+'.exe'))],check=True)
    p=subprocess.run([str(b/(name+'.exe'))],cwd=b,capture_output=True,text=True,check=True,timeout=60)
    (b/(name+'.txt')).write_text(p.stdout+p.stderr,encoding='utf-8');print(p.stdout,end='')
