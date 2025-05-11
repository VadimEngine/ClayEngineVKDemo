
// third party
// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
// clay
#include <clay/application/desktop/AppDesktop.h>
// class
#include "scenes/menu_scene/MenuScene.h"
#include "scenes/games/GamesScene.h"

GamesScene::GamesScene(clay::IApp& app): clay::BaseScene(app) {}

GamesScene::~GamesScene() {}

void GamesScene::update(const float dt) {}

void GamesScene::render(VkCommandBuffer cmdBuffer) {
    renderGUI(cmdBuffer);
}

void GamesScene::renderGUI(VkCommandBuffer cmdBuffer) {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    //----------------------
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Games", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Games WIP");

    if (ImGui::Button("Back")) {
        ((clay::AppDesktop&)mApp_).setScene(new MenuScene(((clay::AppDesktop&)mApp_)));
    }
    ImGui::End();
    //----------------------
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);
}

void GamesScene::initialize() {
    
}

void GamesScene::destroyResources() {

}