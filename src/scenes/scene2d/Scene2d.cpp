// third party
// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
// clay
#include <clay/application/desktop/AppDesktop.h>
// class
#include "scenes/menu_scene/MenuScene.h"
#include "scenes/scene2d/Scene2d.h"

Scene2d::Scene2d(clay::BaseApp& app): clay::BaseScene(app) {}

Scene2d::~Scene2d() {}

void Scene2d::update(const float dt) {}

void Scene2d::render(vk::CommandBuffer cmdBuffer) {
    renderGUI(cmdBuffer);
}

void Scene2d::renderGUI(vk::CommandBuffer cmdBuffer) {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    //----------------------
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Scene2d", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Scene2d WIP");

    if (ImGui::Button("Back")) {
        ((clay::AppDesktop&)mApp_).setScene(new MenuScene(mApp_));
    }
    ImGui::End();
    //----------------------
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);
}

void Scene2d::initialize() {
    
}

void Scene2d::destroyResources() {

}