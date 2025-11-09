#pragma once

#include <clay/application/common/BaseScene.h>

class MenuScene : public clay::BaseScene {
public:
    MenuScene(clay::BaseApp& app);

    ~MenuScene();

    void update(const float dt) override;

    void render(vk::CommandBuffer cmdBuffer) override;

    void renderGUI(vk::CommandBuffer cmdBuffer) override;

    void initialize() override;

    void destroyResources() override;

private:
};