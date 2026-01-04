#pragma once
#include <vulkan/vulkan.h>

namespace rpg_2d {

class RPG2dScene;

class RPG2dSceneGUI {
public:
    RPG2dSceneGUI(RPG2dScene& scene);

    ~RPG2dSceneGUI();

    void render(vk::CommandBuffer cmdBuffer);
    
    RPG2dScene& mScene_;

    bool mVSyncEnabled_ = false;
    bool mShowColliders_ = false;

    float mVolume_;
};

} // namespace rpg_2d
