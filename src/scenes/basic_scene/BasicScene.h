#pragma once
// clay
#include <clay/application/common/BaseScene.h>
// project
#include "scenes/basic_scene/BasicSceneGUI.h"

namespace basic_scene {

class BasicScene : public clay::BaseScene {
public:
    BasicScene(clay::BaseApp& app);

    ~BasicScene();

    void update(const float dt) override;

    void render(VkCommandBuffer cmdBuffer) override;

    void renderGUI(VkCommandBuffer cmdBuffer) override;

    void initialize() override;

    void destroyResources() override;

    std::vector<std::unique_ptr<clay::Entity>>& getEntities();

private:
    BasicSceneGUI mGui_;
    std::vector<std::unique_ptr<clay::Entity>> mEntities_;
};

} // namespace basic_scene