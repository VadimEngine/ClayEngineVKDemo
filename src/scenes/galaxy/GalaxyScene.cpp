// third party
// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
// clay
#include <clay/application/desktop/AppDesktop.h>
// class
#include "scenes/menu_scene/MenuScene.h"
#include "scenes/galaxy/GalaxyScene.h"

GalaxyScene::GalaxyScene(clay::IApp& app): clay::BaseScene(app) {}

GalaxyScene::~GalaxyScene() {}

void GalaxyScene::update(const float dt) {}

void GalaxyScene::render(VkCommandBuffer cmdBuffer) {
    renderGUI(cmdBuffer);
}

void GalaxyScene::renderGUI(VkCommandBuffer cmdBuffer) {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    //----------------------
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Galaxy", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Galaxy WIP");

    if (ImGui::Button("Back")) {
        ((clay::AppDesktop&)mApp_).setScene(new MenuScene(((clay::AppDesktop&)mApp_)));
    }
    ImGui::End();
    //----------------------
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);
}

void GalaxyScene::initialize() {
    
}

void GalaxyScene::destroyResources() {

}