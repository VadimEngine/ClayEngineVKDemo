#pragma once
// third party
#include <glm/glm.hpp>
#include <vulkan/vulkan.hpp>
// clay
#include <clay/application/common/Resources.h>
#include <clay/graphics/common/Mesh.h>
#include <clay/graphics/common/Material.h>
#include <clay/graphics/common/Texture.h>
#include <clay/ecs/EntityManager.h>

namespace rpg_2d {

class Minimap {
public:
    Minimap(clay::Resources& resources, int mapWidth, int mapHeight);
    ~Minimap();

    void render(vk::CommandBuffer cmdBuffer, const glm::vec3& playerPosition, 
                const glm::mat4& cameraView, const glm::mat4& cameraProj);

private:
    clay::Resources& mResources_;
    
    // Minimap properties
    int mMapWidth_;
    int mMapHeight_;
    float mMinimapSize_ = 0.25f; // Size relative to screen (0.25 = 25% of screen width)
    glm::vec2 mMinimapPosition_ = {1.0f, 2.0f}; // Top right corner in normalized coordinates
};

} // namespace rpg_2d