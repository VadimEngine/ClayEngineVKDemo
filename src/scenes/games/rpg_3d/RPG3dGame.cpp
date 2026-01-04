// standard lib
#include <filesystem>
// glm
#include <glm/gtc/type_ptr.hpp>
// clay
#include <clay/application/desktop/AppDesktop.h>
#include <clay/utils/common/Logger.h>
#include <clay/graphics/common/AnimatedMesh.h>
#include <clay/graphics/common/SkeletalAnimation.h>
// project
#include "scenes/games/rpg_3d/RPG3dGame.h"
#include "scenes/games/rpg_3d/RPG3dScene.h"
#include "DemoApp.h"

namespace rpg_3d {

RPG3dGame::RPG3dGame(RPG3dScene& scene)
    : mEntityManager_(scene.getApp().getGraphicsContext(), scene.getApp().getResources())
    , mScene_(scene) {

    mScene_.getFocusCamera()->setPosition({0,1,5});
}

void RPG3dGame::initialize() {
    // Create skybox
    mEntityManager_.setSkybox(
        ((DemoApp&)mScene_.getApp()).mAppProp.mMeshes.sphere,
        ((DemoApp&)mScene_.getApp()).mAppProp.mMaterials.cloudSkyNoDepth
    );
        
    {
        // floor
        clay::ecs::Entity entity = mEntityManager_.createEntity();
        mEntityManager_.addComponent<clay::ecs::ModelRenderable>(
            entity, 
            { 
                ((DemoApp&)mScene_.getApp()).mAppProp.mModels.solidPlane,
                {0.19f, 0.65f, 0.0f, 1.0f}
            }
        );
        clay::ecs::Transform transform;
        transform.mPosition_ = {0, 0, 0};
        transform.mScale_ = {50, 50, 1};
        transform.mOrientation_ = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        mEntityManager_.addComponent<clay::ecs::Transform>(entity, transform);
        mEntities_.insert(entity);
    }
    
    // ========================================================================
    // Create animated player entity
    // ========================================================================
    {
        mPlayerEntity_ = mEntityManager_.createEntity();
        
        // Add transform
        clay::ecs::Transform transform;
        transform.mPosition_ = mPlayerPosition_;
        transform.mOrientation_ = mPlayerOrientation_;
        mEntityManager_.addComponent<clay::ecs::Transform>(mPlayerEntity_, transform);
        
        // Add skeletal animation renderable
        clay::ecs::Animation3DRenderable animRenderable;
        animRenderable.meshHandle = mScene_.getApp().getResources().getHandle<clay::AnimatedMesh>("WalkCycleCharacter");
        animRenderable.animationHandle = mScene_.getApp().getResources().getHandle<clay::SkeletalAnimation>("WalkCycleAnimation");
        animRenderable.mColor_ = glm::vec4(0.95f, 0.674f, 0.411f, 1.0f);
        
        // Create per-entity bone transforms buffer
        vk::DeviceSize boneBufferSize = sizeof(glm::mat4) * 100; // MAX_BONES
        mScene_.getApp().getGraphicsContext().createBuffer(
            boneBufferSize,
            vk::BufferUsageFlagBits::eUniformBuffer,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            animRenderable.boneTransformsBuffer_,
            animRenderable.boneTransformsMemory_
        );
        
        // Initialize with identity matrices
        std::vector<glm::mat4> identityBones(100, glm::mat4(1.0f));
        void* data = mScene_.getApp().getGraphicsContext().getDevice().mapMemory(
            animRenderable.boneTransformsMemory_, 0, boneBufferSize
        );
        memcpy(data, identityBones.data(), boneBufferSize);
        mScene_.getApp().getGraphicsContext().getDevice().unmapMemory(animRenderable.boneTransformsMemory_);
        
        // Create per-entity material with this entity's bone buffer
        auto pipelineHandle = mScene_.getApp().getResources().getHandle<clay::PipelineResource>("Animation3D");
        
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = mScene_.getApp().getGraphicsContext(),
            .pipelineResource = mScene_.getApp().getResources()[pipelineHandle]
        };
        
        matConfig.bufferBindings = {
            {
                .buffer = ((clay::AppDesktop&)mScene_.getApp()).getGraphicsContextDesktop().mCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = vk::DescriptorType::eUniformBuffer
            },
            {
                .buffer = animRenderable.boneTransformsBuffer_,
                .size = boneBufferSize,
                .binding = 1,
                .descriptorType = vk::DescriptorType::eUniformBuffer
            }
        };
    
        // Store per-entity material in resources
        animRenderable.materialHandle = mScene_.getApp().getResources().addResource<clay::Material>(
            std::move(clay::Material(matConfig)),
            "AnimationPlayer" // Unique name per entity
        );
        
        animRenderable.playing = false;
        animRenderable.looping = true;
        animRenderable.currentTime = 0.0f;
        mEntityManager_.addComponent<clay::ecs::Animation3DRenderable>(mPlayerEntity_, animRenderable);
        
        mEntities_.insert(mPlayerEntity_);
    }
}

void RPG3dGame::update(float dt) {
    clay::InputHandlerDesktop& inputHandler = (((clay::AppDesktop&)mScene_.getApp()).getWindow().getInputHandler());

    // Camera orbit controls with right mouse button
    if (inputHandler.isMouseButtonPressed(clay::InputHandlerDesktop::MouseEvent::Button::RIGHT)) {
        glm::vec2 currentMousePos = inputHandler.getMousePosition();
        
        if (!mIsFirstMouse_) {
            glm::vec2 mouseDelta = currentMousePos - mLastMousePos_;
            
            // Adjust sensitivity
            float sensitivity = 0.3f;
            mCameraYaw_ -= mouseDelta.x * sensitivity;
            mCameraPitch_ -= mouseDelta.y * sensitivity;
            
            // Clamp pitch to prevent camera flipping
            mCameraPitch_ = glm::clamp(mCameraPitch_, -80.0f, 80.0f);
        }
        
        mLastMousePos_ = currentMousePos;
        mIsFirstMouse_ = false;
    } else {
        mIsFirstMouse_ = true;
    }

    // Mouse wheel zoom control
    auto mouseEvent = inputHandler.getMouseEvent();
    if (mouseEvent.has_value()) {
        if (mouseEvent->getType() == clay::InputHandlerDesktop::MouseEvent::Type::SCROLL_UP) {
            mCameraDistance_ -= 0.5f;
            mCameraDistance_ = glm::max(mCameraDistance_, 2.0f); // Min distance
        } else if (mouseEvent->getType() == clay::InputHandlerDesktop::MouseEvent::Type::SCROLL_DOWN) {
            mCameraDistance_ += 0.5f;
            mCameraDistance_ = glm::min(mCameraDistance_, 15.0f); // Max distance
        }
    }

    float moveSpeed = 8.0f;
    
    // Calculate camera forward and right vectors based on yaw (XZ plane only)
    float yawRad = glm::radians(mCameraYaw_);
    glm::vec3 cameraForward = glm::vec3(sin(yawRad), 0.0f, cos(yawRad));
    glm::vec3 cameraRight = glm::vec3(-cos(yawRad), 0.0f, sin(yawRad));
    
    // Player movement controls relative to camera orientation (XZ plane only)
    glm::vec3 moveDirection(0.0f);
    bool isMoving = false;
    
    if (inputHandler.isKeyPressed(GLFW_KEY_W)) {
        // Move forward relative to camera
        moveDirection += cameraForward;
        isMoving = true;
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_S)) {
        // Move backward relative to camera
        moveDirection -= cameraForward;
        isMoving = true;
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_A)) {
        // Move left relative to camera
        moveDirection -= cameraRight;
        isMoving = true;
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_D)) {
        // Move right relative to camera
        moveDirection += cameraRight;
        isMoving = true;
    }
    
    // Get animation component once for this update
    auto& animRenderable = mEntityManager_.mAnimation3DRenderables[mPlayerEntity_];
    
    // Control animation based on movement
    if (isMoving && !animRenderable.playing) {
        animRenderable.playing = true;
    } else if (!isMoving && animRenderable.playing) {
        animRenderable.playing = false;
    }
    
    // Normalize and apply movement (only affects X and Z)
    if (glm::length(moveDirection) > 0.0f) {
        moveDirection = glm::normalize(moveDirection);
        mPlayerPosition_ += moveDirection * moveSpeed * dt;
        
        // Smooth rotation to face movement direction
        float targetYaw = atan2(moveDirection.x, moveDirection.z);
        glm::quat targetOrientation = glm::angleAxis(targetYaw, glm::vec3(0.0f, 1.0f, 0.0f));
        
        // Interpolate rotation based on dt and rotation duration
        float rotationSpeed = dt / mRotationDuration_;
        mPlayerOrientation_ = glm::slerp(
            mPlayerOrientation_,
            targetOrientation,
            glm::min(rotationSpeed, 1.0f)
        );
    }
    
    // Update player entity transform
    mEntityManager_.mTransforms[mPlayerEntity_].mPosition_ = mPlayerPosition_;
    mEntityManager_.mTransforms[mPlayerEntity_].mOrientation_ = mPlayerOrientation_;

    // Jump handling
    if (inputHandler.isKeyPressed(GLFW_KEY_SPACE) && !mIsJumping_) {
        mVerticalVelocity_ = mJumpForce_;
        mIsJumping_ = true;
    }
    
    // Apply gravity and vertical movement
    if (mIsJumping_ || mPlayerPosition_.y > mGroundLevel_) {
        mVerticalVelocity_ -= mGravity_ * dt;
        mPlayerPosition_.y += mVerticalVelocity_ * dt;
        
        // Check if landed
        if (mPlayerPosition_.y <= mGroundLevel_) {
            mPlayerPosition_.y = mGroundLevel_;
            mVerticalVelocity_ = 0.0f;
            mIsJumping_ = false;
        }
    }

    // Update camera to orbit around player
    glm::vec3 playerPos = mPlayerPosition_;
    
    // Calculate camera position based on yaw, pitch, and distance
    float pitchRad = glm::radians(mCameraPitch_);
    
    glm::vec3 cameraOffset;
    cameraOffset.x = mCameraDistance_ * cos(pitchRad) * sin(yawRad);
    cameraOffset.y = mCameraDistance_ * sin(pitchRad);
    cameraOffset.z = mCameraDistance_ * cos(pitchRad) * cos(yawRad);
    
    glm::vec3 cameraPos = playerPos + glm::vec3(0.0f, 1.5f, 0.0f) - cameraOffset;
    
    // Position camera
    mScene_.getFocusCamera()->setPosition(cameraPos);
    
    // Calculate orientation to look at player
    glm::vec3 lookAtTarget = playerPos + glm::vec3(0.0f, 1.5f, 0.0f);
    glm::vec3 forward = glm::normalize(lookAtTarget - cameraPos);
    glm::vec3 right = glm::normalize(glm::cross(forward, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::vec3 up = glm::cross(right, forward);
    
    // Create rotation matrix and convert to quaternion
    glm::mat3 rotationMatrix;
    rotationMatrix[0] = right;
    rotationMatrix[1] = up;
    rotationMatrix[2] = -forward;
    glm::quat orientation = glm::quat_cast(rotationMatrix);
    mScene_.getFocusCamera()->setOrientation(orientation);

    // Update skybox
    if (mEntityManager_.hasSkybox()) {
        mEntityManager_.getSkybox()->update(mScene_.getFocusCamera()->getOrientation());
    }

    // Update animation and bone transforms
    auto& skelAnimation = mScene_.getApp().getResources()[animRenderable.animationHandle];
    
    std::vector<glm::mat4> boneTransforms;
    
    if (animRenderable.playing) {
        // Update animation time
        animRenderable.currentTime += dt;
        float duration = skelAnimation.getDuration();
        if (animRenderable.looping && animRenderable.currentTime >= duration) {
            animRenderable.currentTime = fmod(animRenderable.currentTime, duration);
        }
        // Get animated bone transforms
        skelAnimation.setTime(animRenderable.currentTime);
        boneTransforms = skelAnimation.update(0.0f); // Pass 0 since we already updated time
    } else {
        // Scale down to match the animated model size (try 0.01 or 0.02 scale)
        glm::mat4 scaleDown = glm::scale(glm::mat4(1.0f), glm::vec3(0.01f));
        boneTransforms = std::vector<glm::mat4>(100, scaleDown);
    }
    
    // Upload bone transforms to GPU (only if we have transforms)
    if (!boneTransforms.empty()) {
        void* data = mScene_.getApp().getGraphicsContext().getDevice().mapMemory(
            animRenderable.boneTransformsMemory_, 0, sizeof(glm::mat4) * boneTransforms.size()
        );
        memcpy(data, boneTransforms.data(), sizeof(glm::mat4) * boneTransforms.size());
        mScene_.getApp().getGraphicsContext().getDevice().unmapMemory(animRenderable.boneTransformsMemory_);
    }

    mEntityManager_.update(dt);
}

void RPG3dGame::render(vk::CommandBuffer cmdBuffer) {
    mEntityManager_.render(cmdBuffer);
}

} // namespace rpg_3d
