// clay
#include <clay/application/desktop/AppDesktop.h>
#include <clay/utils/desktop/UtilsDesktop.h>
#include <clay/utils/common/Logger.h>

// class
#include "scenes/games/pong/PongScene.h"

namespace pong {

PongScene::PongScene(clay::BaseApp& app)
    : clay::BaseScene(app),
    mGui_(*this),
    mGame_(*this) {
    // TODO move this camera logic to game
    getFocusCamera()->setPosition({0,0,10});
}

PongScene::~PongScene() {}
        
void PongScene::update(const float dt) {
    mGame_.update(dt);
}

void PongScene::render(vk::CommandBuffer cmdBuffer) {
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

void PongScene::renderGUI(vk::CommandBuffer cmdBuffer) {
    mGui_.render(cmdBuffer);
}

void PongScene::initialize() {

}


void PongScene::destroyResources() {

}

} // namespace pong