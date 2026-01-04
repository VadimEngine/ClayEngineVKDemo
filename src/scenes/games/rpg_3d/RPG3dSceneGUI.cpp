// clay 
#include <clay/application/desktop/AppDesktop.h>
#include <clay/gui/desktop/ImGuiComponentDesktop.h>
// project
#include "scenes/games/rpg_3d/RPG3dScene.h"
#include "scenes/games/GamesScene.h"
// class
#include "scenes/games/rpg_3d/RPG3dSceneGUI.h"

namespace rpg_3d {

RPG3dSceneGUI::RPG3dSceneGUI(RPG3dScene& scene)
    : mScene_(scene), mVSyncEnabled_(((clay::AppDesktop&)mScene_.getApp()).tempVSyncValue) {}

RPG3dSceneGUI::~RPG3dSceneGUI() {}

void RPG3dSceneGUI::render(vk::CommandBuffer cmdBuffer) {
    clay::ImGuiComponentDesktop::beginRender();
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Settings");
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 0.f, 0.f, 0.40f));
    if (ImGui::Button("Back")) {
        ((clay::AppDesktop&)mScene_.getApp()).setScene(new GamesScene(mScene_.getApp()));
    }
    ImGui::PopStyleColor();
    ImGui::Text("RPG 3D: WIP");
    ImGui::Text("FPS: %.1f", double(ImGui::GetIO().Framerate));
    if (ImGui::Checkbox("vSync", &mVSyncEnabled_)) {
        ((clay::AppDesktop&)mScene_.getApp()).tempVSyncFlag = true;
        ((clay::AppDesktop&)mScene_.getApp()).tempVSyncValue= mVSyncEnabled_;
    }
    ImGui::Separator();
    
    ImGui::Text("Controls:");
    ImGui::Text("WASD - Move");
    ImGui::Text("Right Click Drag - Move Camera");
    ImGui::Text("Space - Jump");
    
    ImGui::End();

    clay::ImGuiComponentDesktop::endRender(cmdBuffer);
}

} // namespace rpg_3d
