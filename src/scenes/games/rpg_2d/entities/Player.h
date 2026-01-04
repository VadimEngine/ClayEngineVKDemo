#pragma once
// clay
#include <clay/ecs/EntityManager.h>
#include <clay/application/common/Resources.h>
#include <clay/application/desktop/InputHandlerDesktop.h>
// third party
#include <glm/glm.hpp>

namespace rpg_2d {

class Player {
public:
    Player(clay::ecs::EntityManager& entityManager, clay::Resources& resources);
    ~Player() = default;

    void update(float dt, const clay::InputHandlerDesktop& inputHandler);
    
    clay::ecs::Entity getEntity() const { return mEntity_; }
    glm::vec3 getPosition() const;

private:
    enum class Direction { DOWN, LEFT, RIGHT, UP };
    
    clay::ecs::EntityManager& mEntityManager_;
    clay::Resources& mResources_;
    clay::ecs::Entity mEntity_;
    Direction mDirection_ = Direction::DOWN;
};

} // namespace rpg_2d