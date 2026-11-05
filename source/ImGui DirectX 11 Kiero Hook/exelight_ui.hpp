#pragma once
#include <windows.h>
#include <atomic>
#include <filesystem>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <cstdarg>
#include <cstring>
#include <vector>
#include "imgui/imgui.h"
#include "imgui/imgui_stdlib.h"
#include "exelight_translations.hpp"
#include "exelight_arabic_font.hpp"

namespace Exelight {
inline bool arabic = true;
inline float interfaceScale = 1.0f;
inline std::wstring settingsFile;
inline std::string layoutFile;
inline bool saveFailed = false;

inline std::string Trim(std::string value) {
    auto first = value.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return {};
    return value.substr(first,value.find_last_not_of(" \t\r\n")-first+1);
}
inline const char* Text(const char* value) {
    if (!value || !arabic) return value ? value : "";
    auto it=translations.find(Trim(value));
    return it==translations.end()?value:it->second;
}
inline std::string Label(const char* value) {
    if (!value) return {};
    std::string original(value);
    const auto visible=original.substr(0,original.find("##"));
    if (visible.empty()) return original;
    // Same explicit ID in both languages. Language changes do not reset controls.
    return std::string(Text(visible.c_str()))+"###"+original;
}
inline void LoadPreferences(HMODULE module) {
    wchar_t path[32768]{};
    const DWORD length=GetModuleFileNameW(module,path,32768);
    if (!length || length>=32768) return;
    const auto folder=std::filesystem::path(path).parent_path()/L"exelight7";
    std::error_code ec;
    std::filesystem::create_directories(folder,ec);
    settingsFile=(folder/L"interface.ini").wstring();
    auto layout=(folder/L"layout.ini").u8string();
    layoutFile.assign(reinterpret_cast<const char*>(layout.data()),layout.size());
    arabic=GetPrivateProfileIntW(L"Interface",L"Arabic",1,settingsFile.c_str())!=0;
    int scale=GetPrivateProfileIntW(L"Interface",L"ScalePercent",100,settingsFile.c_str());
    if (scale<85 || scale>150) scale=100;
    interfaceScale=scale/100.0f;
}
inline bool SavePreferences() {
    if (settingsFile.empty()) { saveFailed=true; return false; }
    const auto percent=std::to_wstring(static_cast<int>(interfaceScale*100+0.5f));
    bool ok=WritePrivateProfileStringW(L"Interface",L"Arabic",arabic?L"1":L"0",settingsFile.c_str())!=0;
    ok=(WritePrivateProfileStringW(L"Interface",L"ScalePercent",percent.c_str(),settingsFile.c_str())!=0)&&ok;
    saveFailed=!ok;
    return ok;
}
inline void MergeArabicFont() {
    static const ImWchar ranges[]={0x0600,0x06ff,0x0750,0x077f,0x08a0,0x08ff,0xfb50,0xfdff,0xfe70,0xfeff,0};
    ImFontConfig cfg{};
    cfg.MergeMode=true;
    cfg.FontDataOwnedByAtlas=false;
    cfg.PixelSnapH=true;
    ImGui::GetIO().Fonts->AddFontFromMemoryTTF(ArabicFontData,sizeof(ArabicFontData),30.0f,&cfg,ranges);
    if (!layoutFile.empty()) ImGui::GetIO().IniFilename=layoutFile.c_str();
    ImGui::GetIO().FontGlobalScale=interfaceScale;
}
inline void ApplyTheme() {
    auto& s=ImGui::GetStyle();
    s.WindowRounding=10; s.ChildRounding=8; s.FrameRounding=5;
    s.PopupRounding=7; s.ScrollbarRounding=6; s.GrabRounding=4;
    s.WindowPadding=ImVec2(14,12); s.FramePadding=ImVec2(7,5);
    s.ItemSpacing=ImVec2(8,7); s.ItemInnerSpacing=ImVec2(6,4);
    s.WindowBorderSize=1; s.FrameBorderSize=0; s.ChildBorderSize=1;
    s.WindowTitleAlign=ImVec2(arabic?1.0f:0.0f,0.5f);
    s.Colors[ImGuiCol_WindowBg]=ImVec4(.045f,.06f,.09f,.98f);
    s.Colors[ImGuiCol_ChildBg]=ImVec4(.055f,.075f,.11f,.92f);
    s.Colors[ImGuiCol_Text]=ImVec4(.92f,.95f,.99f,1);
    s.Colors[ImGuiCol_TextDisabled]=ImVec4(.51f,.59f,.69f,1);
    s.Colors[ImGuiCol_Border]=ImVec4(.14f,.22f,.32f,1);
    s.Colors[ImGuiCol_TitleBg]=ImVec4(.055f,.09f,.15f,1);
    s.Colors[ImGuiCol_TitleBgActive]=ImVec4(.07f,.16f,.24f,1);
    s.Colors[ImGuiCol_FrameBg]=ImVec4(.10f,.15f,.21f,1);
    s.Colors[ImGuiCol_FrameBgHovered]=ImVec4(.12f,.25f,.33f,1);
    s.Colors[ImGuiCol_FrameBgActive]=ImVec4(.10f,.32f,.40f,1);
    s.Colors[ImGuiCol_Button]=ImVec4(.09f,.26f,.35f,1);
    s.Colors[ImGuiCol_ButtonHovered]=ImVec4(.08f,.42f,.53f,1);
    s.Colors[ImGuiCol_ButtonActive]=ImVec4(.05f,.50f,.61f,1);
    s.Colors[ImGuiCol_Header]=s.Colors[ImGuiCol_Button];
    s.Colors[ImGuiCol_HeaderHovered]=s.Colors[ImGuiCol_ButtonHovered];
    s.Colors[ImGuiCol_HeaderActive]=s.Colors[ImGuiCol_ButtonActive];
    s.Colors[ImGuiCol_Tab]=ImVec4(.07f,.14f,.20f,1);
    s.Colors[ImGuiCol_TabActive]=ImVec4(.08f,.35f,.44f,1);
    s.Colors[ImGuiCol_TabHovered]=ImVec4(.09f,.43f,.52f,1);
    s.Colors[ImGuiCol_CheckMark]=ImVec4(.26f,.87f,.94f,1);
    s.Colors[ImGuiCol_SliderGrab]=s.Colors[ImGuiCol_CheckMark];
    s.Colors[ImGuiCol_SliderGrabActive]=ImVec4(.45f,.97f,1,1);
    ImGui::GetIO().FontGlobalScale=interfaceScale;
}
namespace Ui {
#define EXELIGHT_LABEL_WIDGET(name) template<class... Args> inline auto name(const char* label,Args&&...args) { const auto translated=Label(label); return ImGui::name(translated.c_str(),std::forward<Args>(args)...); }
EXELIGHT_LABEL_WIDGET(Button)
EXELIGHT_LABEL_WIDGET(Checkbox)
EXELIGHT_LABEL_WIDGET(BeginTabItem)
EXELIGHT_LABEL_WIDGET(CollapsingHeader)
EXELIGHT_LABEL_WIDGET(InputInt)
EXELIGHT_LABEL_WIDGET(InputFloat)
EXELIGHT_LABEL_WIDGET(InputText)
EXELIGHT_LABEL_WIDGET(InputTextMultiline)
EXELIGHT_LABEL_WIDGET(SliderInt)
EXELIGHT_LABEL_WIDGET(SliderFloat)
EXELIGHT_LABEL_WIDGET(DragInt)
EXELIGHT_LABEL_WIDGET(DragFloat)
EXELIGHT_LABEL_WIDGET(TreeNode)
EXELIGHT_LABEL_WIDGET(Selectable)
EXELIGHT_LABEL_WIDGET(BeginMenu)
EXELIGHT_LABEL_WIDGET(MenuItem)
EXELIGHT_LABEL_WIDGET(RadioButton)
EXELIGHT_LABEL_WIDGET(TableSetupColumn)
#undef EXELIGHT_LABEL_WIDGET
inline bool Checkbox(const char* label,std::atomic<bool>* value) {
    bool current=value->load(std::memory_order_relaxed);const auto translated=Label(label);
    const bool changed=ImGui::Checkbox(translated.c_str(),&current);if(changed)value->store(current,std::memory_order_relaxed);return changed;
}
inline bool SliderInt(const char* label,std::atomic<int>* value,int minimum,int maximum) {
    int current=value->load(std::memory_order_relaxed);const auto translated=Label(label);
    const bool changed=ImGui::SliderInt(translated.c_str(),&current,minimum,maximum);if(changed)value->store(current,std::memory_order_relaxed);return changed;
}
template<class... Args> inline bool InputTextWithHint(const char* label,const char* hint,Args&&...args) {
    const auto translated=Label(label);
    return ImGui::InputTextWithHint(translated.c_str(),Exelight::Text(hint),std::forward<Args>(args)...);
}
inline bool Combo(const char* label,int* selected,const char*const items[],int count,int height=-1) {
    std::vector<std::string> labels; labels.reserve(count);
    std::vector<const char*> values; values.reserve(count);
    for(int i=0;i<count;++i)labels.emplace_back(Exelight::Text(items[i]));
    for(auto& item:labels)values.push_back(item.c_str());
    const auto translated=Label(label);
    return ImGui::Combo(translated.c_str(),selected,values.data(),count,height);
}
inline bool Combo(const char* label,int* selected,const char* items,int height=-1) {
    std::string list;
    for(auto p=items;*p;p+=std::strlen(p)+1){list+=Exelight::Text(p);list+='\0';}
    list+='\0'; const auto translated=Label(label);
    return ImGui::Combo(translated.c_str(),selected,list.c_str(),height);
}
inline bool Combo(const char* label,int* selected,bool(*getter)(void*,int,const char**),void* data,int count,int height=-1) {
    const auto translated=Label(label);return ImGui::Combo(translated.c_str(),selected,getter,data,count,height);
}
inline void TextUnformatted(const char* text,const char* end=nullptr) {
    if(end){const std::string s(text,end);ImGui::TextUnformatted(Exelight::Text(s.c_str()));}
    else ImGui::TextUnformatted(Exelight::Text(text));
}
inline void Text(const char* format,...) IM_FMTARGS(1);
inline void TextWrapped(const char* format,...) IM_FMTARGS(1);
inline void TextColored(const ImVec4& color,const char* format,...) IM_FMTARGS(2);
inline void TextDisabled(const char* format,...) IM_FMTARGS(1);
inline void SetTooltip(const char* format,...) IM_FMTARGS(1);
inline void Text(const char* format,...) {va_list args;va_start(args,format);ImGui::TextV(Exelight::Text(format),args);va_end(args);}
inline void TextWrapped(const char* format,...) {va_list args;va_start(args,format);ImGui::TextWrappedV(Exelight::Text(format),args);va_end(args);}
inline void TextColored(const ImVec4& color,const char* format,...) {va_list args;va_start(args,format);ImGui::TextColoredV(color,Exelight::Text(format),args);va_end(args);}
inline void TextDisabled(const char* format,...) {va_list args;va_start(args,format);ImGui::TextDisabledV(Exelight::Text(format),args);va_end(args);}
inline void SetTooltip(const char* format,...) {va_list args;va_start(args,format);ImGui::SetTooltipV(Exelight::Text(format),args);va_end(args);}
}
inline void DrawPreferences() {
    ImGui::TextColored(ImVec4(.26f,.87f,.94f,1),"exelight7 / 3.2.4");
    ImGui::SameLine(); Ui::TextUnformatted("Personal edition");
    if(ImGui::SmallButton("Arabic / English###exelight_language")){arabic=!arabic;SavePreferences();}
    ImGui::SameLine();
    if(Ui::Button("Settings"))ImGui::OpenPopup("exelight_preferences");
    if(ImGui::BeginPopup("exelight_preferences")) {
        Ui::TextUnformatted("Local interface preferences only");
        ImGui::Separator();
        if(Ui::Checkbox("Arabic",&arabic))SavePreferences();
        if(Ui::SliderFloat("Interface scale",&interfaceScale,.85f,1.5f,"%.2f")){}
        if(ImGui::IsItemDeactivatedAfterEdit())SavePreferences();
        if(Ui::Button("Save settings"))SavePreferences();
        if(Ui::Button("Reset appearance")){interfaceScale=1;SavePreferences();}
        if(saveFailed)ImGui::TextWrapped("Could not save preferences beside the DLL. Check folder permissions.");
        ImGui::EndPopup();
    }
    ImGui::Separator();
}
}
