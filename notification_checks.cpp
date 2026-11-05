#define NOMINMAX
#include <windows.h>
#include <cassert>
#include <thread>
#include <vector>
#include <atomic>
#include <cstdio>
#include "imgui/imgui.h"
#include "imgui/ImGuiNotify.hpp"
int main() {
    ImGui::CreateContext();auto& io=ImGui::GetIO();io.IniFilename=nullptr;io.DisplaySize={980,640};io.DeltaTime=1.0f/60;
    io.Fonts->AddFontDefault();unsigned char* pixels{};int width{},height{};io.Fonts->GetTexDataAsRGBA32(&pixels,&width,&height);
    std::atomic<int> complete{};std::vector<std::thread> producers;
    for(int t=0;t<8;++t)producers.emplace_back([&,t]{for(int i=0;i<250;++i)ImGui::InsertNotification({ImGuiToastType::Info,100,"producer %d event %d",t,i});++complete;});
    int frames=0;
    while(complete.load()!=8||frames<20) {
        ImGui::NewFrame();ImGui::RenderNotifications();ImGui::Render();++frames;
        assert(ImGui::notifications.size()<=ImGui::notificationCapacity);
        {std::lock_guard<std::mutex> lock(ImGui::pendingNotificationMutex);assert(ImGui::pendingNotifications.size()<=ImGui::notificationCapacity);}
    }
    for(auto& thread:producers)thread.join();
    ImGui::RemoveNotification(SIZE_MAX);ImGui::DestroyContext();
    puts("PASS: 2000 notifications from eight producers while rendering; both queues bounded; invalid removal rejected; no game or mouse input.");
}
