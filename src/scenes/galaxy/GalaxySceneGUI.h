#pragma once
#include <vulkan/vulkan.h>

namespace galaxy {

class GalaxyScene;

class GalaxySceneGUI {
private:

public:
    GalaxySceneGUI(GalaxyScene& scene);

    ~GalaxySceneGUI();

    void render(VkCommandBuffer cmdBuffer);

    GalaxyScene& mScene_;
};

    
} // namespace galaxy
