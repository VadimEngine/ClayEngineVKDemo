// clay
#include <clay/application/desktop/AppDesktop.h>
#include <clay/gui/desktop/ImGuiComponentDesktop.h>
#include <clay/entity/render/ModelRenderable.h>
#include <clay/entity/render/TextRenderable.h>
// class
#include "scenes/basic_scene/BasicScene.h"
#include "scenes/menu_scene/MenuScene.h"

BasicScene::BasicScene(clay::BaseApp& app): clay::BaseScene(app) {
    mCamera_.setPosition({0,0,3});
    const auto [frameWidth, frameHeight] = app.getGraphicsContext().getFrameDimensions();
    mCamera_.setAspectRatio(static_cast<float>(frameWidth) / static_cast<float>(frameHeight));

    clay::Entity* entity1 = new clay::Entity();
    clay::ModelRenderable* modelRenderable = new clay::ModelRenderable(mApp_.getResources().getResource<clay::Model>("SolidSphere"));
    entity1->addRenderable(modelRenderable);
    entity1->setPosition({0,0,0});
    mEntities_.emplace_back(entity1);

    clay::Entity* entity2 = new clay::Entity();
    clay::ModelRenderable* modelRenderable2= new clay::ModelRenderable(mApp_.getResources().getResource<clay::Model>("VTexture"));
    entity2->addRenderable(modelRenderable2);
    entity2->setPosition({1,0,0});
    mEntities_.emplace_back(entity2);

    clay::Entity* entity3 = new clay::Entity();
    clay::TextRenderable* textRenderable = new clay::TextRenderable(mApp_.getGraphicsContext(), "HELLO WORLD", mApp_.getResources().getResource<clay::Font>("Runescape"));
    textRenderable->setScale({.01f,.01f,.01f});
    textRenderable->setColor({1,1,0,1});
    entity3->addRenderable(textRenderable);
    mEntities_.emplace_back(entity3);
}

BasicScene::~BasicScene() {}

void BasicScene::update(const float dt) {
    const clay::InputHandlerDesktop& inputHandler = (((clay::AppDesktop&)mApp_).getWindow().getInputHandler());

   if (inputHandler.isKeyPressed(GLFW_KEY_W)) {
       mCamera_.move(mCamera_.getForward(), mCamera_.getMoveSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_S)) {
       mCamera_.move(mCamera_.getForward(), -mCamera_.getMoveSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_A)) {
       mCamera_.move(mCamera_.getRight(), -mCamera_.getMoveSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_D)) {
       mCamera_.move(mCamera_.getRight(), mCamera_.getMoveSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_SPACE)) {
       mCamera_.move(mCamera_.getUp(), mCamera_.getMoveSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
       mCamera_.move(mCamera_.getUp(), -mCamera_.getMoveSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_LEFT)) {
       mCamera_.rotate(mCamera_.getUp(), mCamera_.getRotationSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_RIGHT)) {
       mCamera_.rotate(mCamera_.getUp(), -mCamera_.getRotationSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_UP)) {
       mCamera_.rotate(mCamera_.getRight(), mCamera_.getRotationSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_DOWN)) {
       mCamera_.rotate(mCamera_.getRight(), -mCamera_.getRotationSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_Q)) {
       mCamera_.rotate(mCamera_.getForward(), 200.0f * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_E)) {
       mCamera_.rotate(mCamera_.getForward(), -200.0f * dt);
   }
   // Speed
   if (inputHandler.isKeyPressed(GLFW_KEY_PERIOD)) {
       mCamera_.zoom(-mCamera_.getZoomSpeed() * dt);
   }
   if (inputHandler.isKeyPressed(GLFW_KEY_COMMA)) {
       mCamera_.zoom(mCamera_.getZoomSpeed() * dt);
   }

   mEntities_[1]->getOrientation() *= glm::angleAxis(glm::radians(60.0f / float(ImGui::GetIO().Framerate)), glm::vec3(0.0f, 1.0f, 0.0f)); 
}

void BasicScene::render(VkCommandBuffer cmdBuffer) {
    // update camera
    clay::BaseScene::CameraConstant ubo{};
    ubo.view = mCamera_.getViewMatrix();
    ubo.proj = mCamera_.getProjectionMatrix();

    ((clay::AppDesktop&)mApp_).getGraphicsContextDesktop().mCameraUniform_->setData(
        &ubo,
        sizeof(CameraConstant)
    );

    for (int i = 0; i < mEntities_.size(); ++i) {
        mEntities_[i]->render(cmdBuffer);
    }

    renderGUI(cmdBuffer);
}

void BasicScene::renderGUI(VkCommandBuffer cmdBuffer) {
    clay::ImGuiComponentDesktop::beginRender();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(250, 480), ImGuiCond_FirstUseEver);
    ImGui::Begin("Basic Scene", NULL, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Basic Scene");
    ImGui::Text("FPS: %.1f", double(ImGui::GetIO().Framerate));
    if (ImGui::Button("Back")) {
        ((clay::AppDesktop&)mApp_).setScene(new MenuScene(mApp_));
    }
    ImGui::End();
    clay::ImGuiComponentDesktop::endRender(cmdBuffer);
}

void BasicScene::initialize() {

}

void BasicScene::destroyResources() {

}