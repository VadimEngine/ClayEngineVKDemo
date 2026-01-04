// clay
#include <clay/application/desktop/AppDesktop.h>
#include <clay/utils/desktop/UtilsDesktop.h>
#include <clay/utils/common/Logger.h>

// class
#include "scenes/games/platform_2d/Platformer2dScene.h"

namespace platform_2d {

Platformer2dScene::Platformer2dScene(clay::BaseApp& app)
    : clay::BaseScene(app),
    mGui_(*this),
    mGame_(*this) {
    getFocusCamera()->setMode(clay::Camera::Mode::ORTHOGRAPHIC);
    getFocusCamera()->setPosition({0,0,10});
}

Platformer2dScene::~Platformer2dScene() {}
        
void Platformer2dScene::update(const float dt) {
    mGame_.update(dt);
}

void Platformer2dScene::render(vk::CommandBuffer cmdBuffer) {
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

void Platformer2dScene::renderGUI(vk::CommandBuffer cmdBuffer) {
    mGui_.render(cmdBuffer);
}

void Platformer2dScene::initialize() {

}

void Platformer2dScene::destroyResources() {

}

} // namespace platform_2d
