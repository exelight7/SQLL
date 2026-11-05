#include <windows.h>
#include <cassert>
#include <cstdio>
#include "exelight_offsets.hpp"
#include "exelight_ui.hpp"
#include "font.hpp"
int main(){
 assert(Exelight::DetectBuild()==0); // The test process is not BO3; reject it.
 assert(!Exelight::Readable(nullptr,8));
 int value=7;assert(Exelight::Readable(&value,sizeof(value)));
 ImGui::CreateContext();auto& io=ImGui::GetIO();
 ImFontConfig cfg{};cfg.FontDataOwnedByAtlas=false;
 auto font=io.Fonts->AddFontFromMemoryTTF(CustomFont,sizeof(CustomFont),18,&cfg);
 Exelight::MergeArabicFont();
 unsigned char* pixels{};int width{},height{};io.Fonts->GetTexDataAsRGBA32(&pixels,&width,&height);
 assert(pixels && width>0 && height>0);
 unsigned glyphs=0;
 for(const auto& entry:Exelight::translations){
  const unsigned char* p=reinterpret_cast<const unsigned char*>(entry.second);
  while(*p){unsigned c=*p++;
   if(c>=0xc0 && c<0xe0){c=((c&31)<<6)|(*p++&63);}
   else if(c>=0xe0 && c<0xf0){unsigned a=*p++,b=*p++;c=((c&15)<<12)|((a&63)<<6)|(b&63);}
   assert(c<0x10000);assert(font->FindGlyphNoFallback(static_cast<ImWchar>(c))!=nullptr);++glyphs;
  }
 }
 io.DisplaySize=ImVec2(1100,760);io.DeltaTime=1.0f/60;
 ImGui::NewFrame();ImGui::Begin("stable_id_test");
 Exelight::arabic=true;auto arabicId=ImGui::GetID(Exelight::Label("Send##LEVEL").c_str());
 Exelight::arabic=false;auto englishId=ImGui::GetID(Exelight::Label("Send##LEVEL").c_str());
 assert(arabicId==englishId);
 assert(englishId!=ImGui::GetID(Exelight::Label("Send##PRESTIGE").c_str()));
 Exelight::Ui::TextUnformatted("Player 100% %n %s");
 ImGui::End();ImGui::Render();
 wchar_t path[32768]{};GetModuleFileNameW(nullptr,path,32768);
 Exelight::settingsFile=(std::filesystem::path(path).parent_path()/L"qa-interface.ini").wstring();
 Exelight::arabic=true;Exelight::interfaceScale=1.25f;assert(Exelight::SavePreferences());
 assert(GetPrivateProfileIntW(L"Interface",L"Arabic",0,Exelight::settingsFile.c_str())==1);
 assert(GetPrivateProfileIntW(L"Interface",L"ScalePercent",0,Exelight::settingsFile.c_str())==125);
 ImGui::DestroyContext();
 std::printf("PASS: %zu translations, %u translated characters, fonts %dx%d, stable IDs, persisted settings, unsupported-host rejection.\n",Exelight::translations.size(),glyphs,width,height);
 return 0;
}
