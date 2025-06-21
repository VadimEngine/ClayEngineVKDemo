#pragma once

#include <clay/application/common/BaseScene.h>

class PhysicsScene : public clay::BaseScene {
public:
    PhysicsScene(clay::BaseApp& app);

    ~PhysicsScene();

    void update(const float dt) override;

    void render(VkCommandBuffer cmdBuffer) override;

    void renderGUI(VkCommandBuffer cmdBuffer) override;

    void initialize() override;

    void destroyResources() override;

private:

};