// clay
#include <glm/gtc/matrix_transform.hpp>
// class
#include "Minimap.h"

namespace rpg_2d {

Minimap::Minimap(clay::Resources& resources, int mapWidth, int mapHeight)
    : mResources_(resources)
    , mMapWidth_(mapWidth)
    , mMapHeight_(mapHeight) {
}

Minimap::~Minimap() {
}

void Minimap::render(vk::CommandBuffer cmdBuffer, const glm::vec3& playerPosition, 
                     const glm::mat4& cameraView, const glm::mat4& cameraProj) {
    // Get resources
    clay::Mesh* pPlaneMesh = &mResources_[mResources_.getHandle<clay::Mesh>("Plane")];
    clay::Material* pRpgMapMaterial = &mResources_[mResources_.getHandle<clay::Material>("RpgMap")];
    clay::Material* pFlatMaterial = &mResources_[mResources_.getHandle<clay::Material>("SolidTexture")];
    
    // For orthographic camera, extract the view bounds from projection matrix
    // Orthographic projection maps world coordinates to NDC
    // We can extract camera position from view matrix and use FOV to calculate visible area
    glm::mat4 invView = glm::inverse(cameraView);
    glm::vec3 cameraPos = glm::vec3(invView[3]);
    
    // For orthographic camera with FOV = 5.0, this determines the view height
    // The visible height in world space is approximately 2 * FOV for orthographic
    float viewHeight = 5.0f;  // Match the FOV setting
    float viewWidth = viewHeight;  // Assuming square view
    
    // Calculate minimap size in world space - made bigger (20% instead of 12.5%)
    float minimapWorldSize = viewHeight * 0.2f;
    
    // Position in top-right corner of the view
    float minimapX = cameraPos.x + viewWidth * 0.3f;   // Moved left from 0.4f
    float minimapY = cameraPos.y + viewHeight * 0.2f; // Moved down from 0.3f
    
    // --- Render minimap background (the map texture) ---
    {
        // Bind RpgMap material for the minimap background
        pRpgMapMaterial->bindMaterial(cmdBuffer);
        
        glm::vec3 minimapPos = glm::vec3(minimapX, minimapY, 5.0f);  // Z=5 to be in front of game
        glm::vec3 minimapScale = glm::vec3(minimapWorldSize, minimapWorldSize, 1.0f);
        
        glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), minimapPos);
        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), minimapScale);
        
        glm::mat4 modelMat = translationMat * scaleMat;
        
        struct PushConstants {
            glm::mat4 model;
            glm::vec4 color;
            glm::vec4 offsets;
        } push{};
        
        push.model = modelMat;
        push.color = glm::vec4(1.0f, 1.0f, 1.0f, 0.8f); // Slightly transparent
        push.offsets = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // No flip needed - texture is correct orientation
        
        pRpgMapMaterial->pushConstants(
            cmdBuffer,
            &push,
            sizeof(push),
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment
        );
        
        pPlaneMesh->bindMesh(cmdBuffer);
        cmdBuffer.drawIndexed(pPlaneMesh->getIndicesCount(), 1, 0, 0, 0);
    }
    
    // --- Render player position indicator (yellow dot) ---
    {
        // Bind Flat material for solid color rendering
        pFlatMaterial->bindMaterial(cmdBuffer);
        
        // Calculate player position on minimap
        // Player position is already in world coordinates with tiles at 0.2 spacing
        // Map dimensions in world space: width = mMapWidth_ * tileSize, height = mMapHeight_ * tileSize
        float tileSize = 0.2f; // Match the tile size from Tilemap
        float mapWorldWidth = mMapWidth_ * tileSize;
        float mapWorldHeight = mMapHeight_ * tileSize;
        
        // Player position relative to map center (map is centered at origin)
        // Normalize to -1 to 1 range
        float normalizedX = playerPosition.x / (mapWorldWidth * 0.5f);
        float normalizedY = playerPosition.y / (mapWorldHeight * 0.5f);
        
        // Map to minimap position in world space
        float playerWorldX = minimapX + normalizedX * minimapWorldSize * 0.5f;
        float playerWorldY = minimapY + normalizedY * minimapWorldSize * 0.5f;
        
        glm::vec3 indicatorPos = glm::vec3(playerWorldX, playerWorldY, 5.1f);  // Above minimap to prevent Z-fighting
        float indicatorSize = minimapWorldSize * 0.05f; // 5% of minimap size
        glm::vec3 indicatorScale = glm::vec3(indicatorSize, indicatorSize, 1.0f);
        
        glm::mat4 translationMat = glm::translate(glm::mat4(1.0f), indicatorPos);
        glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), indicatorScale);
        
        glm::mat4 modelMat = translationMat * scaleMat;
        
        struct PushConstants {
            glm::mat4 model;
            glm::vec4 color;
        } push{};
        
        push.model = modelMat;
        push.color = glm::vec4(1.0f, 1.0f, 0.0f, 1.0f); // Yellow
        
        pFlatMaterial->pushConstants(
            cmdBuffer,
            &push,
            sizeof(push),
            vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment
        );
        
        pPlaneMesh->bindMesh(cmdBuffer);
        cmdBuffer.drawIndexed(pPlaneMesh->getIndicesCount(), 1, 0, 0, 0);
    }
}

} // namespace rpg_2d