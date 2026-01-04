#pragma once
// clay
#include <clay/ecs/EntityManager.h>
#include <clay/application/common/Resources.h>
#include <clay/application/desktop/InputHandlerDesktop.h>
// third party
#include <glm/glm.hpp>

namespace platform_2d {

class Player {
public:
    Player(clay::ecs::EntityManager& entityManager, clay::Resources& resources);
    ~Player() = default;

    void update(float dt, const clay::InputHandlerDesktop& inputHandler);
    
    clay::ecs::Entity getEntity() const { return mEntity_; }
    glm::vec3 getPosition() const;
    glm::vec2 getVelocity() const;

private:
    enum class Direction { LEFT, RIGHT };
    
    bool isGrounded() const;
    
    clay::ecs::EntityManager& mEntityManager_;
    clay::Resources& mResources_;
    clay::ecs::Entity mEntity_;
    Direction mDirection_ = Direction::RIGHT;
    float mGroundCheckDistance_ = 0.12f;
    
    // Jump parameters
    bool mIsJumping_ = false;
    float mJumpHoldTime_ = 0.0f;
    float mMaxJumpHoldTime_ = 0.35f;  // Max time jump can be held for boost
    float mBaseJumpVelocity_ = 3.0f;  // Initial jump velocity (tap jump)
    float mJumpHoldBoost_ = 12.0f;    // Additional velocity per second while holding
    float mMaxJumpVelocity_ = 7.0f;   // Maximum upward velocity from jumping (full hold)
    
    // Movement parameters
    float mMaxMoveSpeed_ = 2.0f;
    float mMoveAcceleration_ = 12.0f;  // Acceleration when key is pressed
    float mMoveDeceleration_ = 15.0f;  // Deceleration when key is released
};

} // namespace platform_2d
