#include "NPC.h"

namespace rpg_2d {

NPC::NPC(clay::ecs::EntityManager& entityManager, clay::Resources& resources,
         const glm::vec3& position, const std::string& animationName)
    : mEntityManager_(entityManager), mResources_(resources) {
    
    mEntity_ = mEntityManager_.createEntity();
    
    // Transform
    clay::ecs::Transform transform;
    transform.mPosition_ = position;
    transform.mPosition_.z = 0.5f;  // Layer 1: Characters
    transform.mScale_ = {0.2f, 0.2f, 1.0f};
    mEntityManager_.addComponent<clay::ecs::Transform>(mEntity_, transform);
    
    // Collider
    clay::ecs::Collider collider;
    collider.mType_ = clay::ecs::Collider::Type::AABB;
    collider.aabb.halfExtents = glm::vec3(0.25f, 0.5f, 1.0f);
    collider.offset = glm::vec3(-0.05f, 0.0f, 0.0f);
    mEntityManager_.addComponent<clay::ecs::Collider>(mEntity_, collider);
    
    // 2D physics body - static (doesn't move)
    clay::ecs::PhysicsBody2D npcPhysics;
    npcPhysics.type = clay::ecs::PhysicsBody2D::Type::STATIC;
    npcPhysics.velocity = {0.0f, 0.0f};
    npcPhysics.mass = 1.0f;
    npcPhysics.isSolid = true;
    npcPhysics.isTrigger = false;
    mEntityManager_.addComponent<clay::ecs::PhysicsBody2D>(mEntity_, npcPhysics);
    
    // Animation component - idle (not playing)
    clay::ecs::Animation2DRenderable animRenderable;
    animRenderable.animationHandle = mResources_.getHandle<clay::Animation2D>(animationName);
    animRenderable.meshHandle = mResources_.getHandle<clay::Mesh>("Plane");
    animRenderable.mColor_ = {1, 1, 1, 1};
    animRenderable.currentFrame = 0;
    animRenderable.currentTime = 0.0f;
    animRenderable.playing = false;  // NPCs are idle
    mEntityManager_.addComponent<clay::ecs::Animation2DRenderable>(mEntity_, animRenderable);
}

glm::vec3 NPC::getPosition() const {
    if (mEntityManager_.mSignatures[mEntity_][clay::ecs::ComponentType::TRANSFORM]) {
        return mEntityManager_.mTransforms[mEntity_].mPosition_;
    }
    return glm::vec3(0.0f);
}

} // namespace rpg_2d