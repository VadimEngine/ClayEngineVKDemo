#pragma once
#include <vulkan/vulkan.h>

namespace basic_scene {

class BasicScene;

class BasicSceneGUI {
public:
    BasicSceneGUI(BasicScene& scene);

    ~BasicSceneGUI();

    void render(vk::CommandBuffer cmdBuffer);

    void cameraSection();

    void entitiesSection();

    BasicScene& mScene_;

    bool mVSyncEnabled_ = false;

    int mCameraMode_;

    int mSelectedEntityIndex_ = 0;
    int mSelectedRenderableIndex_ = 0;

};

} // namespace basic_scene
