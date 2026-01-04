// clay
#include <clay/application/desktop/AppDesktop.h>
#include <clay/utils/common/Logger.h>
#include <clay/utils/desktop/UtilsDesktop.h>
// class
#include "scenes/games/rpg_2d/RPG2dGame.h"
#include "scenes/games/rpg_2d/RPG2dScene.h"

namespace rpg_2d {

RPG2dGame::RPG2dGame(RPG2dScene& scene)
    : mEntityManager_(scene.getApp().getGraphicsContext(), scene.getApp().getResources()),
     mScene_(scene) {

    mScene_.getFocusCamera()->setMode(clay::Camera::Mode::ORTHOGRAPHIC);
    mScene_.getFocusCamera()->setPosition({0,0,10});
    mScene_.getFocusCamera()->setFOV(5.0f); // Zoom in (smaller FOV = more zoom in orthographic)

    // Configure 2D physics with no gravity (top-down view)
    auto& physicsSystem = mEntityManager_.mPhysicsSystem_;
    physicsSystem.setGravity2D(glm::vec2(0.0f, 0.0f));

    // Set debug rendering resources
    mEntityManager_.mRenderSystem_.setDebugResources(
        mScene_.getApp().getResources().getHandle<clay::Material>("SolidTexture"),
        mScene_.getApp().getResources().getHandle<clay::Mesh>("Plane")
    );

    // Setup tilemap
    auto materialHandle = mScene_.getApp().getResources().getHandle<clay::Material>("TilemapInstanced");
    auto meshHandle = mScene_.getApp().getResources().getHandle<clay::Mesh>("Plane");
    mTilemap_ = std::make_unique<clay::Tilemap>(mEntityManager_, mScene_.getApp().getResources(),
                                                 materialHandle, meshHandle);
    
    // Register tile types with their definitions
    // Tile ID 0: Grass (green in image)
    mTilemap_->registerTile(0, {
        glm::vec4(0.0f * 16.0f / 512.0f, 1.0f * 16.0f / 512.0f, 16.0f / 512.0f, 16.0f / 512.0f),
        glm::vec4(1, 1, 1, 1)
    });
    
    // Tile ID 1: Water/Sea (blue in image)
    mTilemap_->registerTile(1, {
        glm::vec4(2.0f * 16.0f / 512.0f, 1.0f * 16.0f / 512.0f, 16.0f / 512.0f, 16.0f / 512.0f),
        glm::vec4(1, 1, 1, 1)
    });
    
    // Tile ID 2: Sand (yellow in image)
    mTilemap_->registerTile(2, {
        glm::vec4(3.0f * 16.0f / 512.0f, 1.0f * 16.0f / 512.0f, 16.0f / 512.0f, 16.0f / 512.0f),
        glm::vec4(1, 1, 1, 1)
    });
    
    // Parse image to tile IDs using color mapping
    clay::utils::ImageData imageData = clay::utils::loadImageFileToMemory_desktop(
        clay::Resources::getResourcePath() / "textures/rpg_2d/World1.png");
    
    if (!imageData.pixels) {
        LOG_E("Failed to load tilemap image");
        return;
    }
    
    int mapWidth = imageData.width;
    int mapHeight = imageData.height;
    
    // Create tile ID array from image
    std::vector<std::vector<int>> tileIds(mapHeight, std::vector<int>(mapWidth, -1));
    
    std::unordered_map<uint32_t, int> colorToTileId = {
        {0x00FF00, 0},  // Green -> Grass
        {0x0000FF, 1},  // Blue -> Water
        {0xFFFF00, 2}   // Yellow -> Sand
    };
    
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int index = (y * mapWidth + x) * imageData.channels;
            
            // Read pixel color
            unsigned char r = imageData.pixels[index];
            unsigned char g = imageData.pixels[index + 1];
            unsigned char b = imageData.pixels[index + 2];
            uint32_t combinedColor = (r << 16) | (g << 8) | b;
            
            // Look up tile ID from color
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
    
    // Create solid collision blocks for water tiles (blue pixels)
    float tileSize = mTilemap_->getTileSize();
    
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            int index = (y * mapWidth + x) * imageData.channels;
            
            // Read pixel color
            unsigned char r = imageData.pixels[index];
            unsigned char g = imageData.pixels[index + 1];
            unsigned char b = imageData.pixels[index + 2];
            uint32_t combinedColor = (r << 16) | (g << 8) | b;
            
            // If blue (water), create solid collision block
            if (combinedColor == 0x0000FF) {
                clay::ecs::Entity blockEntity = mEntityManager_.createEntity();
                
                // Transform
                // Flip Y-axis: image Y=0 is top, but world Y should increase upward
                int flippedY = mapHeight - 1 - y;
                clay::ecs::Transform transform;
                transform.mPosition_ = {
                    (x - mapWidth / 2.0f) * tileSize,
                    (flippedY - mapHeight / 2.0f) * tileSize,
                    -1.0f
                };
                transform.mScale_ = {1.0f, 1.0f, 1.0f};
                mEntityManager_.addComponent<clay::ecs::Transform>(blockEntity, transform);
                
                // Collider - halfExtents directly specify world dimensions
                clay::ecs::Collider collider;
                collider.mType_ = clay::ecs::Collider::Type::AABB;
                collider.aabb.halfExtents = glm::vec3(tileSize / 2.0f, tileSize / 2.0f, 0.1f);
                mEntityManager_.addComponent<clay::ecs::Collider>(blockEntity, collider);
                
                // Physics body - solid and blocks movement
                clay::ecs::PhysicsBody2D physicsBody;
                physicsBody.type = clay::ecs::PhysicsBody2D::Type::STATIC;
                physicsBody.isSolid = true;
                mEntityManager_.addComponent<clay::ecs::PhysicsBody2D>(blockEntity, physicsBody);
                
                mEntities_.insert(blockEntity);
            }
        }
    }
    
    // Initialize minimap
    mMinimap_ = std::make_unique<Minimap>(
        mScene_.getApp().getResources(),
        mTilemap_->getWidth(),
        mTilemap_->getHeight()
    );

    // Create player
    mPlayer_ = std::make_unique<Player>(mEntityManager_, mScene_.getApp().getResources());
    mEntities_.insert(mPlayer_->getEntity());
    
    // Create NPC at a different position
    mNPC_ = std::make_unique<NPC>(mEntityManager_, mScene_.getApp().getResources(),
                                   glm::vec3(0.5f, 0.0f, 0.0f), "PlayerWalkRight");
    mEntities_.insert(mNPC_->getEntity());
    
    // Create a ROCK item at (0, 0)
    mItem_ = std::make_unique<Item>(mEntityManager_, mScene_.getApp().getResources(),
                                     glm::vec3(0.0f, 0.0f, 0.0f), ItemType::ROCK);
    mEntities_.insert(mItem_->getEntity());
    // play background music TODO Loop music
    mScene_.getApp().getAudioManager().playSound(
         mScene_.getApp().getResources()[mScene_.getApp().getResources().getHandle<clay::Audio>("PatakasWorld")].getId()
    );
}

RPG2dGame::~RPG2dGame() {
    mScene_.getApp().getAudioManager().stopSound();
}

void RPG2dGame::update(float dt) {
    // Handle player input
    const clay::InputHandlerDesktop& inputHandler = (((clay::AppDesktop&)mScene_.getApp()).getWindow().getInputHandler());
    
    // Update player
    mPlayer_->update(dt, inputHandler);
    
    // Update camera to follow player
    glm::vec3 playerPos = mPlayer_->getPosition();
    mScene_.getFocusCamera()->setPosition({playerPos.x, playerPos.y, 10});
    
    mEntityManager_.update(dt);
}

void RPG2dGame::render(vk::CommandBuffer cmdBuffer) {
    // Tilemaps are automatically rendered by EntityManager's RenderSystem
    
    // Render entities (player, NPCs, items)
    mEntityManager_.render(cmdBuffer);
    
    // Render minimap (UI layer, always on top)
    if (mMinimap_) {
        glm::vec3 playerPos = mPlayer_->getPosition();
        glm::mat4 cameraView = mScene_.getFocusCamera()->getViewMatrix();
        glm::mat4 cameraProj = mScene_.getFocusCamera()->getProjectionMatrix();
        mMinimap_->render(cmdBuffer, playerPos, cameraView, cameraProj);
    }
}

} // namespace rpg_2d
