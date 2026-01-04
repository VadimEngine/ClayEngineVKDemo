// clay
#include <clay/application/desktop/AppDesktop.h>
#include <clay/utils/common/Logger.h>
// class
#include "scenes/games/pong/PongGame.h"
#include "scenes/games/pong/PongScene.h"

namespace pong {

PongGame::PongGame(PongScene& scene)
    : mEntityManager_(scene.getApp().getGraphicsContext(), scene.getApp().getResources())
    , mScene_(scene) {

        // Store original ball speed
        mOriginalBallSpeed_ = mGameProps_.ballSpeed;

        // Cache audio resource
        mBlipSoundId_ = mScene_.getApp().getResources()[mScene_.getApp().getResources().getHandle<clay::Audio>("Blip1")].getId();

        mScene_.getFocusCamera()->setMode(clay::Camera::Mode::ORTHOGRAPHIC);
        mScene_.getFocusCamera()->setPosition({0,0,10});

        // add up wall (invisible collider at top edge)
        {
            mTopWallEntity_ = mEntityManager_.createEntity();

            mEntityManager_.addComponent<clay::ecs::Transform>(
                mTopWallEntity_,
                {
                    .mPosition_ = {0.0f, mGameProps_.arenaHalfHeight + mGameProps_.ballRadius, 0.0f},
                    .mScale_ = {mGameProps_.arenaHalfWidth * 2.0f, mGameProps_.wallThickness, 1.0f}
                }
            );

            mEntityManager_.addComponent<clay::ecs::Collider>(
                mTopWallEntity_,
                {
                    .mType_ = clay::ecs::Collider::Type::AABB,
                    .aabb = {
                        .halfExtents = {mGameProps_.arenaHalfWidth, mGameProps_.wallThickness / 2.0f, 1.0f}
                    }
                }
            );

            mEntities_.insert(mTopWallEntity_);
        }
        // add down wall (invisible collider at bottom edge)
        {
            mBottomWallEntity_ = mEntityManager_.createEntity();

            mEntityManager_.addComponent<clay::ecs::Transform>(
                mBottomWallEntity_,
                {
                    .mPosition_ = {0.0f, -mGameProps_.arenaHalfHeight - mGameProps_.ballRadius, 0.0f},
                    .mScale_ = {mGameProps_.arenaHalfWidth * 2.0f, mGameProps_.wallThickness, 1.0f}
                }
            );

            mEntityManager_.addComponent<clay::ecs::Collider>(
                mBottomWallEntity_,
                {
                    .mType_ = clay::ecs::Collider::Type::AABB,
                    .aabb = {
                        .halfExtents = {mGameProps_.arenaHalfWidth, mGameProps_.wallThickness / 2.0f, 1.0f}
                    }
                }
            );

            mEntities_.insert(mBottomWallEntity_);
        }
        // add left goal wall (invisible collider at left edge)
        {
            mLeftGoalEntity_ = mEntityManager_.createEntity();

            mEntityManager_.addComponent<clay::ecs::Transform>(
                mLeftGoalEntity_,
                {
                    .mPosition_ = {-mGameProps_.arenaHalfWidth - mGameProps_.ballRadius, 0.0f, 0.0f},
                    .mScale_ = {mGameProps_.wallThickness, mGameProps_.arenaHalfHeight * 2.0f, 1.0f}
                }
            );

            mEntityManager_.addComponent<clay::ecs::Collider>(
                mLeftGoalEntity_,
                {
                    .mType_ = clay::ecs::Collider::Type::AABB,
                    .aabb = {
                        .halfExtents = {mGameProps_.wallThickness / 2.0f, mGameProps_.arenaHalfHeight, 1.0f}
                    }
                }
            );

            mEntities_.insert(mLeftGoalEntity_);
        }
        // add right goal wall (invisible collider at right edge)
        {
            mRightGoalEntity_ = mEntityManager_.createEntity();

            mEntityManager_.addComponent<clay::ecs::Transform>(
                mRightGoalEntity_,
                {
                    .mPosition_ = {mGameProps_.arenaHalfWidth + mGameProps_.ballRadius, 0.0f, 0.0f},
                    .mScale_ = {mGameProps_.wallThickness, mGameProps_.arenaHalfHeight * 2.0f, 1.0f}
                }
            );

            mEntityManager_.addComponent<clay::ecs::Collider>(
                mRightGoalEntity_,
                {
                    .mType_ = clay::ecs::Collider::Type::AABB,
                    .aabb = {
                        .halfExtents = {mGameProps_.wallThickness / 2.0f, mGameProps_.arenaHalfHeight, 1.0f}
                    }
                }
            );

            mEntities_.insert(mRightGoalEntity_);
        }
        // add ball
        {
            mBallEntity_ = mEntityManager_.createEntity();

            mEntityManager_.addComponent<clay::ecs::Transform>(
                mBallEntity_,
                {
                    .mPosition_ = {0.0f, 0.0f, 0.0f},
                    .mScale_ = {mGameProps_.ballRadius, mGameProps_.ballRadius, mGameProps_.ballRadius}
                }
            );

            mEntityManager_.addComponent<clay::ecs::ModelRenderable>(
                mBallEntity_,
                {
                    mScene_.getApp().getResources().getHandle<clay::Model>("SolidCircle"),
                    mGameProps_.ballColor
                }
            );

            mEntityManager_.addComponent<clay::ecs::Collider>(
                mBallEntity_,
                {
                    .mType_ = clay::ecs::Collider::Type::CIRCLE,
                    .circle = {
                        .radius = mGameProps_.ballRadius
                    }
                }
            );

            // Add collision action for bouncing
            mEntityManager_.addComponent<clay::ecs::CollisionAction>(
                mBallEntity_,
                {
                    .onEnter = [this](clay::ecs::Entity other) {
                        auto& ballTransform = mEntityManager_.mTransforms[mBallEntity_];
                        const float ballRadius = mGameProps_.ballRadius;
                        
                        // Bounce off top/bottom walls
                        if (other == mTopWallEntity_) {
                            mBallVelocity_.y = -std::abs(mBallVelocity_.y);  // Force downward
                            // Push ball out of collision
                            ballTransform.mPosition_.y = mGameProps_.arenaHalfHeight - ballRadius;
                            playBlip();
                        }
                        else if (other == mBottomWallEntity_) {
                            mBallVelocity_.y = std::abs(mBallVelocity_.y);  // Force upward
                            // Push ball out of collision
                            ballTransform.mPosition_.y = -mGameProps_.arenaHalfHeight + ballRadius;
                            playBlip();
                        }
                        // Bounce off paddles
                        else if (other == mLeftPaddleEntity_) {
                            mBallVelocity_.x = std::abs(mBallVelocity_.x);  // Force rightward
                            // Push ball out
                            ballTransform.mPosition_.x = -mGameProps_.arenaHalfWidth + mGameProps_.paddleOffsetFromWall + mGameProps_.paddleWidth / 2.0f + ballRadius;
                            playBlip();
                            
                            // Increase speed every 2 paddle bounces
                            mPaddleBounceCount_++;
                            if (mPaddleBounceCount_ % 2 == 0) {
                                mGameProps_.ballSpeed *= 1.1f;  // Increase by 10%
                                // Update velocity to maintain direction but with new speed
                                glm::vec3 direction = glm::normalize(mBallVelocity_);
                                mBallVelocity_ = direction * mGameProps_.ballSpeed;
                            }
                        }
                        else if (other == mRightPaddleEntity_) {
                            mBallVelocity_.x = -std::abs(mBallVelocity_.x);  // Force leftward
                            // Push ball out
                            ballTransform.mPosition_.x = mGameProps_.arenaHalfWidth - mGameProps_.paddleOffsetFromWall - mGameProps_.paddleWidth / 2.0f - ballRadius;
                            playBlip();
                            
                            // Increase speed every 2 paddle bounces
                            mPaddleBounceCount_++;
                            if (mPaddleBounceCount_ % 2 == 0) {
                                mGameProps_.ballSpeed *= 1.1f;  // Increase by 10%
                                // Update velocity to maintain direction but with new speed
                                glm::vec3 direction = glm::normalize(mBallVelocity_);
                                mBallVelocity_ = direction * mGameProps_.ballSpeed;
                            }
                        }
                        // Score - left wall (right player scores)
                        else if (other == mLeftGoalEntity_) {
                            mRightScore_++;
                            mLastScorer_ = 2;
                            resetBall();
                        }
                        // Score - right wall (left player scores)
                        else if (other == mRightGoalEntity_) {
                            mLeftScore_++;
                            mLastScorer_ = 1;
                            resetBall();
                        }
                    },
                    .onExit = nullptr
                }
            );

            mEntities_.insert(mBallEntity_);

            // Set initial ball position and wait for space to start
            resetBall();
        }
        // add left paddle
        {
            mLeftPaddleEntity_ = mEntityManager_.createEntity();

            mEntityManager_.addComponent<clay::ecs::Transform>(
                mLeftPaddleEntity_,
                {
                    .mPosition_ = {-mGameProps_.arenaHalfWidth + mGameProps_.paddleOffsetFromWall, 0.0f, 0.0f},
                    .mScale_ = {mGameProps_.paddleWidth, mGameProps_.paddleHalfHeight * 2.0f, 1.0f}
                }
            );

            mEntityManager_.addComponent<clay::ecs::ModelRenderable>(
                mLeftPaddleEntity_,
                {
                    mScene_.getApp().getResources().getHandle<clay::Model>("SolidPlane"),
                    mGameProps_.paddleColor
                }
            );

            mEntityManager_.addComponent<clay::ecs::Collider>(
                mLeftPaddleEntity_,
                {
                    .mType_ = clay::ecs::Collider::Type::AABB,
                    .aabb = {
                        .halfExtents = {mGameProps_.paddleWidth / 2.0f, mGameProps_.paddleHalfHeight, 0.5f}
                    }
                }
            );

            mEntities_.insert(mLeftPaddleEntity_);
        }
        // add right paddle
        {
            mRightPaddleEntity_ = mEntityManager_.createEntity();

            mEntityManager_.addComponent<clay::ecs::Transform>(
                mRightPaddleEntity_,
                {
                    .mPosition_ = {mGameProps_.arenaHalfWidth - mGameProps_.paddleOffsetFromWall, 0.0f, 0.0f},
                    .mScale_ = {mGameProps_.paddleWidth, mGameProps_.paddleHalfHeight * 2.0f, 1.0f}
                }
            );

            mEntityManager_.addComponent<clay::ecs::ModelRenderable>(
                mRightPaddleEntity_,
                {
                    mScene_.getApp().getResources().getHandle<clay::Model>("SolidPlane"),
                    mGameProps_.paddleColor
                }
            );

            mEntityManager_.addComponent<clay::ecs::Collider>(
                mRightPaddleEntity_,
                {
                    .mType_ = clay::ecs::Collider::Type::AABB,
                    .aabb = {
                        .halfExtents = {mGameProps_.paddleWidth / 2.0f, mGameProps_.paddleHalfHeight, 0.5f}
                    }
                }
            );

            mEntities_.insert(mRightPaddleEntity_);
        }
        
        // add text renderables for score display
        {
            mScoreTextEntity_ = mEntityManager_.createEntity();

            clay::ecs::TextRenderable text;
            text.setFont(&mScene_.getApp().getResources()[mScene_.getApp().getResources().getHandle<clay::Font>("Runescape")]);
            text.setText(mScene_.getApp().getGraphicsContext(), "0:0");
            text.mScale_ = {0.01f, 0.01f, 0.01f};
            text.mColor_ = {1.0f, 1.0f, 1.0f, 1.0f};
            
            mEntityManager_.addComponent<clay::ecs::TextRenderable>(
                mScoreTextEntity_,
                text
            );

            mEntityManager_.addComponent<clay::ecs::Transform>(
                mScoreTextEntity_,
                {
                    .mPosition_ = {0.0f, mGameProps_.arenaHalfHeight - 0.5f, 0.0f}
                }
            );

            mEntities_.insert(mScoreTextEntity_);
        }
}

PongGame::~PongGame() {
}

void PongGame::update(float dt) {
    // Clamp dt to prevent huge first-frame values
    const clay::InputHandlerDesktop& inputHandler = (((clay::AppDesktop&)mScene_.getApp()).getWindow().getInputHandler());

    const float paddleSpeed = mGameProps_.paddleSpeed;
    const float edgeBuffer = 0.02f; // Small buffer from edge
    const float minPaddleY = -mGameProps_.arenaHalfHeight + edgeBuffer;
    const float maxPaddleY = mGameProps_.arenaHalfHeight - edgeBuffer;

    // Left paddle movement (W/S keys)
    auto& leftPaddleTransform = mEntityManager_.mTransforms[mLeftPaddleEntity_];
    if (inputHandler.isKeyPressed(GLFW_KEY_W)) {
        leftPaddleTransform.mPosition_.y += paddleSpeed * dt;
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_S)) {
        leftPaddleTransform.mPosition_.y -= paddleSpeed * dt;
    }
    // Clamp left paddle position
    leftPaddleTransform.mPosition_.y = glm::clamp(leftPaddleTransform.mPosition_.y, minPaddleY, maxPaddleY);

    // Right paddle movement (Up/Down arrow keys or AI)
    auto& rightPaddleTransform = mEntityManager_.mTransforms[mRightPaddleEntity_];
    if (mAIMode_) {
        // AI mode: follow ball Y position when ball is in right half
        const auto& ballTransform = mEntityManager_.mTransforms[mBallEntity_];
        if (ballTransform.mPosition_.x > 0.0f) {
            // Ball is in right half, AI tracks it
            float targetY = ballTransform.mPosition_.y;
            float diff = targetY - rightPaddleTransform.mPosition_.y;
            
            // Move towards target with paddle speed
            if (std::abs(diff) > 0.01f) {
                float moveAmount = std::copysign(paddleSpeed * dt, diff);
                if (std::abs(moveAmount) > std::abs(diff)) {
                    moveAmount = diff;  // Don't overshoot
                }
                rightPaddleTransform.mPosition_.y += moveAmount;
            }
        }
    } else {
        // Human player control
        if (inputHandler.isKeyPressed(GLFW_KEY_UP)) {
            rightPaddleTransform.mPosition_.y += paddleSpeed * dt;
        }
        if (inputHandler.isKeyPressed(GLFW_KEY_DOWN)) {
            rightPaddleTransform.mPosition_.y -= paddleSpeed * dt;
        }
    }
    // Clamp right paddle position
    rightPaddleTransform.mPosition_.y = glm::clamp(rightPaddleTransform.mPosition_.y, minPaddleY, maxPaddleY);

    
    // Check for space key to launch ball
    if (mBallWaitingToLaunch_ && inputHandler.isKeyPressed(GLFW_KEY_SPACE)) {
        mBallWaitingToLaunch_ = false;
        // Launch ball in direction based on last scorer
        float dirX, dirY;
        if (mLastScorer_ == 1) {
            dirX = 1.0f;  // Left player scored, ball goes right
        } else if (mLastScorer_ == 2) {
            dirX = -1.0f; // Right player scored, ball goes left
        } else {
            dirX = (rand() % 2 == 0) ? 1.0f : -1.0f; // Random for game start
        }
        dirY = (rand() % 2 == 0) ? 1.0f : -1.0f; // Random up or down
        setBallVelocity(dirX, dirY);
    }
    
    // Update ball position based on velocity (only if not waiting to launch)
    if (!mBallWaitingToLaunch_) {
        auto& ballTransform = mEntityManager_.mTransforms[mBallEntity_];
        ballTransform.mPosition_ += mBallVelocity_ * dt;
    }

    // Update score display
    auto& scoreText = mEntityManager_.mTextRenderables[mScoreTextEntity_];
    std::string newScoreText = std::to_string(mLeftScore_) + ":" + std::to_string(mRightScore_);
    scoreText.setText(mScene_.getApp().getGraphicsContext(), newScoreText);

    mEntityManager_.update(dt);
}

void PongGame::render(vk::CommandBuffer cmdBuffer) {
    mEntityManager_.render(cmdBuffer);
}

void PongGame::resetBall() {
    // Reset ball to center
    auto& ballTransform = mEntityManager_.mTransforms[mBallEntity_];
    ballTransform.mPosition_ = glm::vec3{0.0f, 0.0f, 0.0f};

    // Stop the ball and wait for space key
    mBallVelocity_ = glm::vec3{0.0f, 0.0f, 0.0f};
    mBallWaitingToLaunch_ = true;
    
    // Reset ball speed and bounce counter
    mGameProps_.ballSpeed = mOriginalBallSpeed_;
    mPaddleBounceCount_ = 0;
}

void PongGame::setBallVelocity(float dirX, float dirY) {
    // Normalize direction and apply constant speed
    glm::vec3 direction = glm::normalize(glm::vec3(dirX, dirY, 0.0f));
    mBallVelocity_ = direction * mGameProps_.ballSpeed;
}

void PongGame::playBlip() {
    mScene_.getApp().getAudioManager().playSound(mBlipSoundId_);
}
} // namespace pong