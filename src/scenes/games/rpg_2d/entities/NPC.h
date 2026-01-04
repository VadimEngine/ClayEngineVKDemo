#pragma once
// clay
#include <clay/ecs/EntityManager.h>
#include <clay/application/common/Resources.h>
// third party
#include <glm/glm.hpp>

namespace rpg_2d {

class NPC {
public:
    NPC(clay::ecs::EntityManager& entityManager, clay::Resources& resources,
        const glm::vec3& position, const std::string& animationName = "PlayerWalkDown");
    ~NPC() = default;

    clay::ecs::Entity getEntity() const { return mEntity_; }
    glm::vec3 getPosition() const;

private:
    clay::ecs::EntityManager& mEntityManager_;
    clay::Resources& mResources_;
    clay::ecs::Entity mEntity_;
};

} // namespace rpg_2d