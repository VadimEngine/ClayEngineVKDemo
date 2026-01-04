#pragma once
// clay
#include <clay/ecs/EntityManager.h>
#include <clay/application/common/Resources.h>
// third party
#include <glm/glm.hpp>

namespace rpg_2d {

enum class ItemType {
    ROCK,
    STICK,
    BERRY
};

class Item {
public:
    Item(clay::ecs::EntityManager& entityManager, clay::Resources& resources,
         const glm::vec3& position, ItemType type);
    ~Item() = default;

    clay::ecs::Entity getEntity() const { return mEntity_; }
    glm::vec3 getPosition() const;
    ItemType getType() const { return mType_; }

private:
    static glm::vec4 getSpriteOffset(ItemType type);
    
    clay::ecs::EntityManager& mEntityManager_;
    clay::Resources& mResources_;
    clay::ecs::Entity mEntity_;
    ItemType mType_;
};

} // namespace rpg_2d