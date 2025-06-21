#pragma once

#include <clay/application/common/BaseScene.h>

class MenuScene : public clay::BaseScene {
public:
    MenuScene(clay::BaseApp& app);

    ~MenuScene();

    void update(const float dt) override;

    void render(VkCommandBuffer cmdBuffer) override;

    void renderGUI(VkCommandBuffer cmdBuffer) override;

    void initialize() override;

    void destroyResources() override;

private:
};