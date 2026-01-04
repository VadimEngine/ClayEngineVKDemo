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
#include "scenes/games/pong/PongScene.h"
#include "scenes/games/rpg_2d/RPG2dScene.h"
#include "scenes/games/rpg_3d/RPG3dScene.h"
#include "scenes/games/platform_2d/Platformer2dScene.h"

GamesScene::GamesScene(clay::BaseApp& app): clay::BaseScene(app) {}

GamesScene::~GamesScene() {}

void GamesScene::update(const float dt) {}

void GamesScene::render(vk::CommandBuffer cmdBuffer) {
    renderGUI(cmdBuffer);
}

void GamesScene::renderGUI(vk::CommandBuffer cmdBuffer) {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    //----------------------
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Games", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Games WIP");

    if (ImGui::Button("Pong")) {
        ((clay::AppDesktop&)mApp_).setScene(new pong::PongScene(mApp_));
    }

    if (ImGui::Button("RPG 2D")) {
        ((clay::AppDesktop&)mApp_).setScene(new rpg_2d::RPG2dScene(mApp_));
    }

    if (ImGui::Button("RPG 3D")) {
        ((clay::AppDesktop&)mApp_).setScene(new rpg_3d::RPG3dScene(mApp_));
    }

    if (ImGui::Button("Platformer 2D")) {
        ((clay::AppDesktop&)mApp_).setScene(new platform_2d::Platformer2dScene(mApp_));
    }
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