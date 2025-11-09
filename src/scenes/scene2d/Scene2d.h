#pragma once
#include <clay/application/common/BaseScene.h>

class Scene2d : public clay::BaseScene {
public:
    Scene2d(clay::BaseApp& app);

    ~Scene2d();

    void update(const float dt) override;

    void render(vk::CommandBuffer cmdBuffer) override;

    void renderGUI(vk::CommandBuffer cmdBuffer) override;

    void initialize() override;

    void destroyResources() override;

private:

};