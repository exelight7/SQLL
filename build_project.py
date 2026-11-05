from pathlib import Path
from build_paths import privacy_flags
import subprocess,json,time,sys,os
from concurrent.futures import ThreadPoolExecutor,as_completed
root=Path(__file__).resolve().parent
source=root/'source'/'ImGui DirectX 11 Kiero Hook'
tool=(Path(os.environ['EXELIGHT_LLVM']) if os.environ.get('EXELIGHT_LLVM') else next((root/'tools').glob('llvm-mingw*')))/'bin'
build=root/'build';build.mkdir(exist_ok=True)
header_time=max(p.stat().st_mtime for p in source.rglob('*') if p.suffix in {'.h','.hpp'})
compiler=tool/'x86_64-w64-mingw32-clang++.exe'
flags=['-std=c++20','-O2','-fstack-protector-strong','-fms-extensions','-D_NO_OLDNAMES','-D_AMD64_','-DWIN32_LEAN_AND_MEAN','-Wno-everything','-Werror=non-pod-varargs','-Werror=format','-ferror-limit=10','-static','-I'+str(source),'-I'+str(source/'imgui')]
flags+=privacy_flags(root,tool.parent)
files=['exelight_mp3.cpp','common.cpp','hooks.cpp','main.cpp','host.cpp','Players.cpp','servers.cpp']
files+=['imgui/'+name for name in ['imgui.cpp','imgui_draw.cpp','imgui_impl_dx11.cpp','imgui_impl_win32.cpp','imgui_stdlib.cpp','imgui_tables.cpp','imgui_texteditor.cpp','imgui_widgets.cpp']]
def compile_file(name):
    obj=build/(name.replace('/','_')+'.o')
    log=build/(name.replace('/','_')+'.log')
    if obj.exists() and obj.stat().st_mtime>max(max(Path(__file__).stat().st_mtime,(root/'build_paths.py').stat().st_mtime),(source/name).stat().st_mtime,header_time):return name,True,'cached'
    command=[str(compiler),*flags,'-c',str(source/name),'-o',str(obj)]
    p=subprocess.run(command,cwd=source,stdout=subprocess.PIPE,stderr=subprocess.STDOUT,text=True,encoding='utf-8',errors='replace')
    log.write_text(p.stdout,encoding='utf-8')
    return name,p.returncode==0,p.stdout[-10000:]
failed=[]
with ThreadPoolExecutor(max_workers=3) as pool:
    for future in as_completed([pool.submit(compile_file,name) for name in files]):
        name,ok,output=future.result()
        print(('OK ' if ok else 'FAIL ')+name,flush=True)
        if not ok:failed.append(name);print(output,flush=True)
if failed:sys.exit(1)
asm=subprocess.run([str(compiler),'-c',str(source/'spoof_gnu.S'),'-o',str(build/'spoof.o')],capture_output=True,text=True)
if asm.returncode:print(asm.stderr);sys.exit(asm.returncode)
resource=source/'exelight_version.rc'
rc=subprocess.run([str(tool/'x86_64-w64-mingw32-windres.exe'),'-I',str(source),str(resource),str(build/'version.o')],cwd=source,capture_output=True,text=True,encoding='utf-8',errors='replace')
if rc.returncode:print('RESOURCE_ERROR',rc.stdout,rc.stderr);sys.exit(1)
objects=[str(build/(name.replace('/','_')+'.o')) for name in files]+[str(build/'spoof.o'),str(build/'version.o')]
command=[str(compiler),'-shared','-static','-fstack-protector-strong','-o',str(build/'exelight7-3.2.4-MXT111-MP3.dll'),*objects,str(build/'libminhook_rebuilt.a'),str(build/'libdetours_rebuilt.a'),'-ld3d11','-ldxgi','-ld3dcompiler','-lwindowscodecs','-lole32','-luuid','-limm32','-lpsapi','-lxinput9_1_0','-luser32','-lgdi32','-ldwmapi','-lshlwapi','-lversion','-ladvapi32','-Wl,--no-insert-timestamp','-Wl,--exclude-all-symbols']
link=subprocess.run(command,cwd=source,capture_output=True,text=True,encoding='utf-8',errors='replace')
(build/'link.log').write_text(link.stdout+link.stderr,encoding='utf-8')
print(link.stdout+link.stderr,flush=True)
if link.returncode:sys.exit(link.returncode)
print('BUILT '+str(build/'exelight7-3.2.4-MXT111-MP3.dll'),flush=True)
