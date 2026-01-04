// clay
#include <clay/application/desktop/AppDesktop.h>
#include <clay/utils/common/Logger.h>
#include <clay/utils/desktop/UtilsDesktop.h>
// class
#include "scenes/games/platform_2d/Platformer2dGame.h"
#include "scenes/games/platform_2d/Platformer2dScene.h"
#include "DemoApp.h"

namespace platform_2d {

Platformer2dGame::Platformer2dGame(Platformer2dScene& scene)
    : mEntityManager_(scene.getApp().getGraphicsContext(), scene.getApp().getResources())
    , mScene_(scene) {

    mScene_.getFocusCamera()->setMode(clay::Camera::Mode::ORTHOGRAPHIC);
    mScene_.getFocusCamera()->setPosition({0, 2, 10});
    mScene_.getFocusCamera()->setFOV(10.0f); // Zoom level TODO confirm this works for orthographic

    // Configure 2D physics
    auto& physicsSystem = mEntityManager_.mPhysicsSystem_;
    physicsSystem.setGravity2D(glm::vec2(0.0f, -20.0f));

    // Set debug rendering resources
    mEntityManager_.mRenderSystem_.setDebugResources(
        mScene_.getApp().getResources().getHandle<clay::Material>("SolidTexture"),
        mScene_.getApp().getResources().getHandle<clay::Mesh>("Plane")
    );

    {
        // background entity
        clay::ecs::Entity bgEntity = mEntityManager_.createEntity();
        mEntityManager_.addComponent<clay::ecs::ModelRenderable>(
            bgEntity, 
            { 
                ((DemoApp&)mScene_.getApp()).mAppProp.mModels.skyPlane,
                {1,1,1,1}
            }
        );
    


        clay::ecs::Transform transform;
        transform.mPosition_ = {0.0f, 1.0f, -10.0f};
        transform.mScale_ = {20.0f, 10.0f, 1.0f};
        mEntityManager_.addComponent<clay::ecs::Transform>(bgEntity, transform);
    }

    // Setup tilemap
    auto materialHandle = ((DemoApp&)mScene_.getApp()).mAppProp.mMaterials.tilemapInstanced;
    auto meshHandle = ((DemoApp&)mScene_.getApp()).mAppProp.mMeshes.plane;
    mTilemap_ = std::make_unique<clay::Tilemap>(mEntityManager_, mScene_.getApp().getResources(),
                                                 materialHandle, meshHandle);
    mTilemap_->setTileSize(0.15f); // 1 unit per tile
    
    // Register tile type: green = platform tile
    // Tile ID 0: Platform tile (green in image, sprite at 0,1 in spritesheet)
    mTilemap_->registerTile(0, {
        glm::vec4(0.0f * 16.0f / 512.0f, 1.0f * 16.0f / 512.0f, 16.0f / 512.0f, 16.0f / 512.0f),
        glm::vec4(1, 1, 1, 1)
    });
    
    // Parse image to tile IDs using color mapping
    clay::utils::ImageData imageData = clay::utils::loadImageFileToMemory_desktop(
        clay::Resources::getResourcePath() / "textures/platform_2d/Level1.png");
    
    if (!imageData.pixels) {
        LOG_E("Failed to load tilemap image");
        return;
    }
    
    int mapWidth = imageData.width;
    int mapHeight = imageData.height;
    
    // Create tile ID array from image
    std::vector<std::vector<int>> tileIds(mapHeight, std::vector<int>(mapWidth, -1));
    
    std::unordered_map<uint32_t, int> colorToTileId = {
        {0x00FF00, 0}   // Green -> Platform tile
    };
    
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int index = (y * mapWidth + x) * imageData.channels;
            
            // Read pixel color
            unsigned char r = imageData.pixels[index];
            unsigned char g = imageData.pixels[index + 1];
            unsigned char b = imageData.pixels[index + 2];
            uint32_t combinedColor = (r << 16) | (g << 8) | b;
            
            // Look up tile ID from color (black = no tile)
            // Flip Y-axis: image Y=0 is top, but world Y should increase upward
            int flippedY = mapHeight - 1 - y;
            auto it = colorToTileId.find(combinedColor);
            if (it != colorToTileId.end()) {
                tileIds[flippedY][x] = it->second;
            }
        }
    }
    
    // Load tilemap with tile IDs
    mTilemap_->load(tileIds);
    
    // Create 2D physics collision blocks for platform tiles
    float tileSize = mTilemap_->getTileSize();
    
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int index = (y * mapWidth + x) * imageData.channels;
            
            // Read pixel color
            unsigned char r = imageData.pixels[index];
            unsigned char g = imageData.pixels[index + 1];
            unsigned char b = imageData.pixels[index + 2];
            uint32_t combinedColor = (r << 16) | (g << 8) | b;
            
            // If green (platform) or gray (hidden wall), create 2D physics collider
            if (combinedColor == 0x00FF00 || combinedColor == 0x808080) {
                // Flip Y-axis: image Y=0 is top, but world Y should increase upward
                int flippedY = mapHeight - 1 - y;
                float worldX = (x - mapWidth / 2.0f) * tileSize;
                float worldY = (flippedY - mapHeight / 2.0f) * tileSize;
                
                // Create platform entity with 2D physics
                clay::ecs::Entity platformEntity = mEntityManager_.createEntity();
                
                // Add Transform component
                clay::ecs::Transform transform;
                transform.mPosition_ = glm::vec3(worldX, worldY, 0.0f);
                transform.mOrientation_ = glm::quat(1, 0, 0, 0);
                transform.mScale_ = glm::vec3(1.0f, 1.0f, 1.0f);
                mEntityManager_.addComponent<clay::ecs::Transform>(platformEntity, transform);
                
                // Add 2D physics body (static platform)
                clay::ecs::PhysicsBody2D physicsBody;
                physicsBody.type = clay::ecs::PhysicsBody2D::Type::STATIC;
                physicsBody.isSolid = true;
                physicsBody.isTrigger = false;
                mEntityManager_.addComponent<clay::ecs::PhysicsBody2D>(platformEntity, physicsBody);
                
                // Add AABB collider
                // halfExtents directly specify world dimensions (no scaling)
                // tileSize/2 = 0.15/2 = 0.075 in each direction
                clay::ecs::Collider collider;
                collider.mType_ = clay::ecs::Collider::Type::AABB;
                collider.offset = glm::vec3(0.0f, 0.0f, 0.0f);
                collider.aabb.halfExtents = glm::vec3(tileSize / 2.0f, tileSize / 2.0f, 1.0f);
                mEntityManager_.addComponent<clay::ecs::Collider>(platformEntity, collider);
                
                mEntities_.insert(platformEntity);
            }
        }
    }
    
    // Create player
    mPlayer_ = std::make_unique<Player>(mEntityManager_, mScene_.getApp().getResources());
    mEntities_.insert(mPlayer_->getEntity());
}

Platformer2dGame::~Platformer2dGame() {}

void Platformer2dGame::update(float dt) {
    // Handle player input
    const clay::InputHandlerDesktop& inputHandler = (((clay::AppDesktop&)mScene_.getApp()).getWindow().getInputHandler());
    
    // Update player
    mPlayer_->update(dt, inputHandler);
    
    // Update physics
    mEntityManager_.update(dt);
}

void Platformer2dGame::render(vk::CommandBuffer cmdBuffer) {
    // Tilemaps are automatically rendered by EntityManager's RenderSystem
    
    // Render entities (player)
    mEntityManager_.render(cmdBuffer);
}

} // namespace platform_2d
