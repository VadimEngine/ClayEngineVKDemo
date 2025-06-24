// clay 
#include <clay/application/desktop/AppDesktop.h>
#include <clay/gui/desktop/ImGuiComponentDesktop.h>
// project
#include "scenes/galaxy/GalaxyScene.h"
#include "scenes/menu_scene/MenuScene.h"
// class
#include "scenes/galaxy/GalaxySceneGUI.h"


namespace galaxy {
    
GalaxySceneGUI::GalaxySceneGUI(GalaxyScene& scene)
    : mScene_(scene) {

}

GalaxySceneGUI::~GalaxySceneGUI() {

}

void GalaxySceneGUI::render(VkCommandBuffer cmdBuffer) {
    clay::ImGuiComponentDesktop::beginRender();
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Galaxy", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Galaxy WIP");

    if (ImGui::Button("Back")) {
        ((clay::AppDesktop&)mScene_.getApp()).setScene(new MenuScene(mScene_.getApp()));
    }
    ImGui::End();

    clay::ImGuiComponentDesktop::endRender(cmdBuffer);
}

} // namespace galaxy
