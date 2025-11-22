
#pragma once
#include <vulkan/vulkan.h>

namespace physX_scene {

class PhysXScene;

class PhysXSceneGUI {
public:
    PhysXSceneGUI(PhysXScene& scene);

    ~PhysXSceneGUI();

    void render(vk::CommandBuffer cmdBuffer);

    void controlSection();

    PhysXScene& mScene_;

    bool mVSyncEnabled_ = false;

    int mSelectedEntityIndex_ = 0;
    int mSelectedRenderableIndex_ = 0;

};
} // physX_scene