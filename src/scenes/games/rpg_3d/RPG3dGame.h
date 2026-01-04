#pragma once
// standard lib
#include <set>
#include <memory>
// clay
#include <clay/ecs/EntityManager.h>
#include <clay/graphics/common/SkyBox.h>
#include <clay/graphics/common/AnimatedMesh.h>
#include <clay/graphics/common/SkeletalAnimation.h>

namespace rpg_3d {

class RPG3dScene;

class RPG3dGame {
public:
    RPG3dGame(RPG3dScene& scene);
    ~RPG3dGame() = default;

    void initialize();

    void update(float dt);

    void render(vk::CommandBuffer cmdBuffer);

public:
    std::set<clay::ecs::Entity> mEntities_;

    clay::ecs::EntityManager mEntityManager_;

    RPG3dScene& mScene_;
    
    clay::ecs::Entity mPlayerEntity_;
    
    // Player transform (for animated character)
    glm::vec3 mPlayerPosition_ = {0.0f, 0.0f, 0.0f};
    glm::quat mPlayerOrientation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    
    // Camera orbit controls
    float mCameraYaw_ = 0.0f;      // Horizontal rotation around player
    float mCameraPitch_ = -30.0f;   // Vertical angle (degrees)
    float mCameraDistance_ = 5.0f; // Distance from player
    glm::vec2 mLastMousePos_ = {0.0f, 0.0f};
    bool mIsFirstMouse_ = true;
    
    // Player rotation controls
    glm::quat mTargetOrientation_ = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    float mRotationProgress_ = 1.0f; // 0 to 1, 1 means rotation complete
    const float mRotationDuration_ = 0.25f; // Time to complete rotation in seconds
    
    // Jump controls
    float mVerticalVelocity_ = 0.0f;
    bool mIsJumping_ = false;
    const float mJumpForce_ = 8.0f;
    const float mGravity_ = 20.0f;
    const float mGroundLevel_ = 0.0f;
};

} // namespace rpg_3d
