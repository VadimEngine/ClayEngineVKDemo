#include "Player.h"
#include <GLFW/glfw3.h>

namespace rpg_2d {

Player::Player(clay::ecs::EntityManager& entityManager, clay::Resources& resources)
    : mEntityManager_(entityManager), mResources_(resources) {
    
    mEntity_ = mEntityManager_.createEntity();
    
    // Transform
    clay::ecs::Transform transform;
    transform.mPosition_ = {0, 0, 0.5f};  // Layer 1: Characters
    transform.mScale_ = {0.2f, 0.2f, 1.0f};
    mEntityManager_.addComponent<clay::ecs::Transform>(mEntity_, transform);
    
    // Collider
    clay::ecs::Collider collider;
    collider.mType_ = clay::ecs::Collider::Type::AABB;
    collider.aabb.halfExtents = glm::vec3(0.25f, 0.5f, 1.0f);
    collider.offset = glm::vec3(-0.05f, 0.0f, 0.0f);
    mEntityManager_.addComponent<clay::ecs::Collider>(mEntity_, collider);
    
    // 2D physics body - dynamic and solid
    clay::ecs::PhysicsBody2D playerPhysics;
    playerPhysics.type = clay::ecs::PhysicsBody2D::Type::DYNAMIC;
    playerPhysics.velocity = {0.0f, 0.0f};
    playerPhysics.mass = 1.0f;
    playerPhysics.isSolid = true;
    playerPhysics.isTrigger = false;
    mEntityManager_.addComponent<clay::ecs::PhysicsBody2D>(mEntity_, playerPhysics);
    
    // Animation component starting with walk down animation
    clay::ecs::Animation2DRenderable animRenderable;
    animRenderable.animationHandle = mResources_.getHandle<clay::Animation2D>("PlayerWalkDown");
    animRenderable.meshHandle = mResources_.getHandle<clay::Mesh>("Plane");
    animRenderable.mColor_ = {1, 1, 1, 1};
    animRenderable.currentFrame = 0;
    animRenderable.currentTime = 0.0f;
    animRenderable.playing = false;  // Start paused (idle)
    mEntityManager_.addComponent<clay::ecs::Animation2DRenderable>(mEntity_, animRenderable);
}

void Player::update(float dt, const clay::InputHandlerDesktop& inputHandler) {
    // Update player velocity based on input
    if (mEntityManager_.mSignatures[mEntity_][clay::ecs::ComponentType::PHYSICS_BODY_2D]) {
        auto& physicsBody = mEntityManager_.mPhysicsBodies2D[mEntity_];
        
        float moveSpeed = 1.5f;
        glm::vec2 movement(0.0f);
        
        // WASD movement
        if (inputHandler.isKeyPressed(GLFW_KEY_W)) {
            movement.y += 1.0f;
        }
        if (inputHandler.isKeyPressed(GLFW_KEY_S)) {
            movement.y -= 1.0f;
        }
        if (inputHandler.isKeyPressed(GLFW_KEY_A)) {
            movement.x -= 1.0f;
        }
        if (inputHandler.isKeyPressed(GLFW_KEY_D)) {
            movement.x += 1.0f;
        }
        
        // Normalize movement vector and apply speed
        float magnitude = glm::length(movement);
        if (magnitude > 0.0f) {
            movement = (movement / magnitude) * moveSpeed;
        }
        
        // Set velocity (CollisionSystem will handle movement and collision resolution)
        physicsBody.velocity = movement;
        
        // Control animation playback based on movement (RenderSystem handles frame updates)
        if (mEntityManager_.mSignatures[mEntity_][clay::ecs::ComponentType::ANIMATION_2D_RENDERABLE]) {
            auto& animRenderable = mEntityManager_.mAnimation2DRenderables[mEntity_];
            
            if (magnitude > 0.0f) {
                // Determine direction based on movement vector
                Direction newDirection = mDirection_;
                
                // Use the dominant direction (larger component)
                if (std::abs(movement.x) > std::abs(movement.y)) {
                    // Horizontal movement is dominant
                    if (movement.x < 0.0f) {
                        newDirection = Direction::LEFT;
                    } else {
                        newDirection = Direction::RIGHT;
                    }
                } else {
                    // Vertical movement is dominant
                    if (movement.y > 0.0f) {
                        newDirection = Direction::UP;
                    } else {
                        newDirection = Direction::DOWN;
                    }
                }
                
                // If direction changed, switch to the new animation
                if (newDirection != mDirection_) {
                    mDirection_ = newDirection;
                    
                    switch (mDirection_) {
                        case Direction::DOWN:
                            animRenderable.animationHandle = mResources_.getHandle<clay::Animation2D>("PlayerWalkDown");
                            break;
                        case Direction::LEFT:
                            animRenderable.animationHandle = mResources_.getHandle<clay::Animation2D>("PlayerWalkLeft");
                            break;
                        case Direction::RIGHT:
                            animRenderable.animationHandle = mResources_.getHandle<clay::Animation2D>("PlayerWalkRight");
                            break;
                        case Direction::UP:
                            animRenderable.animationHandle = mResources_.getHandle<clay::Animation2D>("PlayerWalkUp");
                            break;
                    }
                    
                    // Reset animation to start with new animation
                    animRenderable.currentFrame = 0;
                    animRenderable.currentTime = 0.0f;
                }
                
                animRenderable.playing = true;
            } else {
                // Idle - pause animation (will show first frame of current direction)
                animRenderable.playing = false;
            }
        }
    }
}

glm::vec3 Player::getPosition() const {
    if (mEntityManager_.mSignatures[mEntity_][clay::ecs::ComponentType::TRANSFORM]) {
        return mEntityManager_.mTransforms[mEntity_].mPosition_;
    }
    return glm::vec3(0.0f);
}

} // namespace rpg_2d