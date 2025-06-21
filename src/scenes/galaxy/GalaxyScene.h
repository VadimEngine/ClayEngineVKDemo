#pragma once
// clay
#include <clay/application/common/BaseScene.h>

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

};