
// clay 
#include <clay/application/desktop/AppDesktop.h>
#include <clay/gui/desktop/ImGuiComponentDesktop.h>
// project
#include "scenes/physics_scene/PhysicsScene.h"
#include "scenes/menu_scene/MenuScene.h"
// class
#include "scenes/physics_scene/PhysicsSceneGUI.h"

namespace physics_scene {

PhysicsSceneGUI::PhysicsSceneGUI(PhysicsScene& scene)
    : mScene_(scene), mVSyncEnabled_(((clay::AppDesktop&)mScene_.getApp()).tempVSyncValue) {}

PhysicsSceneGUI::~PhysicsSceneGUI() {}

void PhysicsSceneGUI::render(vk::CommandBuffer cmdBuffer) {
    clay::ImGuiComponentDesktop::beginRender();
    
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Settings");
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 0.f, 0.f, 0.40f));
    if (ImGui::Button("Back")) {
        ((clay::AppDesktop&)mScene_.getApp()).setScene(new MenuScene(mScene_.getApp()));
    }
    ImGui::PopStyleColor();
    ImGui::Text("Physics Scene");
    ImGui::Text("FPS: %.1f", double(ImGui::GetIO().Framerate));
    if (ImGui::Checkbox("vSync", &mVSyncEnabled_)) {
        ((clay::AppDesktop&)mScene_.getApp()).tempVSyncFlag = true;
        ((clay::AppDesktop&)mScene_.getApp()).tempVSyncValue= mVSyncEnabled_;
    }
    const auto cameraPos = mScene_.getFocusCamera()->getPosition();
    ImGui::Text("Camera: %.1f %.1f %.1f", cameraPos.x, cameraPos.y, cameraPos.z);
    ImGui::Separator();
    entitySection();
    ImGui::End();

    clay::ImGuiComponentDesktop::endRender(cmdBuffer);
}

void PhysicsSceneGUI::entitySection() {
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


    if (mScene_.getEntities().contains(mSelectedEntityIndex_)) {
        // Display selected Entity details/controls
        // for now assume there is a Transform
        clay::ecs::Transform& transform = mScene_.getEntityManager().mTransforms[mSelectedEntityIndex_];

        float entityPosition[3] = {
            transform.mPosition_.x,
            transform.mPosition_.y,
            transform.mPosition_.z
        };

        glm::quat& entityOrientation = transform.mOrientation_;
        glm::vec3 eulerAngles = glm::eulerAngles(entityOrientation);
        float entityRotation[3] = {
            eulerAngles.x,
            eulerAngles.y,
            eulerAngles.z
        };
        float entityScale[3] = {
            transform.mScale_.x,
            transform.mScale_.y,
            transform.mScale_.z
        };

        if (ImGui::SliderFloat3("Position##Entity", entityPosition, -10.f, 10.f, "%.2f")) {
            transform.mPosition_ = {
                entityPosition[0],
                entityPosition[1],
                entityPosition[2]
            };
        }

        if (ImGui::SliderFloat3("Rotation##Entity", entityRotation, -10.f, 10.f, "%.2f")) {
            transform.mOrientation_ = glm::quat(glm::vec3{
                glm::radians(entityRotation[0]),
                glm::radians(entityRotation[1]),
                glm::radians(entityRotation[2])
            });
        }

        if (ImGui::SliderFloat3("Scale##Entity", entityScale, -10.f, 10.f, "%.2f")) {
            transform.mScale_ = {
                entityScale[0],
                entityScale[1],
                entityScale[2]
            };
        }
    }

    if (ImGui::Button("Add Particle")) {
        clay::ecs::Entity entity = mScene_.getEntityManager().createEntity();

        mScene_.getEntityManager().addModelRenderable(
            entity, 
            { 
                mScene_.getApp().getResources().getHandle<clay::Model>("SolidCircle"),
                {1,1,1,1}
            }
        );

        mScene_.getEntityManager().addTransform(entity, {});
        mScene_.getEntityManager().addCollider(entity, {});
        clay::ecs::RigidBody rigidBody{};
        // rigidBody.attractive = true;
        mScene_.getEntityManager().addRigidBody(entity, rigidBody);
        mScene_.getEntities().insert(entity);
    }
}


} // namespace physics_scene