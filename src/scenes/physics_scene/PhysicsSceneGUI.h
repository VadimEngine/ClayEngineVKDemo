#pragma once
#include <vulkan/vulkan.h>

namespace physics_scene {

class PhysicsScene;

class PhysicsSceneGUI {
public:
    PhysicsSceneGUI(PhysicsScene& scene);

    ~PhysicsSceneGUI();

    void render(VkCommandBuffer cmdBuffer);

    void entitySection();

    PhysicsScene& mScene_;

    bool mVSyncEnabled_ = false;

    int mSelectedEntityIndex_ = 0;
    int mSelectedRenderableIndex_ = 0;
};
} // physics_scene