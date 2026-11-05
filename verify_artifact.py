from pathlib import Path
import ctypes
from ctypes import wintypes
import hashlib,json,struct,subprocess,sys
from privacy_scan import scan

root=Path(__file__).resolve().parent;build=root/'build'
dll=build/'exelight7-3.2.4-MXT111-MP3.dll';data=dll.read_bytes()
assert data[:2]==b'MZ'
pe=struct.unpack_from('<I',data,0x3c)[0];assert data[pe:pe+4]==b'PE\0\0'
machine,sections=struct.unpack_from('<HH',data,pe+4)
optional_size,characteristics=struct.unpack_from('<HH',data,pe+20);optional=pe+24
assert machine==0x8664 and characteristics&0x2000 and struct.unpack_from('<H',data,optional)[0]==0x20b
assert struct.unpack_from('<H',data,optional+70)[0]&0x160==0x160
for i in range(sections):
    flags=struct.unpack_from('<I',data,optional+optional_size+i*40+36)[0]
    assert flags&0xa0000000!=0xa0000000
version=ctypes.WinDLL('version',use_last_error=True)
version.GetFileVersionInfoSizeW.argtypes=[wintypes.LPCWSTR,ctypes.POINTER(wintypes.DWORD)]
version.GetFileVersionInfoSizeW.restype=wintypes.DWORD
version.GetFileVersionInfoW.argtypes=[wintypes.LPCWSTR,wintypes.DWORD,wintypes.DWORD,ctypes.c_void_p]
version.GetFileVersionInfoW.restype=wintypes.BOOL
version.VerQueryValueW.argtypes=[ctypes.c_void_p,wintypes.LPCWSTR,ctypes.POINTER(ctypes.c_void_p),ctypes.POINTER(wintypes.UINT)]
version.VerQueryValueW.restype=wintypes.BOOL
ignored=wintypes.DWORD();size=version.GetFileVersionInfoSizeW(str(dll),ctypes.byref(ignored));assert size
buffer=ctypes.create_string_buffer(size);assert version.GetFileVersionInfoW(str(dll),0,size,buffer)
pointer=ctypes.c_void_p();length=wintypes.UINT()
assert version.VerQueryValueW(buffer,'\\',ctypes.byref(pointer),ctypes.byref(length)) and length.value>=52
fixed=struct.unpack('<13I',ctypes.string_at(pointer,52));assert fixed[0]==0xfeef04bd and fixed[2:6]==(0x30002,0x40007,0x30002,0x40007)
assert version.VerQueryValueW(buffer,'\\StringFileInfo\\040904b0\\OriginalFilename',ctypes.byref(pointer),ctypes.byref(length))
assert ctypes.wstring_at(pointer)==dll.name
loader='''import ctypes,sys
ctypes.windll.kernel32.SetErrorMode(0x8003)
try: ctypes.WinDLL(sys.argv[1])
except OSError as error:
    if error.winerror!=1114: raise
    print("PASS: unsupported host refused cleanly with error 1114.")
else: raise SystemExit("Unexpected load in unsupported host")
'''
p=subprocess.run([sys.executable,'-c',loader,str(dll)],capture_output=True,text=True,timeout=20)
assert p.returncode==0,p.stdout+p.stderr
manifest=json.loads((root/'GAMEPLAY_SOURCE_MANIFEST.json').read_text(encoding='utf-8'))
for name,digest in manifest['source_files'].items():assert hashlib.sha256((root/name).read_bytes()).hexdigest()==digest,name
tests={}
for name in ['mp3-checks','mp3-ui-checks','mp3-overlay-checks','optimization-checks','stability-checks','notification-checks','varargs-regression','format-regression','ui-checks','runtime-checks']:
    lines=(build/(name+'.txt')).read_text(encoding='utf-8').splitlines();assert lines and lines[0].startswith('PASS:')
    tests[name]=[line for line in lines if line.startswith('PASS:')]
privacy=scan(root,[dll]);assert privacy['passed'],privacy['findings']
(build/'privacy-scan.json').write_text(json.dumps(privacy,indent=2),encoding='utf-8')
report={'filename':dll.name,'version':'3.2.4.7','edition':'MP3','sha256':hashlib.sha256(data).hexdigest(),'size':len(data),
        'architecture':'x64','aslr_nx_high_entropy':True,'no_writable_executable_pe_sections':True,
        'unsupported_host_rejection':True,'tests':tests,'gameplay_source_identity_verified':True,
        'gameplay_source_files_verified':len(manifest['source_files']),'privacy_scan':privacy,
        'provenance':json.loads((root/'PROVENANCE.json').read_text(encoding='utf-8')),
        'aim_and_esp_tested_in_game':False,'flight_implemented':False,
        'limits':'Standalone checks only. Public multiplayer behavior and complete runtime stability remain unverified.'}
(build/'verification.json').write_text(json.dumps(report,ensure_ascii=False,indent=2),encoding='utf-8')
message='PASS: x64/version, ASLR/NX, clean unsupported-host refusal, unchanged MP3 gameplay source and publication privacy checks.\n'
(build/'artifact-checks.txt').write_text(message,encoding='utf-8');print(message,end='')
