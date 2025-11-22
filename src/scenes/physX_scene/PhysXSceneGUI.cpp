

// clay 
#include <clay/application/desktop/AppDesktop.h>
#include <clay/gui/desktop/ImGuiComponentDesktop.h>
// project
#include "scenes/physX_scene/PhysXScene.h"
#include "scenes/menu_scene/MenuScene.h"
// class
#include "scenes/physX_scene/PhysXSceneGUI.h"

namespace physX_scene {

PhysXSceneGUI::PhysXSceneGUI(PhysXScene& scene)
    : mScene_(scene), mVSyncEnabled_(((clay::AppDesktop&)mScene_.getApp()).tempVSyncValue) {}

PhysXSceneGUI::~PhysXSceneGUI() {}

void PhysXSceneGUI::render(vk::CommandBuffer cmdBuffer) {
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
    // entitySection();

    // if (mScene_.mBox_) {
    //     physx::PxTransform t = mScene_.mBox_->getGlobalPose();
    //     ImGui::Text("Box Position: %.2f, %.2f, %.2f",
    //         t.p.x, t.p.y, t.p.z);
    // }

    // if (ImGui::Button("Reset Box")) {
    //     mScene_.mBox_->setGlobalPose(physx::PxTransform(physx::PxVec3(0, 5, 0)));
    //     mScene_.mBox_->setLinearVelocity(physx::PxVec3(0, 0, 0));
    //     mScene_.mBox_->setAngularVelocity(physx::PxVec3(0, 0, 0));
    // }

    controlSection();


    ImGui::End();

    clay::ImGuiComponentDesktop::endRender(cmdBuffer);
}

void PhysXSceneGUI::controlSection() {

    ImGui::Text("Gravity [x]");


    ImGui::Separator();


    ImGui::Text("Entities");
    if (ImGui::BeginListBox("##Entities")) {
        for (const auto entity: mScene_.mEntities_) {
            std::string tempName = "Entity " + std::to_string(entity);
            if (ImGui::Selectable(tempName.c_str(), entity == mSelectedEntityIndex_)) {
                mSelectedEntityIndex_ = entity;
            }
        }
        ImGui::EndListBox();
    }

    if (mScene_.mEntities_.contains(mSelectedEntityIndex_) && mScene_.mEntityManager_.mSignatures[mSelectedEntityIndex_][clay::ecs::RIGID_BODY] ) {
        // Display selected Entity details/controls
        // for now assume there is a Transform
        //clay::ecs::Transform& transform = mScene_.mEntityManager_.mTransforms[mSelectedEntityIndex_];

        physx::PxTransform pxTransform = mScene_.mEntityManager_.mRigidBodies[mSelectedEntityIndex_].actor->getGlobalPose();
        clay::ecs::Transform clTransform = mScene_.mEntityManager_.mTransforms[mSelectedEntityIndex_];

        float entityPosition[3] = {
            pxTransform.p.x,
            pxTransform.p.y,
            pxTransform.p.z
        };

        glm::quat entityOrientation = {pxTransform.q.w, pxTransform.q.x, pxTransform.q.y, pxTransform.q.z};
        glm::vec3 eulerAngles = glm::eulerAngles(entityOrientation);
        float entityRotation[3] = {
            eulerAngles.x,
            eulerAngles.y,
            eulerAngles.z
        };
        float entityScale[3] = {
            clTransform.mScale_.x,
            clTransform.mScale_.y,
            clTransform.mScale_.z
        };

        if (ImGui::SliderFloat3("Position##Entity", entityPosition, -10.f, 10.f, "%.2f")) {
            mScene_.mEntityManager_.mRigidBodies[mSelectedEntityIndex_].actor->setGlobalPose(physx::PxTransform(
                physx::PxVec3(entityPosition[0], entityPosition[1], entityPosition[2]),
                physx::PxQuat(entityOrientation.x, entityOrientation.y, entityOrientation.z, entityOrientation.w)
            ));
            // transform.mPosition_ = {
            //     entityPosition[0],
            //     entityPosition[1],
            //     entityPosition[2]
            // };
        }

        if (ImGui::SliderFloat3("Rotation##Entity", entityRotation, -10.f, 10.f, "%.2f")) {
            // transform.mOrientation_ = glm::quat(glm::vec3{
            //     glm::radians(entityRotation[0]),
            //     glm::radians(entityRotation[1]),
            //     glm::radians(entityRotation[2])
            // });
        }

        if (ImGui::SliderFloat3("Scale##Entity", entityScale, -10.f, 10.f, "%.2f")) {
            // transform.mScale_ = {
            //     entityScale[0],
            //     entityScale[1],
            //     entityScale[2]
            // };
        }

        ImGui::Text("Shape");
        ImGui::Text("Disable");
        ImGui::SameLine();
        ImGui::Text("Remove");
    }

    ImGui::Separator();


    // current selection
    static int current_item = 0;

    // list of items
    const char* items[] = { "Sphere", "Cube", "Torus", "Plane" };
    const int items_count = IM_ARRAYSIZE(items);

    // Label for combo
    if (ImGui::BeginCombo("Sphere", items[current_item])) {
        // Loop through items
        for (int n = 0; n < items_count; n++) {
            // Is the item selected?
            bool is_selected = (current_item == n);

            // Display item and check if clicked
            if (ImGui::Selectable(items[n], is_selected))
                current_item = n;

            // Set initial focus when opening the combo (optional)
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (ImGui::Button("Add Object")) {
        mScene_.addEntity();
    }
    

}


/*

entities:
    1
    2
    3

position
scale
orientation
shape
remove
disable

new object
    shape[^]

*/

} // namespace physX_scene