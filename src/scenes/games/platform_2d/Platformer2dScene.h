#pragma once

// clay
#include <clay/application/common/BaseScene.h>
#include <clay/ecs/EntityManager.h>
// project
#include "scenes/games/platform_2d/Platformer2dSceneGUI.h"
#include "scenes/games/platform_2d/Platformer2dGame.h"

namespace platform_2d {

class Platformer2dScene : public clay::BaseScene {
public:
    Platformer2dScene(clay::BaseApp& app);
    ~Platformer2dScene();
        
    void update(const float dt) override;

    void render(vk::CommandBuffer cmdBuffer) override;

    void renderGUI(vk::CommandBuffer cmdBuffer) override;

    void initialize() override;

    void destroyResources() override;

public:
    Platformer2dSceneGUI mGui_;
    Platformer2dGame mGame_;
};

} // namespace platform_2d
