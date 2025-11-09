#pragma once
// standard lib
#include <set>
// clay
#include <clay/application/common/BaseScene.h>
#include <clay/ecs/EntityManager.h>
// project
#include "scenes/basic_scene/BasicSceneGUI.h"

namespace basic_scene {

class BasicScene : public clay::BaseScene {
public:
    BasicScene(clay::BaseApp& app);

    ~BasicScene();

    void update(const float dt) override;

    void render(vk::CommandBuffer cmdBuffer) override;

    void renderGUI(vk::CommandBuffer cmdBuffer) override;

    void initialize() override;

    void destroyResources() override;

    std::set<clay::ecs::Entity>& getEntities();

private:
    BasicSceneGUI mGui_;
    clay::ecs::Entity mTextureSphere_;
    
    std::set<clay::ecs::Entity> mECSEntities_;
    clay::ecs::EntityManager mEntityManager_;

};

} // namespace basic_scene