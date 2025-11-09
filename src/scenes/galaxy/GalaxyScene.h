#pragma once
// standard lib
#include <set>
// clay
#include <clay/application/common/BaseScene.h>
#include <clay/graphics/common/SkyBox.h>
#include <clay/ecs/EntityManager.h>
// project
#include "scenes/galaxy/GalaxySceneGUI.h"
#include "scenes/galaxy/MoonEntity.h"
#include "scenes/galaxy/PlanetEntity.h"
#include "scenes/galaxy/SunEntity.h"

namespace galaxy {

class GalaxyScene : public clay::BaseScene {
public:
    GalaxyScene(clay::BaseApp& app);

    ~GalaxyScene();

    void update(const float dt) override;

    void render(vk::CommandBuffer cmdBuffer) override;

    void renderGUI(vk::CommandBuffer cmdBuffer) override;

    void initialize() override;

    void destroyResources() override;

private:
    GalaxySceneGUI mGui_;

    std::set<clay::ecs::Entity> mECSEntities_;
    clay::ecs::EntityManager mEntityManager_;

    SunEntity* mSunEntity_ = nullptr;
    PlanetEntity* mPlanetEntity_ = nullptr;
    MoonEntity* mMoonEntity_ = nullptr;

    clay::SkyBox* mSkyBox_;
};

} // namespace galaxy