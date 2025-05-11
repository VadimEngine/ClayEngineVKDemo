// third party
// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
// clay
#include <clay/application/desktop/AppDesktop.h>
// class
#include "scenes/menu_scene/MenuScene.h"
#include "scenes/basic_scene/BasicScene.h"
#include "scenes/scene2d/Scene2d.h"
#include "scenes/physics_scene/PhysicsScene.h"
#include "scenes/galaxy/GalaxyScene.h"
#include "scenes/games/GamesScene.h"


MenuScene::MenuScene(clay::IApp& app): clay::BaseScene(app) {}

MenuScene::~MenuScene() {}

void MenuScene::update(const float dt) {}

void MenuScene::render(VkCommandBuffer cmdBuffer) {
    renderGUI(cmdBuffer);
}

void MenuScene::renderGUI(VkCommandBuffer cmdBuffer) {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    //----------------------
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Clay EngineVK Sandbox");

    if (ImGui::Button("Basic Scene")) {
        ((clay::AppDesktop&)mApp_).setScene(new BasicScene(((clay::AppDesktop&)mApp_)));
    }
    if (ImGui::Button("Scene 2D")) {
        ((clay::AppDesktop&)mApp_).setScene(new Scene2d(mApp_));
    }
    if (ImGui::Button("Physics Scene")) {
        ((clay::AppDesktop&)mApp_).setScene(new PhysicsScene(mApp_));
    }
    if (ImGui::Button("Galaxy")) {
        ((clay::AppDesktop&)mApp_).setScene(new GalaxyScene(mApp_));
    }
    if (ImGui::Button("Games")) {
        ((clay::AppDesktop&)mApp_).setScene(new GamesScene(mApp_));
    }
    if (ImGui::Button("Exit")) {
       ((clay::AppDesktop&)mApp_).quit();
    }
    ImGui::End();
    //----------------------
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);
}

void MenuScene::initialize() {

}

void MenuScene::destroyResources() {

}
