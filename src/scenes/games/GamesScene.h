#pragma once
#include <clay/application/common/BaseScene.h>

class GamesScene : public clay::BaseScene {
public:
    GamesScene(clay::BaseApp& app);

    ~GamesScene();

    void update(const float dt) override;

    void render(vk::CommandBuffer cmdBuffer) override;

    void renderGUI(vk::CommandBuffer cmdBuffer) override;

    void initialize() override;

    void destroyResources() override;
private:

};