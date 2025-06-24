// third party
// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
// clay
#include <clay/application/desktop/AppDesktop.h>
// class
#include "scenes/menu_scene/MenuScene.h"
#include "scenes/galaxy/GalaxyScene.h"

namespace galaxy {

GalaxyScene::GalaxyScene(clay::BaseApp& app)
    : clay::BaseScene(app),
      mGui_(*this) {
    getFocusCamera()->setPosition({0,0,5});

    moonEntity_ = new MoonEntity(*this);
}

GalaxyScene::~GalaxyScene() {}

void GalaxyScene::update(const float dt) {}

void GalaxyScene::render(VkCommandBuffer cmdBuffer) {
    renderGUI(cmdBuffer);

    clay::BaseScene::CameraConstant ubo{};
    ubo.view = mCamera_.getViewMatrix();
    ubo.proj = mCamera_.getProjectionMatrix();
    ((clay::AppDesktop&)mApp_).getGraphicsContextDesktop().mCameraUniform_->setData(
        &ubo,
        sizeof(CameraConstant)
    );

    moonEntity_->render(cmdBuffer);
}

void GalaxyScene::renderGUI(VkCommandBuffer cmdBuffer) {
    mGui_.render(cmdBuffer);
}

void GalaxyScene::initialize() {
    
}

void GalaxyScene::destroyResources() {

}

} // namespace galaxy