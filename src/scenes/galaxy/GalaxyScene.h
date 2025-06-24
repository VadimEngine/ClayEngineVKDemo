#pragma once
// clay
#include <clay/application/common/BaseScene.h>
// project
#include "scenes/galaxy/GalaxySceneGUI.h"
#include "scenes/galaxy/MoonEntity.h"

namespace galaxy {

// TODO add a skybox
class GalaxyScene : public clay::BaseScene {
public:
    GalaxyScene(clay::BaseApp& app);

    ~GalaxyScene();

    void update(const float dt) override;

    void render(VkCommandBuffer cmdBuffer) override;

    void renderGUI(VkCommandBuffer cmdBuffer) override;

    void initialize() override;

    void destroyResources() override;

private:
    GalaxySceneGUI mGui_;

    MoonEntity* moonEntity_ = nullptr;
};

} // namespace galaxy