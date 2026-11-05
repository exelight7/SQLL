from pathlib import Path
from build_paths import privacy_flags
from concurrent.futures import ThreadPoolExecutor,as_completed
import subprocess,sys,os
root=Path(__file__).resolve().parent
tool=(Path(os.environ['EXELIGHT_LLVM']) if os.environ.get('EXELIGHT_LLVM') else next((root/'tools').glob('llvm-mingw*')))/'bin'
dest=root/'build'/'vendors';dest.mkdir(parents=True,exist_ok=True)
detours=root/'vendors/Detours-4.0.1/src'
header=detours/'detours.h'
text=header.read_text().replace('#if (_MSC_VER < 1299)','#if defined(_MSC_VER) && (_MSC_VER < 1299)')
header.write_text(text)
modules=detours/'modules.cpp'
text=modules.read_text()
if not text.startswith('#include <windows.h>'):text='#include <windows.h>\n#include <strsafe.h>\n'+text
text=text.replace('return GetProcAddress(hClr, "_CorExeMain");','return reinterpret_cast<PVOID>(GetProcAddress(hClr, "_CorExeMain"));')
modules.write_text(text)
jobs=[]
for name in ['hook.c','buffer.c','trampoline.c','hde/hde64.c']:
    jobs.append(('minhook',root/'vendors/minhook-1.3.4/src'/name))
for name in ['detours.cpp','modules.cpp','disasm.cpp']:
    jobs.append(('detours',detours/name))
def build(job):
    group,source=job;obj=dest/(group+'_'+source.name+'.o')
    if obj.exists() and obj.stat().st_mtime>max(Path(__file__).stat().st_mtime,(root/'build_paths.py').stat().st_mtime):return group,obj,0,''
    compiler=tool/('clang.exe' if source.suffix=='.c' else 'clang++.exe')
    args=[str(compiler),'-O2','-fstack-protector-strong','-ffunction-sections','-fdata-sections','-fms-extensions','-D_AMD64_','-DWIN32_LEAN_AND_MEAN','-Wno-everything','-ferror-limit=5','-c',str(source),'-o',str(obj)]
    args[1:1]=privacy_flags(root,tool.parent)
    p=subprocess.run(args,capture_output=True,text=True,encoding='utf-8',errors='replace')
    (dest/(source.name+'.log')).write_text(p.stdout+p.stderr)
    return group,obj,p.returncode,p.stdout+p.stderr
groups={'minhook':[],'detours':[]};failed=False
with ThreadPoolExecutor(max_workers=4) as pool:
    for f in as_completed([pool.submit(build,j) for j in jobs]):
        group,obj,code,output=f.result();print(('FAIL ' if code else 'OK ')+obj.name,flush=True)
        if code:failed=True;print(output,flush=True)
        else:groups[group].append(str(obj))
if failed:sys.exit(1)
for group,objects in groups.items():
    subprocess.run([str(tool/'llvm-ar.exe'),'rcs',str(root/'build'/('lib'+group+'_rebuilt.a')),*objects],check=True)
print('VENDORS_READY',flush=True)
