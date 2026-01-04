#pragma once
#include <vulkan/vulkan.h>

namespace rpg_3d {

class RPG3dScene;

class RPG3dSceneGUI {
public:
    RPG3dSceneGUI(RPG3dScene& scene);

    ~RPG3dSceneGUI();

    void render(vk::CommandBuffer cmdBuffer);
    
    RPG3dScene& mScene_;

    bool mVSyncEnabled_ = false;
};

} // namespace rpg_3d
