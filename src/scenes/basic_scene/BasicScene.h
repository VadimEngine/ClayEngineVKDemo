#pragma once
#include <clay/application/common/BaseScene.h>

class BasicScene : public clay::BaseScene {
public:
    BasicScene(clay::IApp& app);

    ~BasicScene();

    void update(const float dt) override;

    void render(VkCommandBuffer cmdBuffer) override;

    void renderGUI(VkCommandBuffer cmdBuffer) override;

    void initialize() override;

    void destroyResources() override;

private:
    std::vector<std::unique_ptr<clay::Entity>> mEntities_;
};
