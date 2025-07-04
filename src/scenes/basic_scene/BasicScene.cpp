// clay
#include <clay/application/desktop/AppDesktop.h>
// class
#include "scenes/basic_scene/BasicScene.h"
#include "scenes/menu_scene/MenuScene.h"

namespace basic_scene {

BasicScene::BasicScene(clay::BaseApp& app)
    : clay::BaseScene(app),
      mEntityManager_(mApp_.getResources()),
      mGui_(*this) {
    mCamera_.setPosition({0,1,5});
    const auto [frameWidth, frameHeight] = app.getGraphicsContext().getFrameDimensions();
    mCamera_.setAspectRatio(static_cast<float>(frameWidth) / static_cast<float>(frameHeight));

    {
        // solid sphere
        clay::ecs::Entity entity = mEntityManager_.createEntity();
        mEntityManager_.addModelRenderable(
            entity, 
            { 
                mApp_.getResources().mModelsPool_.getHandle("SolidSphere"),
                {1,1,1,1}
            }
        );
        clay::ecs::Transform transform;
        transform.mPosition_ = {0, 1, 0};
        mEntityManager_.addTransform(entity, transform);
        mECSEntities_.insert(entity);
    }

    {
        // Texture sphere
        mTextureSphere_ = mEntityManager_.createEntity();
        mEntityManager_.addModelRenderable(
            mTextureSphere_, 
            { 
                mApp_.getResources().mModelsPool_.getHandle("VSphere"),
                {1,1,1,1}
            }
        );
        clay::ecs::Transform transform;
        transform.mPosition_ = {1, 1, 0};
        mEntityManager_.addTransform(mTextureSphere_, transform);
        mECSEntities_.insert(mTextureSphere_);
    }
    {
        // Text
        clay::ecs::Entity entity = mEntityManager_.createEntity();

        clay::ecs::TextRenderable text;
        text.initialize(
            mApp_.getGraphicsContext(),
            "HELLO WORLD",
            mApp_.getResources().getResource<clay::Font>("Runescape")
        );
        text.mScale_ = {.01f,.01f,.01f};
        text.mColor_ = {1,1,0,1};
        mEntityManager_.addTextRenderable(
            entity, 
            text
        );
        mEntityManager_.addTransform(entity, {});
        mECSEntities_.insert(entity);
    }

    {
        // sprite
        clay::ecs::Entity entity = mEntityManager_.createEntity();

        clay::ecs::SpriteRenderable sprite;
        mEntityManager_.addSpriteRenderable(
            entity, 
            {
                mApp_.getResources().getResource<clay::Mesh>("Plane"),
                mApp_.getResources().getResource<clay::Material>("SpriteSheet"),
                {0, 0, 16.0f / 512.0f, 16.0f / 512.0f},
                {1,1,1,1}
            }
        );
        clay::ecs::Transform transform;
        transform.mPosition_ = {-1, 1, 0};
        mEntityManager_.addTransform(entity, transform);
        mECSEntities_.insert(entity);
    }
    {
        // floor
        clay::ecs::Entity entity = mEntityManager_.createEntity();
        mEntityManager_.addModelRenderable(
            entity, 
            { 
                mApp_.getResources().mModelsPool_.getHandle("SolidPlane"),
                {0.1f, 0.1f, 0.1f, 1.0f}
            }
        );
        clay::ecs::Transform transform;
        transform.mPosition_ = {0, 0, 0};
        transform.mScale_ = {10, 10, 1};
        transform.mOrientation_ = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mEntityManager_.addTransform(entity, transform);
        mECSEntities_.insert(entity);
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

   mEntityManager_.mTransforms[mTextureSphere_].mOrientation_ *= glm::angleAxis(glm::radians(60.0f * dt), glm::vec3(0.0f, 1.0f, 0.0f));
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

    mEntityManager_.render(cmdBuffer);
    renderGUI(cmdBuffer);
}

void BasicScene::renderGUI(VkCommandBuffer cmdBuffer) {
    mGui_.render(cmdBuffer);
}

void BasicScene::initialize() {

}

void BasicScene::destroyResources() {

}

std::set<clay::ecs::Entity>& BasicScene::getEntities() {
    return mECSEntities_;
}

} // namespace basic_scene