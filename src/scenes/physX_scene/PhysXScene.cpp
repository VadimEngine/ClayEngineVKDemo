
// third party
#include <PxPhysicsAPI.h>
// clay
#include <clay/application/desktop/AppDesktop.h>
#include <clay/utils/desktop/UtilsDesktop.h>
#include <clay/utils/common/Logger.h>
// class
#include "scenes/menu_scene/MenuScene.h"
#include "scenes/physX_scene/PhysXScene.h"

namespace physX_scene {

PhysXScene::PhysXScene(clay::BaseApp& app)
    : clay::BaseScene(app),
      mEntityManager_(app.getGraphicsContext(), app.getResources()),
      mGui_(*this) {

    getFocusCamera()->setPosition({0,1,10});

    // Initialize PhysX Foundation and Physics
    static physx::PxDefaultAllocator gAllocator;
    static physx::PxDefaultErrorCallback gErrorCallback;

    mPxFoundation_ = PxCreateFoundation(
        PX_PHYSICS_VERSION, 
        gAllocator,
        gErrorCallback
    );

    if (!mPxFoundation_) {
        LOG_E("PhysX Foundation creation failed");
    }

    mPxPhysics_ = PxCreatePhysics(
        PX_PHYSICS_VERSION,
        *mPxFoundation_,
        physx::PxTolerancesScale()
    );

    // Create a simple scene
    physx::PxSceneDesc sceneDesc(mPxPhysics_->getTolerancesScale());
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
    sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;

    mPXScene_ = mPxPhysics_->createScene(sceneDesc);
    if (!mPXScene_) {
        LOG_E("PhysX Scene creation failed");
        mPxPhysics_->release();
        mPxFoundation_->release();
    }
    LOG_I("PhysX initialized successfully");

    // --- Create Ground Plane ---
    mMaterial_ = mPxPhysics_->createMaterial(0.5f, 0.5f, 0.6f);

    mGround_ = physx::PxCreatePlane(*mPxPhysics_, physx::PxPlane(0, 1, 0, 0), *mMaterial_);
    mPXScene_->addActor(*mGround_);

    // --- Create a Dynamic Box ---
    physx::PxTransform boxTransform(physx::PxVec3(0.0f, 5.0f, 0.0f));
    physx::PxBoxGeometry boxGeometry(0.5f, 0.5f, 0.5f);
    physx::PxSphereGeometry sphereGoo(0.5f);

    {
        // Ground
        clay::ecs::Entity entity = mEntityManager_.createEntity();

        mEntityManager_.addModelRenderable(
            entity, 
            { 
                mApp_.getResources().getHandle<clay::Model>("SolidPlane"),
                {.5,.5,.5,1}
            }
        );

        physx::PxTransform t = mGround_->getGlobalPose();
        glm::quat physxQuat(t.q.w, t.q.x, t.q.y, t.q.z);
        glm::quat rotX = glm::angleAxis(glm::radians(-90.0f), glm::vec3(1.0f,0.0f,0.0f));
        glm::quat finalQuat = rotX * physxQuat;

        mEntityManager_.addTransform(
            entity,
            {
                .mPosition_ = {t.p.x, t.p.y, t.p.z},
                .mOrientation_ = {finalQuat.w, finalQuat.x, finalQuat.y, finalQuat.z},
                .mScale_ = {10,10,10}
            }
        );

        mEntities_.insert(entity);
    }

    addEntity();
}

PhysXScene::~PhysXScene() {
    if (mGround_) mGround_->release();
    if (mPXScene_) mPXScene_->release();
    if (mPxPhysics_) mPxPhysics_->release();
    if (mPxFoundation_) mPxFoundation_->release();
}

void PhysXScene::update(const float dt) {
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


    mPXScene_->simulate(dt);
    mPXScene_->fetchResults(true);

    mEntityManager_.update(dt);
}

void PhysXScene::render(vk::CommandBuffer cmdBuffer) {
    {
        clay::BaseScene::CameraConstant ubo{};
        ubo.view = mCamera_.getViewMatrix();
        ubo.proj = mCamera_.getProjectionMatrix();
        ((clay::AppDesktop&)mApp_).getGraphicsContextDesktop().mCameraUniform_->setData(
            &ubo,
            sizeof(CameraConstant)
        );
    }
    {
        clay::BaseScene::CameraConstant ubo{};
        glm::mat4 view = mCamera_.getViewMatrix();
        ubo.view = glm::mat4(glm::mat3(view)); 
        ubo.proj = mCamera_.getProjectionMatrix();
        ((clay::AppDesktop&)mApp_).getGraphicsContextDesktop().mCameraUniformHeadLocked_->setData(
            &ubo,
            sizeof(CameraConstant)
        );
    }

    mEntityManager_.render(cmdBuffer);
    renderGUI(cmdBuffer);
}

void PhysXScene::renderGUI(vk::CommandBuffer cmdBuffer) {
    mGui_.render(cmdBuffer);
}

void PhysXScene::addEntity() {
    // --- Create a Dynamic Box ---
    physx::PxTransform boxTransform(physx::PxVec3(0.0f, 5.0f, 0.0f));
    physx::PxBoxGeometry boxGeometry(0.5f, 0.5f, 0.5f);

    auto* rigidBody = physx::PxCreateDynamic(
        *mPxPhysics_,
        boxTransform,
        boxGeometry,
        *mMaterial_,
        1.0f
    );

    rigidBody->setAngularDamping(0.5f);
    mPXScene_->addActor(*rigidBody);
    clay::ecs::Entity entity = mEntityManager_.createEntity();

    mEntityManager_.addModelRenderable(
        entity, 
        { 
            mApp_.getResources().getHandle<clay::Model>("Cube"),
            {1,1,1,1}
        }
    );
    
    physx::PxTransform t = rigidBody->getGlobalPose();

    mEntityManager_.addTransform(
        entity, 
        {
            .mPosition_ = {t.p.x, t.p.y, t.p.z},
            .mOrientation_ = { t.q.w, t.q.x, t.q.y, t.q.z},
            .mScale_ {1,1,1}
        }
    );

    mEntityManager_.addRigidBody(
        entity,
        {
            rigidBody
        }
    );

    mEntities_.insert(entity);
}


void PhysXScene::initialize() {}

void PhysXScene::destroyResources() {}

} // namespace physX_scene