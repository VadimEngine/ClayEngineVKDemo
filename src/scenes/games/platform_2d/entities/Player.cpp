#include "scenes/games/platform_2d/entities/Player.h"
#include <GLFW/glfw3.h>
#include <clay/utils/common/Logger.h>

namespace platform_2d {

Player::Player(clay::ecs::EntityManager& entityManager, clay::Resources& resources)
    : mEntityManager_(entityManager), mResources_(resources) {
    
    mEntity_ = mEntityManager_.createEntity();
    
    // Add animation rendering
    clay::ecs::Animation2DRenderable animRenderable;
    animRenderable.animationHandle = mResources_.getHandle<clay::Animation2D>("PlayerWalkRight");
    animRenderable.meshHandle = mResources_.getHandle<clay::Mesh>("Plane");
    
    animRenderable.mColor_ = {1, 1, 1, 1};
    animRenderable.mOffset_ = {0.0f, 0.0f, 0.0f};
    animRenderable.currentFrame = 0;
    animRenderable.currentTime = 0.0f;
    animRenderable.playing = false;  // Start paused (idle)
    mEntityManager_.addComponent<clay::ecs::Animation2DRenderable>(mEntity_, animRenderable);
    
    // Add transform
    mEntityManager_.addComponent<clay::ecs::Transform>(
        mEntity_,
        {
            .mPosition_ = {-1.2f, -1.2f, 0.0f},
            .mOrientation_ = {1.0f, 0.0f, 0.0f, 0.0f},
            .mScale_ = {0.2f, 0.2f, 1.0f}
        }
    );
    
    // Add 2D physics body (dynamic)
    clay::ecs::PhysicsBody2D physicsBody;
    physicsBody.type = clay::ecs::PhysicsBody2D::Type::DYNAMIC;
    physicsBody.velocity = {0.0f, 0.0f};
    physicsBody.mass = 1.0f;
    physicsBody.isSolid = true;
    physicsBody.bounciness = 0.0f;
    mEntityManager_.addComponent<clay::ecs::PhysicsBody2D>(mEntity_, physicsBody);
    
    // Add collider (AABB matching player size)
    clay::ecs::Collider collider;
    collider.mType_ = clay::ecs::Collider::Type::AABB;
    collider.aabb.halfExtents = glm::vec3(0.25f, 0.5f, 1.0f);
    collider.offset = glm::vec3(-0.05f, 0.0f, 0.0f);
    mEntityManager_.addComponent<clay::ecs::Collider>(mEntity_, collider);
}

void Player::update(float dt, const clay::InputHandlerDesktop& inputHandler) {
    if (!mEntityManager_.mSignatures[mEntity_][clay::ecs::ComponentType::PHYSICS_BODY_2D]) {
        return;
    }
    
    auto& body = mEntityManager_.mPhysicsBodies2D[mEntity_];
    
    bool isMoving = false;
    Direction newDirection = mDirection_;
    
    // Check if player is grounded
    bool grounded = isGrounded();
    
    // Horizontal movement with gradual acceleration/deceleration
    bool pressingLeft = inputHandler.isKeyPressed(GLFW_KEY_A);
    bool pressingRight = inputHandler.isKeyPressed(GLFW_KEY_D);
    
    if (pressingLeft) {
        // Accelerate left
        body.velocity.x -= mMoveAcceleration_ * dt;
        body.velocity.x = std::max(body.velocity.x, -mMaxMoveSpeed_);
        isMoving = true;
        newDirection = Direction::LEFT;
    } else if (pressingRight) {
        // Accelerate right
        body.velocity.x += mMoveAcceleration_ * dt;
        body.velocity.x = std::min(body.velocity.x, mMaxMoveSpeed_);
        isMoving = true;
        newDirection = Direction::RIGHT;
    } else {
        // Decelerate when no input
        if (body.velocity.x > 0.0f) {
            body.velocity.x -= mMoveDeceleration_ * dt;
            if (body.velocity.x < 0.0f) body.velocity.x = 0.0f;
        } else if (body.velocity.x < 0.0f) {
            body.velocity.x += mMoveDeceleration_ * dt;
            if (body.velocity.x > 0.0f) body.velocity.x = 0.0f;
        }
    }
    
    // Variable jump height based on hold duration
    bool spacePressed = inputHandler.isKeyPressed(GLFW_KEY_SPACE);
    
    if (spacePressed && grounded && !mIsJumping_) {
        // Start jump
        body.velocity.y = mBaseJumpVelocity_;
        mIsJumping_ = true;
        mJumpHoldTime_ = 0.0f;
    } else if (spacePressed && mIsJumping_ && body.velocity.y > 0.0f) {
        // Continue boosting jump while space is held and still moving upward
        if (mJumpHoldTime_ < mMaxJumpHoldTime_) {
            body.velocity.y += mJumpHoldBoost_ * dt;
            body.velocity.y = std::min(body.velocity.y, mMaxJumpVelocity_);
            mJumpHoldTime_ += dt;
        }
    } else if (!spacePressed || body.velocity.y <= 0.0f) {
        // Reset jump state when space is released or falling
        mIsJumping_ = false;
        mJumpHoldTime_ = 0.0f;
    }
    
    // Update animation based on movement
    if (mEntityManager_.mSignatures[mEntity_][clay::ecs::ComponentType::ANIMATION_2D_RENDERABLE]) {
        auto& animRenderable = mEntityManager_.mAnimation2DRenderables[mEntity_];
        
        // If direction changed, switch animation
        if (newDirection != mDirection_) {
            mDirection_ = newDirection;
            
            switch (mDirection_) {
                case Direction::LEFT:
                    animRenderable.animationHandle = mResources_.getHandle<clay::Animation2D>("PlayerWalkLeft");
                    break;
                case Direction::RIGHT:
                    animRenderable.animationHandle = mResources_.getHandle<clay::Animation2D>("PlayerWalkRight");
                    break;
            }
            
            // Reset animation to start with new animation
            animRenderable.currentFrame = 0;
            animRenderable.currentTime = 0.0f;
        }
        
        // Control animation playback - play if pressing movement keys
        animRenderable.playing = isMoving;
    }
}

glm::vec3 Player::getPosition() const {
    if (!mEntityManager_.mSignatures[mEntity_][clay::ecs::ComponentType::TRANSFORM]) {
        return glm::vec3(0.0f);
    }
    return mEntityManager_.mTransforms[mEntity_].mPosition_;
}

glm::vec2 Player::getVelocity() const {
    if (!mEntityManager_.mSignatures[mEntity_][clay::ecs::ComponentType::PHYSICS_BODY_2D]) {
        return glm::vec2(0.0f);
    }
    return mEntityManager_.mPhysicsBodies2D[mEntity_].velocity;
}

bool Player::isGrounded() const {
    if (!mEntityManager_.mSignatures[mEntity_][clay::ecs::ComponentType::TRANSFORM] ||
        !mEntityManager_.mSignatures[mEntity_][clay::ecs::ComponentType::COLLIDER] ||
        !mEntityManager_.mSignatures[mEntity_][clay::ecs::ComponentType::PHYSICS_BODY_2D]) {
        return false;
    }
    
    const auto& transform = mEntityManager_.mTransforms[mEntity_];
    const auto& collider = mEntityManager_.mColliders[mEntity_];
    const auto& body = mEntityManager_.mPhysicsBodies2D[mEntity_];
    
    // Don't consider grounded if moving upward (jumping)
    if (body.velocity.y > 0.1f) {
        return false;
    }
    
    // Calculate player's bottom position
    float playerBottom = transform.mPosition_.y - (collider.aabb.halfExtents.y * transform.mScale_.y);
    
    // Check all static bodies for collision below
    for (clay::ecs::Entity e : mEntityManager_.mCurrentEntities_) {
        if (e == mEntity_) continue;
        
        if (!mEntityManager_.mSignatures[e][clay::ecs::ComponentType::TRANSFORM] ||
            !mEntityManager_.mSignatures[e][clay::ecs::ComponentType::PHYSICS_BODY_2D] ||
            !mEntityManager_.mSignatures[e][clay::ecs::ComponentType::COLLIDER]) {
            continue;
        }
        
        const auto& otherBody = mEntityManager_.mPhysicsBodies2D[e];
        if (otherBody.type != clay::ecs::PhysicsBody2D::Type::STATIC) {
            continue;
        }
        
        const auto& otherTransform = mEntityManager_.mTransforms[e];
        const auto& otherCollider = mEntityManager_.mColliders[e];
        
        // Calculate platform's top position
        float platformTop = otherTransform.mPosition_.y + (otherCollider.aabb.halfExtents.y * otherTransform.mScale_.y);
        
        // Check if player is close to platform top and horizontally overlapping
        float verticalDist = playerBottom - platformTop;
        if (verticalDist >= 0.0f && verticalDist <= mGroundCheckDistance_) {
            // Check horizontal overlap - require significant overlap (at least 30% of player width)
            float playerLeft = transform.mPosition_.x + collider.offset.x - (collider.aabb.halfExtents.x * transform.mScale_.x);
            float playerRight = transform.mPosition_.x + collider.offset.x + (collider.aabb.halfExtents.x * transform.mScale_.x);
            float platformLeft = otherTransform.mPosition_.x - (otherCollider.aabb.halfExtents.x * otherTransform.mScale_.x);
            float platformRight = otherTransform.mPosition_.x + (otherCollider.aabb.halfExtents.x * otherTransform.mScale_.x);
            
            // Calculate overlap amount
            float overlapLeft = std::max(playerLeft, platformLeft);
            float overlapRight = std::min(playerRight, platformRight);
            float overlapWidth = overlapRight - overlapLeft;
            float playerWidth = playerRight - playerLeft;
            
            // Require at least 10% of player width to overlap with platform
            // This allows edge jumps while preventing wall-climb exploits
            if (overlapWidth > 0.0f && overlapWidth >= playerWidth * 0.1f) {
                return true;
            }
        }
    }
    
    return false;
}

} // namespace platform_2d
