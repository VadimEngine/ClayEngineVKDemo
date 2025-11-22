#pragma once
// standard lib
#include <set>
// clay
#include <clay/application/common/BaseScene.h>
#include <clay/ecs/EntityManager.h>
// project
#include "scenes/physics_scene/PhysicsSceneGUI.h"

/*
TODO different modes
    - 2d
    - 3d
    - particles with materials/lighting
*/

namespace physics_scene {

class PhysicsScene : public clay::BaseScene {
public:
    PhysicsScene(clay::BaseApp& app);

    ~PhysicsScene();

    void update(const float dt) override;

    void handleOverlap();

    void handleAttractions(float dt);
    
    std::optional<glm::vec3> getCollisionMVT(const clay::ecs::Entity& EntityA, const clay::ecs::Entity& EntityB);

    void render(vk::CommandBuffer cmdBuffer) override;

    void renderGUI(vk::CommandBuffer cmdBuffer) override;

    void initialize() override;

    void destroyResources() override;

    clay::ecs::EntityManager& getEntityManager();

    std::set<clay::ecs::Entity>& getEntities();

private:
    PhysicsSceneGUI mGui_;

    std::set<clay::ecs::Entity> mEntities_;

    clay::ecs::EntityManager mEntityManager_;
};

} // namespace physics_scene