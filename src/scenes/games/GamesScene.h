#pragma once
#include <clay/application/common/BaseScene.h>

class GamesScene : public clay::BaseScene {
public:
    GamesScene(clay::IApp& app);

    ~GamesScene();

    void update(const float dt) override;

    void render(VkCommandBuffer cmdBuffer) override;

    void renderGUI(VkCommandBuffer cmdBuffer) override;

    void initialize() override;

    void destroyResources() override;
private:

};