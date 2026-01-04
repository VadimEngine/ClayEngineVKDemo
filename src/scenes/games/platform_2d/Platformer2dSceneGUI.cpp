// clay 
#include <clay/application/desktop/AppDesktop.h>
#include <clay/gui/desktop/ImGuiComponentDesktop.h>
// project
#include "scenes/games/platform_2d/Platformer2dScene.h"
#include "scenes/games/GamesScene.h"
// class
#include "scenes/games/platform_2d/Platformer2dSceneGUI.h"

namespace platform_2d {

Platformer2dSceneGUI::Platformer2dSceneGUI(Platformer2dScene& scene)
    : mScene_(scene), mVSyncEnabled_(((clay::AppDesktop&)mScene_.getApp()).tempVSyncValue) {}

Platformer2dSceneGUI::~Platformer2dSceneGUI() {}

void Platformer2dSceneGUI::render(vk::CommandBuffer cmdBuffer) {
    clay::ImGuiComponentDesktop::beginRender();
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Settings");
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 0.f, 0.f, 0.40f));
    if (ImGui::Button("Back")) {
        ((clay::AppDesktop&)mScene_.getApp()).setScene(new GamesScene(mScene_.getApp()));
    }
    ImGui::PopStyleColor();
    ImGui::Text("Platformer 2D");
    ImGui::Text("FPS: %.1f", double(ImGui::GetIO().Framerate));
    if (ImGui::Checkbox("vSync", &mVSyncEnabled_)) {
        ((clay::AppDesktop&)mScene_.getApp()).tempVSyncFlag = true;
        ((clay::AppDesktop&)mScene_.getApp()).tempVSyncValue= mVSyncEnabled_;
    }
    
    if (ImGui::Checkbox("Show Colliders", &mShowColliders_)) {
        mScene_.mGame_.mEntityManager_.mRenderSystem_.mDebugDrawColliders_ = mShowColliders_;
    }
    
    ImGui::Separator();
    
    // Player info
    if (mScene_.mGame_.mPlayer_) {
        glm::vec3 playerPos = mScene_.mGame_.mPlayer_->getPosition();
        glm::vec2 playerVel = mScene_.mGame_.mPlayer_->getVelocity();
        
        ImGui::Text("Player Position:");
        ImGui::Text("  X: %.2f", playerPos.x);
        ImGui::Text("  Y: %.2f", playerPos.y);
        ImGui::Text("  Z: %.2f", playerPos.z);
        
        ImGui::Text("Player Velocity:");
        ImGui::Text("  X: %.2f", playerVel.x);
        ImGui::Text("  Y: %.2f", playerVel.y);
    }
    ImGui::Separator();

    ImGui::Text("Controls:");
    ImGui::Text("A D - Move");
    ImGui::Text("Space - Jump");
    
    ImGui::End();

    clay::ImGuiComponentDesktop::endRender(cmdBuffer);
}

} // namespace platform_2d
