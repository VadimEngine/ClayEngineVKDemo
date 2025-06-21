// third party
// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
// clay
#include <clay/application/desktop/AppDesktop.h>
// class
#include "scenes/menu_scene/MenuScene.h"
#include "scenes/physics_scene/PhysicsScene.h"

PhysicsScene::PhysicsScene(clay::BaseApp& app): clay::BaseScene(app) {}

PhysicsScene::~PhysicsScene() {}

void PhysicsScene::update(const float dt) {}

void PhysicsScene::render(VkCommandBuffer cmdBuffer) {
    renderGUI(cmdBuffer);
}

void PhysicsScene::renderGUI(VkCommandBuffer cmdBuffer) {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    //----------------------
    ImGui::NewFrame();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Physics", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Physics WIP");

    if (ImGui::Button("Back")) {
        ((clay::AppDesktop&)mApp_).setScene(new MenuScene(mApp_));
    }
    ImGui::End();
    //----------------------
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuffer);
}

void PhysicsScene::initialize() {
    
}

void PhysicsScene::destroyResources() {

}