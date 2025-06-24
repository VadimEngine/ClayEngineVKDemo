// clay
#include <clay/application/desktop/AppDesktop.h>
#include <clay/entity/render/ModelRenderable.h>
#include <clay/entity/render/TextRenderable.h>
#include <clay/entity/render/SpriteRenderable.h>
// class
#include "scenes/basic_scene/BasicScene.h"
#include "scenes/menu_scene/MenuScene.h"

namespace basic_scene {

BasicScene::BasicScene(clay::BaseApp& app)
    : clay::BaseScene(app),
      mGui_(*this) {
    mCamera_.setPosition({0,1,5});
    const auto [frameWidth, frameHeight] = app.getGraphicsContext().getFrameDimensions();
    mCamera_.setAspectRatio(static_cast<float>(frameWidth) / static_cast<float>(frameHeight));
    {   
        // solid sphere
        clay::Entity* entity = new clay::Entity();
        clay::ModelRenderable* modelRenderable = new clay::ModelRenderable(mApp_.getResources().getResource<clay::Model>("SolidSphere"));
        entity->addRenderable(modelRenderable);
        entity->setPosition({0,1,0});
        mEntities_.emplace_back(entity);
    }
    {    
        // texture sphere
        clay::Entity* entity = new clay::Entity();
        clay::ModelRenderable* modelRenderable2= new clay::ModelRenderable(mApp_.getResources().getResource<clay::Model>("VTexture"));
        entity->addRenderable(modelRenderable2);
        entity->setPosition({1,1,0});
        mEntities_.emplace_back(entity);
    }
    {    
        // text
        clay::Entity* entity = new clay::Entity();
        clay::TextRenderable* textRenderable = new clay::TextRenderable(mApp_.getGraphicsContext(), "HELLO WORLD", mApp_.getResources().getResource<clay::Font>("Runescape"));
        textRenderable->setScale({.01f,.01f,.01f});
        textRenderable->setColor({1,1,0,1});
        entity->addRenderable(textRenderable);
        mEntities_.emplace_back(entity);
    }
    {    
        // Sprite
        clay::Entity* entity = new clay::Entity();
        clay::SpriteRenderable* spriteRenderable = new clay::SpriteRenderable(
            mApp_.getResources().getResource<clay::Mesh>("Plane"), 
            mApp_.getResources().getResource<clay::Material>("SpriteSheet"),
            {0, 0, 16.0f / 512.0f , 16.0f / 512.0f }// TODO replace these magic numbers with Sprite Class
        );
        entity->addRenderable(spriteRenderable);
        entity->setPosition({-1,1,0});
        entity->setOrientation(glm::angleAxis(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
        mEntities_.emplace_back(entity);
    }
    {
        // floor
        clay::Entity* entity = new clay::Entity();
        clay::ModelRenderable* modelRenderable = new clay::ModelRenderable(
            mApp_.getResources().getResource<clay::Model>("SolidPlane")
        );
        modelRenderable->setColor({0.1f, 0.1f, 0.1f, 1.0f});
        modelRenderable->setScale({10, 1, 10});
        entity->addRenderable(modelRenderable);
        mEntities_.emplace_back(entity);
    }

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

   mEntities_[1]->getOrientation() *= glm::angleAxis(glm::radians(60.0f * dt), glm::vec3(0.0f, 1.0f, 0.0f)); 
}

void BasicScene::render(VkCommandBuffer cmdBuffer) {
    // update camera // TODO maybe have a camera.bind() method?
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
    mGui_.render(cmdBuffer);
}

void BasicScene::initialize() {

}

void BasicScene::destroyResources() {

}

std::vector<std::unique_ptr<clay::Entity>>& BasicScene::getEntities() {
    return mEntities_;
}


} // namespace basic_scene