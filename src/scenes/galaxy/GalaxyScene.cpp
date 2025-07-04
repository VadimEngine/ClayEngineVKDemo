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
      mEntityManager_(mApp_.getResources()),
      mGui_(*this) {
    getFocusCamera()->setPosition({0,1,10});

    // skybox
    mSkyBox_ = new clay::SkyBox(
        *mApp_.getResources().getResource<clay::Mesh>("Sphere"),
        *mApp_.getResources().getResource<clay::Material>("Stars")
    );

    // sun
    mSunEntity_ = new SunEntity(*this, mEntityManager_);
    // planet
    mPlanetEntity_ = new PlanetEntity(*this, mEntityManager_);
    // moon
    mMoonEntity_ = new MoonEntity(*this, mEntityManager_);
}

GalaxyScene::~GalaxyScene() {
    // Delete ECS
}

void GalaxyScene::update(const float dt) {
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

    mSkyBox_->update(mpFocusCamera_->getOrientation(), dt);

    glm::vec3 planetDiff;
    {
        glm::vec3 orbitCenter = mSunEntity_->getPosition();

        // Update planet orbit
        glm::vec3 planetPos = mPlanetEntity_->getPosition();

        // Calculate relative position of the planet with respect to the center of the orbit
        glm::vec3 relativePos = planetPos - orbitCenter;

        // Calculate angle of rotation based on current position
        float planetAngle = glm::atan(relativePos.z, relativePos.x);

        // Calculate distance from center
        float radius = glm::length(glm::vec2(relativePos.x, relativePos.z));

        // Update angle if planet is not at the center
        if (radius > 0.0f) {
            // Adjust the speed of rotation as needed
            planetAngle += mPlanetEntity_->getRotationSpeed() * dt;
        }

        // Calculate new position of the planet
        float x = orbitCenter.x + radius * glm::cos(planetAngle);
        float z = orbitCenter.z + radius * glm::sin(planetAngle);
        glm::vec3 newPos = glm::vec3(x, orbitCenter.y, z); // Y-coordinate remains the same as orbit center

        planetDiff = newPos - planetPos;

        // Update planet position
        mPlanetEntity_->setPosition(newPos);
    }
    {
        // move moon
        mMoonEntity_->setPosition(mMoonEntity_->getPosition() + planetDiff);

        // orbit moon around planet
        glm::vec3 orbitCenter = mPlanetEntity_->getPosition();
        glm::vec3 moonPos = mMoonEntity_->getPosition();

        glm::vec3 relativePos = moonPos - orbitCenter;

        float moonAngle = glm::atan(relativePos.z, relativePos.x);
        float radius = glm::length(glm::vec2(relativePos.x, relativePos.z));

        // Update angle if moon is not at the center
        if (radius > 0.0f) {
            // Adjust the speed of rotation as needed
            moonAngle += mMoonEntity_->getRotationSpeed() * dt;
        }
        // Calculate new position of the planet
        float x = orbitCenter.x + radius * glm::cos(moonAngle);
        float z = orbitCenter.z + radius * glm::sin(moonAngle);
        glm::vec3 newPos = glm::vec3(x, orbitCenter.y, z); // Y-coordinate remains the same as orbit center

        // Update planet position
        mMoonEntity_->setPosition(newPos);
    }
}

void GalaxyScene::render(VkCommandBuffer cmdBuffer) {

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
    // TODO move skybox logic to Entity component
    mSkyBox_->render(cmdBuffer); // skybox should be drawn first
    mEntityManager_.render(cmdBuffer);

    renderGUI(cmdBuffer);
}

void GalaxyScene::renderGUI(VkCommandBuffer cmdBuffer) {
    mGui_.render(cmdBuffer);
}

void GalaxyScene::initialize() {
    
}

void GalaxyScene::destroyResources() {

}

} // namespace galaxy