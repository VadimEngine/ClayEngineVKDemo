#pragma once
// standard lib
#include <set>
#include <memory>
#include <vector>
// clay
#include <clay/ecs/EntityManager.h>
#include <clay/graphics/common/Tilemap.h>
// local
#include "entities/Player.h"

namespace platform_2d {

class Platformer2dScene;

class Platformer2dGame {
public:
    Platformer2dGame(Platformer2dScene& scene);
    ~Platformer2dGame(); 

    void update(float dt);

    void render(vk::CommandBuffer cmdBuffer);

public:
    std::set<clay::ecs::Entity> mEntities_;

    clay::ecs::EntityManager mEntityManager_;

    Platformer2dScene& mScene_;
    
    // Tilemap
    std::unique_ptr<clay::Tilemap> mTilemap_;
    
    // Player
    std::unique_ptr<Player> mPlayer_;
};

} // namespace platform_2d
