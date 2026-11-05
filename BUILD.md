# Build MP3 3.2.4.7

The tested route is Python 3.12 on Windows with x64 UCRT LLVM-MinGW. Complete source dependencies and generated Arabic headers are included. The compiler archive is downloaded separately.

Toolchain: [LLVM-MinGW 20261104 x64 UCRT](https://github.com/mstorsjo/llvm-mingw/releases/download/20261104/llvm-mingw-20261104-ucrt-x86_64.zip).

SHA256: `552191b5754c49a5a20723276a6fc56541ea1922c4c50b7b669022333fc1b40f`.

```powershell
$env:EXELIGHT_LLVM = 'C:\path\to\llvm-mingw-20261104-ucrt-x86_64'
python build_vendors.py
python build_project.py
python run_mp3_checks.py
python run_optimization_checks.py
python run_stability_checks.py
python run_ui_checks.py
python run_runtime_checks.py
python privacy_checks.py
python verify_artifact.py
```

The environment value names the folder containing `bin`. The DLL is `build/exelight7-3.2.4-MXT111-MP3.dll`. Public verification and privacy reports are generated under `build`. Start with a fresh build directory when changing toolchains.

`build_paths.py` applies compiler file/debug prefix mapping to both the main project and rebuilt dependencies. `GAMEPLAY_SOURCE_MANIFEST.json` verifies that the gameplay source is identical to the adopted MP3 source. The publication rebuild changes build-path handling, not gameplay features.

The compiler rejects unsafe non-POD varargs and format errors. The test programs use synthetic data. UI/ESP tests substitute a denying SendInput stub, and the artifact check loads the DLL only into a disposable unsupported Python process. They do not start BO3, send mouse input, or connect to a server. In-game operation remains unverified.

Optional preview: install NumPy and Pillow, then run `python render_mp3_preview.py` after the MP3 tests. To regenerate Arabic headers after editing the dictionary, install arabic-reshaper, python-bidi and fonttools and run `python generate_arabic.py`. Regenerating gameplay headers changes the source manifest and should be reviewed as a new source revision.

Original credits and component licenses are retained. See [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md) and [PRIVACY.md](PRIVACY.md).
