from pathlib import Path
import json,re
ROOT=Path(__file__).resolve().parent
SOURCE=ROOT/"source"/"ImGui DirectX 11 Kiero Hook"
import arabic_reshaper
from bidi.algorithm import get_display
from fontTools.ttLib import TTFont
from fontTools.varLib.instancer import instantiateVariableFont
from fontTools import subset

translations=json.loads((ROOT/'ui_ar.json').read_text(encoding='utf-8'))
def literal(value):return '"'+''.join(f'\\x{b:02x}' for b in value.encode('utf-8'))+'"'
glyphs=set(range(32,127));pairs=[]
for begin,end in [(0x600,0x700),(0x750,0x780),(0x8a0,0x900),(0xfb50,0xfe00),(0xfe70,0xff00)]:glyphs.update(range(begin,end))
for english,arabic in translations.items():
    english=english.replace('\\n','\n').replace('\\t','\t')
    marked=re.sub(r'%[a-z0-9.]*[dszuf]',lambda m:'\u200e'+m[0]+'\u200e',arabic)
    shaped=get_display(arabic_reshaper.reshape(marked)).replace('\u200e','')
    assert re.findall(r'%\w+',english)==re.findall(r'%\w+',shaped),(english,shaped)
    glyphs.update(map(ord,shaped));pairs.append((english.strip(),shaped))
header='#pragma once\n#include <unordered_map>\n#include <string>\nnamespace Exelight {\ninline const std::unordered_map<std::string,const char*> translations={\n'
for english,arabic in pairs:header+='    {'+literal(english)+','+literal(arabic)+'},\n'
header+='};\n}\n';(SOURCE/'exelight_translations.hpp').write_text(header,encoding='utf-8')
font=TTFont(SOURCE/'assets'/'NotoSansArabic.ttf')
required={ord(c) for _,text in pairs for c in text if ord(c)>127}
assert required.issubset(font.getBestCmap())
glyphs.intersection_update(font.getBestCmap())
font=instantiateVariableFont(font,{'wght':400,'wdth':100},inplace=True)
options=subset.Options();options.layout_features=[]
sub=subset.Subsetter(options=options);sub.populate(unicodes=glyphs);sub.subset(font)
font.save(SOURCE/'assets'/'exelight-arabic.ttf')
raw=(SOURCE/'assets'/'exelight-arabic.ttf').read_bytes()
header='#pragma once\nnamespace Exelight {\ninline unsigned char ArabicFontData[]={\n'
for i in range(0,len(raw),24):header+=','.join(f'0x{x:02x}' for x in raw[i:i+24])+',\n'
header+='};\n}\n';(SOURCE/'exelight_arabic_font.hpp').write_text(header)

print("Generated Arabic dictionary and embedded font.")
