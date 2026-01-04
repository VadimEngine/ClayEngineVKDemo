// clay 
#include <clay/application/desktop/AppDesktop.h>
#include <clay/gui/desktop/ImGuiComponentDesktop.h>
// project
#include "scenes/games/rpg_2d/RPG2dScene.h"
#include "scenes/games/GamesScene.h"
// class
#include "scenes/games/rpg_2d/RPG2dSceneGUI.h"

namespace rpg_2d {

RPG2dSceneGUI::RPG2dSceneGUI(RPG2dScene& scene)
    : mScene_(scene), mVSyncEnabled_(((clay::AppDesktop&)mScene_.getApp()).tempVSyncValue) {
        mVolume_ = mScene_.getApp().getAudioManager().getGain();
    }

RPG2dSceneGUI::~RPG2dSceneGUI() {}

void RPG2dSceneGUI::render(vk::CommandBuffer cmdBuffer) {
    clay::ImGuiComponentDesktop::beginRender();
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Settings");
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 0.f, 0.f, 0.40f));
    if (ImGui::Button("Back")) {
        ((clay::AppDesktop&)mScene_.getApp()).setScene(new GamesScene(mScene_.getApp()));
    }
    ImGui::PopStyleColor();
    ImGui::Text("RPG 2D");
    ImGui::Text("FPS: %.1f", double(ImGui::GetIO().Framerate));
    if (ImGui::Checkbox("vSync", &mVSyncEnabled_)) {
        ((clay::AppDesktop&)mScene_.getApp()).tempVSyncFlag = true;
        ((clay::AppDesktop&)mScene_.getApp()).tempVSyncValue= mVSyncEnabled_;
    }
    
    if (ImGui::Checkbox("Show Colliders", &mShowColliders_)) {
        mScene_.mGame_.mEntityManager_.mRenderSystem_.mDebugDrawColliders_ = mShowColliders_;
    }

    if (ImGui::SliderFloat("Volume", &mVolume_, 0.f, 1.f)) {
       mScene_.getApp().getAudioManager().setGain(mVolume_);
    }

    
    ImGui::Separator();

    ImGui::Text("Controls:");
    ImGui::Text("WASD - Move");
    
    ImGui::End();

    clay::ImGuiComponentDesktop::endRender(cmdBuffer);
}

} // namespace rpg_2d
