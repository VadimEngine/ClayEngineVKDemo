#pragma once
#include <vulkan/vulkan.h>

namespace galaxy {

class GalaxyScene;

class GalaxySceneGUI {
public:
    GalaxySceneGUI(GalaxyScene& scene);

    ~GalaxySceneGUI();

    void render(VkCommandBuffer cmdBuffer);

    GalaxyScene& mScene_;

    bool mVSyncEnabled_ = false;
};

} // namespace galaxy
