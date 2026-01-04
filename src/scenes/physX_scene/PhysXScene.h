
#pragma once
// standard lib
#include <set>
// third party
#include <PxPhysicsAPI.h>
// clay
#include <clay/application/common/BaseScene.h>
#include <clay/ecs/EntityManager.h>
// project
#include "scenes/physX_scene/PhysXSceneGUI.h"

namespace physX_scene {

class PhysXScene : public clay::BaseScene {
public:

    enum class ObjectType {
        SPHERE = 0,
        CUBE,
    };

    PhysXScene(clay::BaseApp& app);

    ~PhysXScene();

    void update(const float dt) override;

    void render(vk::CommandBuffer cmdBuffer) override;

    void renderGUI(vk::CommandBuffer cmdBuffer) override;

    void initialize() override;

    void addEntity(ObjectType type);

    void destroyResources() override;

private:
public:
    PhysXSceneGUI mGui_;

    std::set<clay::ecs::Entity> mEntities_;

    clay::ecs::EntityManager mEntityManager_;

    physx::PxScene* mPXScene_ = nullptr;
    physx::PxMaterial* mPXMaterial_ = nullptr;
    physx::PxRigidStatic* mGround_ = nullptr;
};

} // namespace physX_scene