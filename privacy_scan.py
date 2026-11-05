"""Screen publication inputs without printing matched sensitive values."""
from pathlib import Path
import argparse
import getpass
import ipaddress
import json
import re
import subprocess
import struct
import zipfile

VERSIONS = {'3.2.4.7', '3.2.4.6', '3.2.4.5', '3.2.4.4', '3.2.4.3', '3.2.4.2',
            '3.2.4.1', '3.2.4.0', '1.3.4.0', '1.0.0.0', '4.0.1.0', '0.0.0.0'}
LOCAL_PATH = re.compile(r'(?i:[a-z]:[\\/]Users[\\/][^\\/\s]+)'
                        + r'|/' + r'Users/[^/\s]+' + r'|/' + r'home/(?!runner/|buildbot/)[^/\s]+')
SECRET = re.compile(r'gh[pousr]_[A-Za-z0-9_]{20,}|github_pat_[A-Za-z0-9_]{20,}|AKIA[0-9A-Z]{16}|-----BEGIN (?:RSA |EC |OPENSSH )?PRIVATE KEY')
IPV4 = re.compile(r'(?<![\w.])(?:\d{1,3}\.){3}\d{1,3}(?![\w.])')
IPV6 = re.compile(r'(?<!\w)(?:[0-9a-fA-F]{0,4}:){2,7}[0-9a-fA-F]{0,4}(?!\w)')

def sensitive_kinds(text: str, account: str, check_ip=True):
    kinds = set()
    if LOCAL_PATH.search(text): kinds.add('local-user-path')
    if len(account) >= 4 and account.lower() in text.lower(): kinds.add('build-account-name')
    if SECRET.search(text): kinds.add('credential-pattern')
    if not check_ip: return kinds
    for candidate in IPV4.findall(text):
        try: ipaddress.IPv4Address(candidate)
        except ValueError: continue
        if candidate not in VERSIONS: kinds.add('unexpected-literal-ipv4')
    for candidate in IPV6.findall(text):
        groups = [part for part in candidate.split(':') if part]
        # Reject C++ labels and short glyph-byte coincidences before parsing.
        if len(groups) < 2 or not candidate.split(':')[0] or len(candidate.split(':')[0]) < 2: continue
        try: ipaddress.IPv6Address(candidate)
        except ValueError: continue
        kinds.add('unexpected-literal-ipv6')
    return kinds

def font_metadata(raw: bytes):
    """Read TTF name records; glyph instruction bytes are not text addresses."""
    if len(raw)<12: raise ValueError('Invalid font')
    tables=struct.unpack_from('>H',raw,4)[0]
    if tables>4096 or 12+tables*16>len(raw): raise ValueError('Invalid font directory')
    for i in range(tables):
        at=12+i*16
        if raw[at:at+4]!=b'name': continue
        offset,length=struct.unpack_from('>II',raw,at+8)
        if offset+length>len(raw) or length<6: raise ValueError('Invalid font names')
        count,strings=struct.unpack_from('>HH',raw,offset+2)
        if 6+count*12>length: raise ValueError('Invalid font records')
        names=[]
        for j in range(count):
            platform,_,_,_,size,start=struct.unpack_from('>6H',raw,offset+6+j*12)
            begin=offset+strings+start
            if begin+size>offset+length: raise ValueError('Invalid font string')
            names.append(raw[begin:begin+size].decode('utf-16be' if platform in (0,3) else 'mac_roman',errors='replace'))
        return '\n'.join(names)
    raise ValueError('Font name table missing')

def public_files(root: Path):
    p = subprocess.run(['git', 'ls-files', '--cached', '--others', '--exclude-standard', '-z'],
                       cwd=root, capture_output=True, check=True)
    return sorted({root / name.decode('utf-8') for name in p.stdout.split(b'\0') if name})

def scan(root: Path, artifacts=()):
    findings = []
    account = getpass.getuser().lower()
    checked = 0
    def inspect(name: str, raw: bytes):
        nonlocal checked
        checked += 1
        strings = [raw.decode('utf-8', errors='ignore'), raw.decode('utf-16le', errors='ignore')]
        kinds = set()
        for text in strings:
            kinds.update(sensitive_kinds(text, account, check_ip=not name.lower().endswith('.ttf')))
        if name.lower().endswith('.ttf'):
            try: kinds.update(sensitive_kinds(font_metadata(raw),account))
            except (ValueError,struct.error): kinds.add('invalid-font-metadata')
        for kind in sorted(kinds): findings.append({'file': name, 'category': kind})
    files = public_files(root)
    for path in files:
        if not path.is_file(): continue
        relative = path.relative_to(root).as_posix()
        if path.suffix.lower() in {'.log', '.ini', '.dmp', '.gltn', '.pdb', '.key', '.pem'} or path.name.startswith('.env'):
            findings.append({'file': relative, 'category': 'private-file-type'})
        inspect(relative, path.read_bytes())
    for path in map(Path, artifacts):
        if zipfile.is_zipfile(path):
            with zipfile.ZipFile(path) as archive:
                if archive.testzip() is not None: findings.append({'file': path.name, 'category': 'invalid-archive'})
                for info in archive.infolist():
                    if not info.is_dir(): inspect(path.name + ':' + info.filename, archive.read(info.filename))
        else: inspect(path.name, path.read_bytes())
    return {'passed': not findings, 'checked_inputs': checked, 'source_files': len(files),
            'findings': findings, 'matched_values_omitted': True,
            'scope': 'Published files; recognized software version literals are excluded from IP findings.'}

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--artifact', action='append', default=[])
    parser.add_argument('--output', default='build/privacy-scan.json')
    args = parser.parse_args()
    root = Path(__file__).resolve().parent
    report = scan(root, args.artifact)
    destination = root / args.output
    destination.parent.mkdir(parents=True, exist_ok=True)
    destination.write_text(json.dumps(report, indent=2), encoding='utf-8')
    print(json.dumps(report, indent=2))
    raise SystemExit(0 if report['passed'] else 1)
