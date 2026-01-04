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
    : mScene_(scene), mVSyncEnabled_(((clay::AppDesktop&)mScene_.getApp()).tempVSyncValue) {}

GalaxySceneGUI::~GalaxySceneGUI() {}

void GalaxySceneGUI::render(vk::CommandBuffer cmdBuffer) {
    clay::ImGuiComponentDesktop::beginRender();
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Settings");
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 0.f, 0.f, 0.40f));
    if (ImGui::Button("Back")) {
        ((clay::AppDesktop&)mScene_.getApp()).setScene(new MenuScene(mScene_.getApp()));
    }
    ImGui::PopStyleColor();
    ImGui::Text("Galaxy Scene");
    ImGui::Text("FPS: %.1f", double(ImGui::GetIO().Framerate));
    if (ImGui::Checkbox("vSync", &mVSyncEnabled_)) {
        ((clay::AppDesktop&)mScene_.getApp()).tempVSyncFlag = true;
        ((clay::AppDesktop&)mScene_.getApp()).tempVSyncValue= mVSyncEnabled_;
    }
    ImGui::Separator();
    
    // Camera information
    ImGui::Text("Camera");
    glm::vec3 camPos = mScene_.getFocusCamera()->getPosition();
    ImGui::Text("Position: (%.2f, %.2f, %.2f)", camPos.x, camPos.y, camPos.z);
    
    glm::quat camOrient = mScene_.getFocusCamera()->getOrientation();
    glm::vec3 euler = glm::eulerAngles(camOrient);
    ImGui::Text("Orientation (deg):");
    ImGui::Text("  Pitch: %.2f", glm::degrees(euler.x));
    ImGui::Text("  Yaw: %.2f", glm::degrees(euler.y));
    ImGui::Text("  Roll: %.2f", glm::degrees(euler.z));
    
    ImGui::Separator();
    ImGui::End();

    clay::ImGuiComponentDesktop::endRender(cmdBuffer);
}

} // namespace galaxy
