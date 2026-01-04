#pragma once
#include <vulkan/vulkan.h>

namespace platform_2d {

class Platformer2dScene;

class Platformer2dSceneGUI {
public:
    Platformer2dSceneGUI(Platformer2dScene& scene);

    ~Platformer2dSceneGUI();

    void render(vk::CommandBuffer cmdBuffer);
    
    Platformer2dScene& mScene_;

    bool mVSyncEnabled_ = false;
    bool mShowColliders_ = false;
};

} // namespace platform_2d
