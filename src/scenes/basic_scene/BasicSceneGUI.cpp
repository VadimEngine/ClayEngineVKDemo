// clay 
#include <clay/application/desktop/AppDesktop.h>
#include <clay/gui/desktop/ImGuiComponentDesktop.h>
// project
#include "scenes/basic_scene/BasicScene.h"
#include "scenes/menu_scene/MenuScene.h"
// class
#include "scenes/basic_scene/BasicSceneGUI.h"


namespace basic_scene {

BasicSceneGUI::BasicSceneGUI(BasicScene& scene)
    : mScene_(scene), mVSyncEnabled_(((clay::AppDesktop&)mScene_.getApp()).tempVSyncValue) {}

BasicSceneGUI::~BasicSceneGUI() {}

void BasicSceneGUI::render(VkCommandBuffer cmdBuffer) {
    clay::ImGuiComponentDesktop::beginRender();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Settings");
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 0.f, 0.f, 0.40f));
    if (ImGui::Button("Back")) {
        ((clay::AppDesktop&)mScene_.getApp()).setScene(new MenuScene(mScene_.getApp()));
    }
    ImGui::PopStyleColor();
    ImGui::Text("Basic Scene");
    ImGui::Text("FPS: %.1f", double(ImGui::GetIO().Framerate));
    if (ImGui::Checkbox("vSync", &mVSyncEnabled_)) {
        ((clay::AppDesktop&)mScene_.getApp()).tempVSyncFlag = true;
        ((clay::AppDesktop&)mScene_.getApp()).tempVSyncValue= mVSyncEnabled_;
    }
    ImGui::Separator();
    // // Camera control
    cameraSection();
    ImGui::Separator();
    entitiesSection();
    ImGui::End();

    clay::ImGuiComponentDesktop::endRender(cmdBuffer);
}

void BasicSceneGUI::cameraSection() {
    ImGui::Text("Camera");

    ImGui::Text("Camera Mode");

    if (ImGui::RadioButton("Perspective", &mCameraMode_, 0)) {
        //mScene_.getFocusCamera()->setMode(static_cast<clay::Camera::CameraMode>(mCameraMode_));
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("Orthogonal", &mCameraMode_, 1)) {
        //mScene_.getFocusCamera()->setMode(static_cast<clay::Camera::CameraMode>(mCameraMode_));
    }

    ImGui::Text("Camera Movement");
    glm::vec3 camPosition = mScene_.getFocusCamera()->getPosition();
    ImGui::Text(
        "Position: %.2f %.2f %.2f",
        camPosition.x,
        camPosition.y,
        camPosition.z
    );
    glm::vec3 camForward = mScene_.getFocusCamera()->getForward();
    ImGui::Text(
        "Direction: %.2f %.2f %.2f",
        camForward.x,
        camForward.y,
        camForward.z
    );
    ImGui::Text(
        "FOV: %.2f",
        mScene_.getFocusCamera()->getFOV()
    );
}

void BasicSceneGUI::entitiesSection() {
    ImGui::Text("Entities");

    if (ImGui::BeginListBox("##Entities")) {
        for (const auto entity: mScene_.getEntities()) {
            std::string tempName = "Entity " + std::to_string(entity);
            if (ImGui::Selectable(tempName.c_str(), entity == mSelectedEntityIndex_)) {
                mSelectedEntityIndex_ = entity;
            }
        }
        ImGui::EndListBox();
    }

    // if (mSelectedEntityIndex_ < mScene_.getEntities().size()) {
    //     // Display selected Entity details/controls
    //     clay::Entity* selectedEntity = mScene_.getEntities()[mSelectedEntityIndex_].get();
    //     //mScene_.setHighLightEntity(selectedEntity);

    //     float entityPosition[3] = {
    //         selectedEntity->getPosition().x,
    //         selectedEntity->getPosition().y,
    //         selectedEntity->getPosition().z
    //     };
    //     glm::quat& entityOrientation = selectedEntity->getOrientation();
    //     glm::vec3 eulerAngles = glm::eulerAngles(entityOrientation);
    //     float entityRotation[3] = {
    //         eulerAngles.x,
    //         eulerAngles.y,
    //         eulerAngles.z
    //     };
    //     float entityScale[3] = {
    //         selectedEntity->getScale().x,
    //         selectedEntity->getScale().y,
    //         selectedEntity->getScale().z
    //     };

    //     if (ImGui::SliderFloat3("Position##Entity", entityPosition, -10.f, 10.f, "%.2f")) {
    //         selectedEntity->setPosition({
    //             entityPosition[0],
    //             entityPosition[1],
    //             entityPosition[2]
    //         });
    //     }

    //     if (ImGui::SliderFloat3("Rotation##Entity", entityRotation, -10.f, 10.f, "%.2f")) {
    //         selectedEntity->setOrientation( glm::quat(glm::vec3{
    //             glm::radians(entityRotation[0]),
    //             glm::radians(entityRotation[1]),
    //             glm::radians(entityRotation[2])
    //         }));
    //     }

    //     if (ImGui::SliderFloat3("Scale##Entity", entityScale, -10.f, 10.f, "%.2f")) {
    //         selectedEntity->setScale({
    //             entityScale[0],
    //             entityScale[1],
    //             entityScale[2]
    //         });
    //     }

    //     // List Renderable components for the selected Entity
    //     ImGui::Text("Renderable Components");

    //     auto& renderables = selectedEntity->getRenderables();

    //     if (ImGui::BeginListBox("##Renderables")) {
    //         for (unsigned int i = 0; i < renderables.size(); ++i) {
    //             std::string tempName = "Renderable " + std::to_string(i);
    //             if (ImGui::Selectable(tempName.c_str(), i == mSelectedRenderableIndex_)) {
    //                 mSelectedRenderableIndex_ = i;
    //             }
    //         }
    //         ImGui::EndListBox();
    //     }

        
    //     if (mSelectedRenderableIndex_ < renderables.size()) {
    //         // Display selected Renderable details/controls
    //         auto& selectedRenderable = renderables[mSelectedRenderableIndex_];

    //         // float renderableColor[4] = {
    //         //     selectedRenderable->getColor().r,
    //         //     selectedRenderable->getColor().g,
    //         //     selectedRenderable->getColor().b,
    //         //     selectedRenderable->getColor().a
    //         // };
    //         float renderablePosition[3] = {
    //             selectedRenderable->getPosition().x,
    //             selectedRenderable->getPosition().y,
    //             selectedRenderable->getPosition().z
    //         };
    //         glm::quat& renderableOrientation = selectedRenderable->getOrientation();
    //         glm::vec3 eulerAngles = glm::eulerAngles(entityOrientation);
    //         float renderableRotation[3] = {
    //             eulerAngles.x,
    //             eulerAngles.y,
    //             eulerAngles.z
    //         };
    //         float renderableScale[3] = {
    //             selectedRenderable->getScale().x,
    //             selectedRenderable->getScale().y,
    //             selectedRenderable->getScale().z
    //         };

    //         bool isEnabled = selectedRenderable->isEnabled();

    //         if (ImGui::Checkbox("Enabled", &isEnabled)) {
    //             selectedRenderable->setEnabled(isEnabled);
    //         }

    //         // if (ImGui::ColorEdit4("Color##Renderable", renderableColor)) {
    //         //     selectedRenderable->setColor({
    //         //         renderableColor[0],
    //         //         renderableColor[1],
    //         //         renderableColor[2],
    //         //         renderableColor[3]
    //         //     });
    //         // }

    //         if (ImGui::SliderFloat3("Position##Renderable", renderablePosition, -10.f, 10.f, "%.2f")) {
    //             selectedRenderable->setPosition({
    //                 renderablePosition[0],
    //                 renderablePosition[1],
    //                 renderablePosition[2]
    //             });
    //         }

    //         if (ImGui::SliderFloat3("Rotation##Renderable", renderableRotation, -10.f, 10.f, "%.2f")) {
    //             selectedRenderable->setOrientation( glm::quat(glm::vec3{
    //                 glm::radians(renderableRotation[0]),
    //                 glm::radians(renderableRotation[1]),
    //                 glm::radians(renderableRotation[2])
    //             }));
    //         }

    //         if (ImGui::SliderFloat3("Scale##Renderable", renderableScale, -10.f, 10.f, "%.2f")) {
    //             selectedRenderable->setScale({
    //                 renderableScale[0],
    //                 renderableScale[1],
    //                 renderableScale[2]
    //             });
    //         }
    //     }
    // }
}


} // namespace basic_scene