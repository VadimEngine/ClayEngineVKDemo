// clay
#include <clay/application/desktop/AppDesktop.h>
#include <clay/utils/desktop/UtilsDesktop.h>
#include <clay/utils/common/Logger.h>

// class
#include "scenes/games/rpg_3d/RPG3dScene.h"

namespace rpg_3d {

RPG3dScene::RPG3dScene(clay::BaseApp& app)
    : clay::BaseScene(app),
    mGui_(*this),
    mGame_(*this) {
    getFocusCamera()->setPosition({0,1,5});
    const auto [frameWidth, frameHeight] = app.getGraphicsContext().getFrameDimensions();
    getFocusCamera()->setAspectRatio(static_cast<float>(frameWidth) / static_cast<float>(frameHeight));
}

RPG3dScene::~RPG3dScene() {}
        
void RPG3dScene::update(const float dt) {
    mGame_.update(dt);
}

void RPG3dScene::render(vk::CommandBuffer cmdBuffer) {
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

void RPG3dScene::renderGUI(vk::CommandBuffer cmdBuffer) {
    mGui_.render(cmdBuffer);
}

void RPG3dScene::initialize() {
    mGame_.initialize();
}

void RPG3dScene::destroyResources() {

}

} // namespace rpg_3d
