#pragma once

// clay
#include <clay/application/common/BaseScene.h>
#include <clay/ecs/EntityManager.h>
// project
#include "scenes/games/rpg_2d/RPG2dSceneGUI.h"
#include "scenes/games/rpg_2d/RPG2dGame.h"

namespace rpg_2d {

class RPG2dScene : public clay::BaseScene {
public:
    RPG2dScene(clay::BaseApp& app);
    ~RPG2dScene();
        
    void update(const float dt) override;

    void render(vk::CommandBuffer cmdBuffer) override;

    void renderGUI(vk::CommandBuffer cmdBuffer) override;

    void initialize() override;

    void destroyResources() override;

public:
    RPG2dSceneGUI mGui_;
    RPG2dGame mGame_;
};

} // namespace rpg_2d
