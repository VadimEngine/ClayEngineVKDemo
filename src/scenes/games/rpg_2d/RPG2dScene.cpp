// clay
#include <clay/application/desktop/AppDesktop.h>
#include <clay/utils/desktop/UtilsDesktop.h>
#include <clay/utils/common/Logger.h>

// class
#include "scenes/games/rpg_2d/RPG2dScene.h"

namespace rpg_2d {

RPG2dScene::RPG2dScene(clay::BaseApp& app)
    : clay::BaseScene(app),
    mGui_(*this),
    mGame_(*this) {
    getFocusCamera()->setMode(clay::Camera::Mode::ORTHOGRAPHIC);
    getFocusCamera()->setPosition({0,0,10});
}

RPG2dScene::~RPG2dScene() {}
        
void RPG2dScene::update(const float dt) {
    mGame_.update(dt);
}

void RPG2dScene::render(vk::CommandBuffer cmdBuffer) {
    {
        clay::BaseScene::CameraConstant ubo{};
        ubo.view = mCamera_.getViewMatrix();
        ubo.proj = mCamera_.getProjectionMatrix();
        ((clay::AppDesktop&)mApp_).getGraphicsContextDesktop().mCameraUniform_->setData(
            &ubo,
            sizeof(CameraConstant)
        );
    }
    {
        clay::BaseScene::CameraConstant ubo{};
        glm::mat4 view = mCamera_.getViewMatrix();
        ubo.view = glm::mat4(glm::mat3(view)); 
        ubo.proj = mCamera_.getProjectionMatrix();
        ((clay::AppDesktop&)mApp_).getGraphicsContextDesktop().mCameraUniformHeadLocked_->setData(
            &ubo,
            sizeof(CameraConstant)
        );
    }

    mGame_.render(cmdBuffer);
    renderGUI(cmdBuffer);
}

void RPG2dScene::renderGUI(vk::CommandBuffer cmdBuffer) {
    mGui_.render(cmdBuffer);
}

void RPG2dScene::initialize() {

}

void RPG2dScene::destroyResources() {

}

} // namespace rpg_2d
