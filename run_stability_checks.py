from pathlib import Path
import os,subprocess
r=Path(__file__).resolve().parent;s=r/'source/ImGui DirectX 11 Kiero Hook';b=r/'build';tool=Path(os.environ['EXELIGHT_LLVM'])/'bin'
flags=['-std=c++20','-O1','-static','-fstack-protector-strong','-Wno-everything','-Werror=non-pod-varargs','-Werror=format','-I'+str(s)]
objects=[b/('imgui_'+name+'.cpp.o') for name in ['imgui','imgui_draw','imgui_widgets','imgui_tables','imgui_stdlib']]
for source,extra,name in [('stability_checks.cpp',[],'stability-checks'),('notification_checks.cpp',objects,'notification-checks')]:
    subprocess.run([str(tool/'clang++.exe'),*flags,str(r/source),*map(str,extra),'-limm32','-o',str(b/(name+'.exe'))],check=True)
    p=subprocess.run([str(b/(name+'.exe'))],capture_output=True,text=True,cwd=b,check=True,timeout=60)
    (b/(name+'.txt')).write_text(p.stdout+p.stderr,encoding='utf-8');print(p.stdout,end='')
fixtures={
    'varargs-regression':('#include <string>\nvoid sink(const char*,...);void test(){sink("%s",std::string("3.2.4"));}\n',['non-pod-varargs']),
    'format-regression':('#include "exelight_ui.hpp"\nvoid test(const char* external){Exelight::Ui::Text("%s",42);Exelight::Ui::Text(external);}\n',['format specifies type','format string is not a string literal'])
}
for name,(text,diagnostics) in fixtures.items():
    file=b/(name+'.cpp');file.write_text(text)
    p=subprocess.run([str(tool/'clang++.exe'),*flags,'-DNOMINMAX','-c',str(file),'-o',str(b/(name+'.o'))],capture_output=True,text=True)
    assert p.returncode!=0 and all(d in p.stderr for d in diagnostics),p.stderr
    message='PASS: '+name+' unsafe pattern rejected by compiler.\n'
    (b/(name+'.txt')).write_text(message+p.stderr,encoding='utf-8');print(message,end='')
