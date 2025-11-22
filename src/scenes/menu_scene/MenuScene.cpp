// clay
#include <clay/application/desktop/AppDesktop.h>
#include <clay/gui/desktop/ImGuiComponentDesktop.h>
// class
#include "scenes/menu_scene/MenuScene.h"
#include "scenes/basic_scene/BasicScene.h"
#include "scenes/scene2d/Scene2d.h"
#include "scenes/physics_scene/PhysicsScene.h"
#include "scenes/physX_scene/PhysXScene.h"
#include "scenes/galaxy/GalaxyScene.h"
#include "scenes/games/GamesScene.h"

MenuScene::MenuScene(clay::BaseApp& app): clay::BaseScene(app) {}

MenuScene::~MenuScene() {}

void MenuScene::update(const float dt) {}

void MenuScene::render(vk::CommandBuffer cmdBuffer) {
    renderGUI(cmdBuffer);
}

void MenuScene::renderGUI(vk::CommandBuffer cmdBuffer) {
    clay::ImGuiComponentDesktop::beginRender();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Menu", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Clay EngineVK Demo");

    if (ImGui::Button("Basic Scene")) {
        ((clay::AppDesktop&)mApp_).setScene(new basic_scene::BasicScene(((clay::AppDesktop&)mApp_)));
    }
    if (ImGui::Button("Scene 2D")) {
        ((clay::AppDesktop&)mApp_).setScene(new Scene2d(mApp_));
    }
    if (ImGui::Button("Physics Scene")) {
        ((clay::AppDesktop&)mApp_).setScene(new physics_scene::PhysicsScene(mApp_));
    }
    if (ImGui::Button("PhysX Scene")) {
        ((clay::AppDesktop&)mApp_).setScene(new physX_scene::PhysXScene(mApp_));
    }
    if (ImGui::Button("Galaxy")) {
        ((clay::AppDesktop&)mApp_).setScene(new galaxy::GalaxyScene(mApp_));
    }
    if (ImGui::Button("Games")) {
        ((clay::AppDesktop&)mApp_).setScene(new GamesScene(mApp_));
    }
    if (ImGui::Button("Exit")) {
       ((clay::AppDesktop&)mApp_).quit();
    }
    ImGui::End();

    clay::ImGuiComponentDesktop::endRender(cmdBuffer);
}

void MenuScene::initialize() {

}

void MenuScene::destroyResources() {

}
