#include "Item.h"

namespace rpg_2d {

Item::Item(clay::ecs::EntityManager& entityManager, clay::Resources& resources,
           const glm::vec3& position, ItemType type)
    : mEntityManager_(entityManager), mResources_(resources), mType_(type) {
    
    mEntity_ = mEntityManager_.createEntity();
    
    // Transform
    clay::ecs::Transform transform;
    transform.mPosition_ = position;
    transform.mPosition_.z = 0.0f;  // Layer 0: Ground items
    transform.mScale_ = {0.15f, 0.15f, 1.0f};
    mEntityManager_.addComponent<clay::ecs::Transform>(mEntity_, transform);
    
    // Sprite renderable
    
    clay::ecs::SpriteRenderable sprite;
    sprite.meshHandle = mResources_.getHandle<clay::Mesh>("Plane");
    sprite.materialHandle = mResources_.getHandle<clay::Material>("SpriteSheet");
    sprite.mSpriteOffset_ = getSpriteOffset(type);
    sprite.mColor_ = {1, 1, 1, 1};
    mEntityManager_.addComponent<clay::ecs::SpriteRenderable>(mEntity_, sprite);
    
    // Collider - trigger only (not blocking)
    clay::ecs::Collider collider;
    collider.mType_ = clay::ecs::Collider::Type::AABB;
    collider.aabb.halfExtents = glm::vec3(0.2f, 0.2f, 0.2f);
    mEntityManager_.addComponent<clay::ecs::Collider>(mEntity_, collider);
    
    // 2D physics body - trigger (non-solid)
    clay::ecs::PhysicsBody2D itemPhysics;
    itemPhysics.type = clay::ecs::PhysicsBody2D::Type::STATIC;
    itemPhysics.velocity = {0.0f, 0.0f};
    itemPhysics.mass = 0.0f;
    itemPhysics.isSolid = false;  // Not blocking
    itemPhysics.isTrigger = true;  // Only triggers events
    mEntityManager_.addComponent<clay::ecs::PhysicsBody2D>(mEntity_, itemPhysics);
}

glm::vec3 Item::getPosition() const {
    if (mEntityManager_.mSignatures[mEntity_][clay::ecs::ComponentType::TRANSFORM]) {
        return mEntityManager_.mTransforms[mEntity_].mPosition_;
    }
    return glm::vec3(0.0f);
}

glm::vec4 Item::getSpriteOffset(ItemType type) {
    switch (type) {
        case ItemType::ROCK:
            return glm::vec4(0 * 16.0f / 512.0f, 2 * 16.0f / 512.0f, 16.0f / 512.0f, 16.0f / 512.0f);
        case ItemType::STICK:
            return glm::vec4(1 * 16.0f / 512.0f, 2 * 16.0f / 512.0f, 16.0f / 512.0f, 16.0f / 512.0f);
        case ItemType::BERRY:
            return glm::vec4(3 * 16.0f / 512.0f, 2 * 16.0f / 512.0f, 16.0f / 512.0f, 16.0f / 512.0f);
        default:
            return glm::vec4(0.0f);
    }
}

} // namespace rpg_2d