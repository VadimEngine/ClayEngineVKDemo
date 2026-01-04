#pragma once

// clay
#include <clay/application/common/BaseScene.h>
#include <clay/ecs/EntityManager.h>
// project
#include "scenes/games/rpg_3d/RPG3dSceneGUI.h"
#include "scenes/games/rpg_3d/RPG3dGame.h"

namespace rpg_3d {

class RPG3dScene : public clay::BaseScene {
public:
    RPG3dScene(clay::BaseApp& app);
    ~RPG3dScene();
        
    void update(const float dt) override;

    void render(vk::CommandBuffer cmdBuffer) override;

    void renderGUI(vk::CommandBuffer cmdBuffer) override;

    void initialize() override;

    void destroyResources() override;

public:
    RPG3dSceneGUI mGui_;
    RPG3dGame mGame_;
};

} // namespace rpg_3d
