#pragma once
#include <vulkan/vulkan.h>

namespace pong {

class PongScene;

class PongSceneGUI {
public:
    PongSceneGUI(PongScene& scene);

    ~PongSceneGUI();

    void render(vk::CommandBuffer cmdBuffer);
    
    PongScene& mScene_;

    bool mVSyncEnabled_ = false;
    int mPlayerCount_ = 2;  // 1 = single player (AI), 2 = two player
};

} // namespace pong