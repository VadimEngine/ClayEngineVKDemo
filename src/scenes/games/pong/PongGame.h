#pragma once
// standard lib
#include <set>
// clay
#include <clay/ecs/EntityManager.h>

namespace pong {

class PongScene;

struct GameProperties {
    // Arena dimensions
    float arenaHalfWidth = 1.8f;
    float arenaHalfHeight = 1.3f;
    
    // Ball properties
    float ballRadius = 0.1f;
    float ballSpeed = 3.0f;
    
    // Paddle properties
    float paddleWidth = 0.1f;
    float paddleHalfHeight = 0.25f;
    float paddleOffsetFromWall = 0.1f;
    float paddleSpeed = 3.0f;
    
    // Wall properties
    float wallThickness = 0.1f;
    
    // Game rules
    int winningScore = 5;
    
    // Colors
    glm::vec4 ballColor = {1.0f, 1.0f, 1.0f, 1.0f};
    glm::vec4 paddleColor = {1.0f, 1.0f, 1.0f, 1.0f};
};

class PongGame {
public:
    PongGame(PongScene& scene);
    ~PongGame(); 

    void update(float dt);

    void render(vk::CommandBuffer cmdBuffer);

    void setAIMode(bool enabled) { mAIMode_ = enabled; }

public:
    std::set<clay::ecs::Entity> mEntities_;

    clay::ecs::EntityManager mEntityManager_ ;

    PongScene& mScene_;


private:
    // Game configuration
    GameProperties mGameProps_;
    
    // Game entities
    clay::ecs::Entity mBallEntity_;
    clay::ecs::Entity mLeftPaddleEntity_;
    clay::ecs::Entity mRightPaddleEntity_;
    clay::ecs::Entity mTopWallEntity_;
    clay::ecs::Entity mBottomWallEntity_;
    clay::ecs::Entity mLeftGoalEntity_;
    clay::ecs::Entity mRightGoalEntity_;
    clay::ecs::Entity mScoreTextEntity_;

    // Game state
    glm::vec3 mBallVelocity_ = {0.0f, 0.0f, 0.0f};
    bool mBallWaitingToLaunch_ = true;
    int mLastScorer_ = 0; // 1 = left player scored, 2 = right player scored, 0 = game start
    int mLeftScore_ = 0;
    int mRightScore_ = 0;
    int mPaddleBounceCount_ = 0;
    float mOriginalBallSpeed_ = 0.0f;
    
    // Cached audio
    int mBlipSoundId_ = -1;
    
    // AI control
    bool mAIMode_ = false;

    void resetBall();
    void setBallVelocity(float dirX, float dirY);
    void playBlip();
};
} // namespace pong