// third party
// imgui
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
// clay
#include <clay/application/desktop/AppDesktop.h>
// class
#include "scenes/menu_scene/MenuScene.h"
#include "scenes/physics_scene/PhysicsScene.h"

namespace physics_scene {

PhysicsScene::PhysicsScene(clay::BaseApp& app)
    : clay::BaseScene(app),
      mEntityManager_(app.getGraphicsContext(), app.getResources()),
      mGui_(*this) {
    getFocusCamera()->setPosition({0,0,5});
    {
        clay::ecs::Entity entity = mEntityManager_.createEntity();

        mEntityManager_.addModelRenderable(
            entity, 
            { 
                mApp_.getResources().getHandle<clay::Model>("SolidCircle"),
                {1,1,1,1}
            }
        );

        mEntityManager_.addTransform(entity, {});
        mEntityManager_.addCollider(entity, {});
        clay::ecs::RigidBody rigidBody{};
        rigidBody.attractive = true;
        mEntityManager_.addRigidBody(entity, rigidBody);
        mEntities_.insert(entity);
    }
    {
        clay::ecs::Entity entity = mEntityManager_.createEntity();

        mEntityManager_.addModelRenderable(
            entity, 
            { 
                mApp_.getResources().getHandle<clay::Model>("SolidCircle"),
                {1,1,1,1}
            }
        );

        clay::ecs::Transform transform{};
        transform.mPosition_ = {1,0,0};
        mEntityManager_.addTransform(entity, transform);
        mEntityManager_.addCollider(entity, {});
        clay::ecs::RigidBody rigidBody{};
        rigidBody.attractive = true;
        mEntityManager_.addRigidBody(entity, rigidBody);
        mEntities_.insert(entity);
    }
}

PhysicsScene::~PhysicsScene() {}

void PhysicsScene::update(const float dt) {
    const clay::InputHandlerDesktop& inputHandler = (((clay::AppDesktop&)mApp_).getWindow().getInputHandler());

    if (inputHandler.isKeyPressed(GLFW_KEY_W)) {
        mCamera_.move(mCamera_.getForward(), mCamera_.getMoveSpeed() * dt);
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_S)) {
        mCamera_.move(mCamera_.getForward(), -mCamera_.getMoveSpeed() * dt);
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_A)) {
        mCamera_.move(mCamera_.getRight(), -mCamera_.getMoveSpeed() * dt);
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_D)) {
        mCamera_.move(mCamera_.getRight(), mCamera_.getMoveSpeed() * dt);
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_SPACE)) {
        mCamera_.move(mCamera_.getUp(), mCamera_.getMoveSpeed() * dt);
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
        mCamera_.move(mCamera_.getUp(), -mCamera_.getMoveSpeed() * dt);
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_LEFT)) {
        mCamera_.rotate(mCamera_.getUp(), mCamera_.getRotationSpeed() * dt);
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_RIGHT)) {
        mCamera_.rotate(mCamera_.getUp(), -mCamera_.getRotationSpeed() * dt);
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_UP)) {
        mCamera_.rotate(mCamera_.getRight(), mCamera_.getRotationSpeed() * dt);
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_DOWN)) {
        mCamera_.rotate(mCamera_.getRight(), -mCamera_.getRotationSpeed() * dt);
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_Q)) {
        mCamera_.rotate(mCamera_.getForward(), 200.0f * dt);
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_E)) {
        mCamera_.rotate(mCamera_.getForward(), -200.0f * dt);
    }
    // Speed
    if (inputHandler.isKeyPressed(GLFW_KEY_PERIOD)) {
        mCamera_.zoom(-mCamera_.getZoomSpeed() * dt);
    }
    if (inputHandler.isKeyPressed(GLFW_KEY_COMMA)) {
        mCamera_.zoom(mCamera_.getZoomSpeed() * dt);
    }

    for (clay::ecs::Entity e: mEntities_) {
        if (mEntityManager_.mSignatures[e][clay::ecs::ComponentType::RIGID_BODY]) {
            // assume there is a transform
            mEntityManager_.mTransforms[e].mPosition_ += mEntityManager_.mRigidBodies[e].velocity;
        }
    }

    handleAttractions(dt);
    handleOverlap();
}

void PhysicsScene::handleOverlap() {
    
    for (auto it1 = mEntities_.begin(); it1 != mEntities_.end(); ++it1) {
        clay::ecs::Entity entityA = *it1;
        if (mEntityManager_.mSignatures[entityA][clay::ecs::ComponentType::COLLIDER]) {
            auto it2 = it1;
            ++it2;

            for (; it2 != mEntities_.end(); ++it2) {
                clay::ecs::Entity entityB = *it2;

                if (mEntityManager_.mSignatures[entityB][clay::ecs::ComponentType::COLLIDER]) {
                    // check if there is an overlap
                    auto mvt = getCollisionMVT(entityA, entityB);
                    if (mvt.has_value()) {
                        clay::ecs::Transform& transfromA = mEntityManager_.mTransforms[entityA];
                        clay::ecs::Transform& transfromB = mEntityManager_.mTransforms[entityB];

                        transfromA.mPosition_ += mvt.value();
                        transfromB.mPosition_ -= mvt.value();

                        // test with rigid
                        auto norm = glm::normalize(transfromA.mPosition_ - transfromB.mPosition_);

                        clay::ecs::RigidBody& rigidBodyA = mEntityManager_.mRigidBodies[entityA];
                        clay::ecs::RigidBody& rigidBodyB = mEntityManager_.mRigidBodies[entityB];

                        rigidBodyA.velocity = glm::reflect(rigidBodyA.velocity, glm::normalize(-norm));
                        rigidBodyB.velocity = glm::reflect(rigidBodyB.velocity, glm::normalize(-norm));
                    }
                }
            }
        }
    }
}

void PhysicsScene::handleAttractions(float dt) {
    for (auto it1 = mEntities_.begin(); it1 != mEntities_.end(); ++it1) {
        clay::ecs::Entity entityA = *it1;
        if (mEntityManager_.mSignatures[entityA][clay::ecs::ComponentType::RIGID_BODY] && mEntityManager_.mRigidBodies[entityA].attractive) {
            auto it2 = it1;
            ++it2;

            for (; it2 != mEntities_.end(); ++it2) {
                clay::ecs::Entity entityB = *it2;
                if (mEntityManager_.mSignatures[entityB][clay::ecs::ComponentType::RIGID_BODY] && mEntityManager_.mRigidBodies[entityB].attractive) {
                    clay::ecs::Transform& transfromA = mEntityManager_.mTransforms[entityA];
                    clay::ecs::Transform& transfromB = mEntityManager_.mTransforms[entityB];

                    clay::ecs::RigidBody& rigidBodyA = mEntityManager_.mRigidBodies[entityA];
                    clay::ecs::RigidBody& rigidBodyB = mEntityManager_.mRigidBodies[entityB];

                    const float distance = glm::length(transfromA.mPosition_ - transfromB.mPosition_);

                    const float epsilon = 1e-6f;

                    if (abs(distance) > epsilon) {
                        glm::vec3 dir = glm::vec3(
                            transfromA.mPosition_.x - transfromB.mPosition_.x,
                            transfromA.mPosition_.y - transfromB.mPosition_.y,
                            0.0f
                        );

                        const float scale = .1;
                        rigidBodyA.velocity = (rigidBodyA.velocity - (dir * static_cast<float>(scale * (1.0f / pow(distance, 2.0f)) * dt)));
                        rigidBodyB.velocity = (rigidBodyB.velocity + (dir * static_cast<float>(scale * (1.0f / pow(distance, 2.0f)) * dt)));
                    }
                }
            }
        }
    }
}

// this only supports 2d circle without rotation. need to add more shape and rotation support
// maybe return a collision struct that has information about the collider, rigidbody, mvt and normal
std::optional<glm::vec3> PhysicsScene::getCollisionMVT(const clay::ecs::Entity& entityA, const clay::ecs::Entity& entityB) {
    // TODO for now assume there is a transform and collider for both
    clay::ecs::Transform& transfromA = mEntityManager_.mTransforms[entityA];
    clay::ecs::Transform& transfromB = mEntityManager_.mTransforms[entityB];

    clay::ecs::Collider& colliderA = mEntityManager_.mColliders[entityA];
    clay::ecs::Collider& colliderB = mEntityManager_.mColliders[entityB];

    glm::vec3 colliderACenter = transfromA.mPosition_ + colliderA.offset;
    glm::vec3 colliderBCenter = transfromB.mPosition_ + colliderB.offset;

    glm::vec2 halfSizeA = glm::vec2{
        colliderA.scale.x / 2 * transfromA.mScale_.x,
        colliderA.scale.y / 2 * transfromA.mScale_.y
    };
    glm::vec2 halfSizeB = glm::vec2{
        colliderB.scale.x / 2 * transfromB.mScale_.x,
        colliderB.scale.y / 2 * transfromB.mScale_.y
    };

    glm::vec3 posDiff = colliderBCenter - colliderACenter;

    float xOverlap = halfSizeA.x + halfSizeB.x - std::abs(posDiff.x);
    float yOverlap = halfSizeA.y + halfSizeB.y - std::abs(posDiff.y);

    if (xOverlap > 0 && yOverlap > 0) {
        if (xOverlap < yOverlap) {
            float sign = (posDiff.x > 0) ? 1.0f : -1.0f;
            return glm::vec3{ -xOverlap * sign, 0.0f, 0.0f };
        } else {
            float sign = (posDiff.y > 0) ? 1.0f : -1.0f;
            return glm::vec3{ 0.0f, -yOverlap * sign, 0.0f };
        }
    } else {
        return std::nullopt;
    }
}

void PhysicsScene::render(VkCommandBuffer cmdBuffer) {
    clay::BaseScene::CameraConstant ubo{};
    ubo.view = mCamera_.getViewMatrix();
    ubo.proj = mCamera_.getProjectionMatrix();

    ((clay::AppDesktop&)mApp_).getGraphicsContextDesktop().mCameraUniform_->setData(
        &ubo,
        sizeof(CameraConstant)
    );

    mEntityManager_.render(cmdBuffer);
    renderGUI(cmdBuffer);
}

void PhysicsScene::renderGUI(VkCommandBuffer cmdBuffer) {
    mGui_.render(cmdBuffer);
}

void PhysicsScene::initialize() {}

void PhysicsScene::destroyResources() {}

clay::ecs::EntityManager& PhysicsScene::getEntityManager() {
    return mEntityManager_;
}

std::set<clay::ecs::Entity>& PhysicsScene::getEntities() {
    return mEntities_;
}

/*

todo
    check all pairs
        - if both have a collider, 
            of there is overlap then check their rigid body to decide how to resolve collision
                - overlap is determined by entity's position combined with collider
                -rigid body should also tell if the object should "bounce"

*/

} // namespace physics_scene