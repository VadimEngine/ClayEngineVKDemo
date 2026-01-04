#pragma once
// standard lib
#include <set>
#include <memory>
// clay
#include <clay/ecs/EntityManager.h>
#include <clay/graphics/common/Tilemap.h>
// class
#include "ui/Minimap.h"
#include "entities/Player.h"
#include "entities/NPC.h"
#include "items/Item.h"

namespace rpg_2d {

class RPG2dScene;

class RPG2dGame {
public:
    RPG2dGame(RPG2dScene& scene);
    ~RPG2dGame(); 

    void update(float dt);

    void render(vk::CommandBuffer cmdBuffer);

public:
    std::set<clay::ecs::Entity> mEntities_;

    clay::ecs::EntityManager mEntityManager_;

    RPG2dScene& mScene_;
    
    // Player
    std::unique_ptr<Player> mPlayer_;
    
    // NPCs
    std::unique_ptr<NPC> mNPC_;
    
    // Items
    std::unique_ptr<Item> mItem_;

    // Tilemap
    std::unique_ptr<clay::Tilemap> mTilemap_;
    
    // UI
    std::unique_ptr<Minimap> mMinimap_;
};

} // namespace rpg_2d
