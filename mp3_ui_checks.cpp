#define NOMINMAX
#include <windows.h>
#include <atomic>
#include <cstdint>
#include <cstdio>
#include <cassert>
#include "exelight_ui.hpp"
#include "exelight_mp3.hpp"
#include "font.hpp"
std::atomic<std::uintptr_t> cgArray{0};
std::atomic<bool> bPointersInitialized{false};
namespace hooks {bool is_match_loaded(){return false;}}
static unsigned sentInput=0;
static UINT WINAPI DenyInput(UINT,LPINPUT,int){++sentInput;return 0;}
extern "C" {decltype(&SendInput) __imp_SendInput=DenyInput;}
int main() {
    ImGui::CreateContext();auto& io=ImGui::GetIO();io.IniFilename=nullptr;io.DisplaySize={980,640};io.DeltaTime=1.0f/60;
    ImFontConfig cfg{};cfg.FontDataOwnedByAtlas=false;io.Fonts->AddFontFromMemoryTTF(CustomFont,sizeof(CustomFont),18,&cfg);Exelight::MergeArabicFont();
    unsigned char* pixels{};int width{},height{};io.Fonts->GetTexDataAsRGBA32(&pixels,&width,&height);Exelight::ApplyTheme();
    for(int frame=0;frame<3;++frame) {
        Exelight::MP3::Frame(nullptr,true);ImGui::NewFrame();ImGui::SetNextWindowPos({0,0});ImGui::SetNextWindowSize({980,640});
        ImGui::Begin("exelight7 | MP3 3.2.4.7",nullptr,ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove);Exelight::DrawPreferences();
        if(ImGui::BeginTabBar("test_tabs")){Exelight::MP3::DrawTab();ImGui::EndTabBar();}
        ImGui::End();ImGui::Render();
    }
    auto data=ImGui::GetDrawData();assert(data&&data->TotalVtxCount>100);
    auto atlas=fopen("aim-ui-atlas.rgba","wb");assert(atlas);fwrite(pixels,4,width*height,atlas);fclose(atlas);
    auto file=fopen("aim-ui-mesh.json","wb");assert(file);
    fprintf(file,"{\"width\":980,\"height\":640,\"atlasWidth\":%d,\"atlasHeight\":%d,\"lists\":[",width,height);
    for(int i=0;i<data->CmdListsCount;++i){const auto list=data->CmdLists[i];if(i)fprintf(file,",");fprintf(file,"{\"vertices\":[");
        for(int j=0;j<list->VtxBuffer.Size;++j){const auto& v=list->VtxBuffer[j];if(j)fprintf(file,",");fprintf(file,"[%.4f,%.4f,%.8f,%.8f,%u]",v.pos.x,v.pos.y,v.uv.x,v.uv.y,v.col);}
        fprintf(file,"],\"indices\":[");for(int j=0;j<list->IdxBuffer.Size;++j){if(j)fprintf(file,",");fprintf(file,"%u",unsigned(list->IdxBuffer[j]));}
        fprintf(file,"],\"commands\":[");for(int j=0;j<list->CmdBuffer.Size;++j){const auto& c=list->CmdBuffer[j];assert(!c.UserCallback);if(j)fprintf(file,",");fprintf(file,"[%u,%u,%u,%.4f,%.4f,%.4f,%.4f]",c.ElemCount,c.IdxOffset,c.VtxOffset,c.ClipRect.x,c.ClipRect.y,c.ClipRect.z,c.ClipRect.w);}fprintf(file,"]}");
    }
    fprintf(file,"]}");fclose(file);ImGui::DestroyContext();assert(sentInput==0);
    puts("PASS: actual Arabic MP3 tab rendered in an unsupported host; SendInput replaced by a denying test stub; zero input calls and no game execution.");
}
