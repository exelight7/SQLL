from pathlib import Path
import os
import subprocess

root = Path(__file__).resolve().parent
source = root / 'source' / 'ImGui DirectX 11 Kiero Hook'
tool = (Path(os.environ['EXELIGHT_LLVM']) if os.environ.get('EXELIGHT_LLVM') else next((root / 'tools').glob('llvm-mingw*'))) / 'bin'
build = root / 'build'
build.mkdir(exist_ok=True)
subprocess.run([str(tool / 'clang++.exe'), '-std=c++20', '-O1', '-static', '-fstack-protector-strong', '-Wno-everything', '-I' + str(source), str(root / 'runtime_checks.cpp'), '-o', str(build / 'runtime_checks.exe')], check=True)
result = subprocess.run([str(build / 'runtime_checks.exe')], cwd=build, capture_output=True, text=True, check=True)
(build / 'runtime-checks.txt').write_text(result.stdout + result.stderr, encoding='utf-8')
print(result.stdout, end='')
