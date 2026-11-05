from pathlib import Path
import os
import subprocess

root = Path(__file__).resolve().parent
source = root / 'source' / 'ImGui DirectX 11 Kiero Hook'
tool = (Path(os.environ['EXELIGHT_LLVM']) if os.environ.get('EXELIGHT_LLVM') else next((root / 'tools').glob('llvm-mingw*'))) / 'bin'
build = root / 'build'
objects = [build / ('imgui_' + name + '.cpp.o') for name in ['imgui', 'imgui_draw', 'imgui_widgets', 'imgui_tables', 'imgui_stdlib']]
args = [str(tool / 'clang++.exe'), '-std=c++20', '-O1', '-static', '-fstack-protector-strong', '-Wno-everything', '-I' + str(source), str(root / 'ui_checks.cpp'), *map(str, objects), '-limm32', '-o', str(build / 'ui_checks.exe')]
subprocess.run(args, check=True)
result = subprocess.run([str(build / 'ui_checks.exe')], cwd=build, capture_output=True, text=True, check=True)
(build / 'ui-checks.txt').write_text(result.stdout + result.stderr, encoding='utf-8')
print(result.stdout, end='')
