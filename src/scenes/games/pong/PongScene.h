#pragma once

// clay
#include <clay/application/common/BaseScene.h>
#include <clay/ecs/EntityManager.h>
// project
#include "scenes/games/pong/PongSceneGUI.h"
#include "scenes/games/pong/PongGame.h"

namespace pong {

class PongScene : public clay::BaseScene {
public:
    PongScene(clay::BaseApp& app);
    ~PongScene();
        
    void update(const float dt) override;

    void render(vk::CommandBuffer cmdBuffer) override;

    void renderGUI(vk::CommandBuffer cmdBuffer) override;

    void initialize() override;

    void destroyResources() override;

private:
public:
    PongSceneGUI mGui_;
    PongGame mGame_;
};

} // namespace pong