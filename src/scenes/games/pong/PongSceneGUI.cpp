// clay 
#include <clay/application/desktop/AppDesktop.h>
#include <clay/gui/desktop/ImGuiComponentDesktop.h>
// project
#include "scenes/games/pong/PongScene.h"
#include "scenes/games/GamesScene.h"
// class
#include "scenes/games/pong/PongSceneGUI.h"

namespace pong {

PongSceneGUI::PongSceneGUI(PongScene& scene)
    : mScene_(scene), mVSyncEnabled_(((clay::AppDesktop&)mScene_.getApp()).tempVSyncValue) {}

PongSceneGUI::~PongSceneGUI() {}

void PongSceneGUI::render(vk::CommandBuffer cmdBuffer) {
    clay::ImGuiComponentDesktop::beginRender();
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Settings");
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 0.f, 0.f, 0.40f));
    if (ImGui::Button("Back")) {
        ((clay::AppDesktop&)mScene_.getApp()).setScene(new GamesScene(mScene_.getApp()));
    }
    ImGui::PopStyleColor();
    ImGui::Text("Pong Scene");
    ImGui::Text("FPS: %.1f", double(ImGui::GetIO().Framerate));
    if (ImGui::Checkbox("vSync", &mVSyncEnabled_)) {
        ((clay::AppDesktop&)mScene_.getApp()).tempVSyncFlag = true;
        ((clay::AppDesktop&)mScene_.getApp()).tempVSyncValue= mVSyncEnabled_;
    }
    ImGui::Separator();
    
    // Player mode selection
    ImGui::Text("Players");
    if (ImGui::RadioButton("1 Player", mPlayerCount_ == 1)) {
        mPlayerCount_ = 1;
        mScene_.mGame_.setAIMode(true);
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("2 Players", mPlayerCount_ == 2)) {
        mPlayerCount_ = 2;
        mScene_.mGame_.setAIMode(false);
    }
    
    ImGui::Separator();
    
    // Controls instructions
    ImGui::Text("Controls");
    ImGui::Text("Left Paddle:");
    ImGui::BulletText("W - Move Up");
    ImGui::BulletText("S - Move Down");
    if (mPlayerCount_ == 2) {
        ImGui::Text("Right Paddle:");
        ImGui::BulletText("Up - Move Up");
        ImGui::BulletText("Down - Move Down");
    } else {
        ImGui::Text("Right Paddle: AI");
    }
    ImGui::Separator();
    ImGui::BulletText("Space - Launch Ball");
    
    ImGui::End();

    clay::ImGuiComponentDesktop::endRender(cmdBuffer);
}

} // namespace pong