// clay
#include <clay/utils/common/Logger.h>
#include <clay/utils/desktop/UtilsDesktop.h>
// project
#include "DemoApp.h"
#include "scenes/menu_scene/MenuScene.h"
#include <PxPhysicsAPI.h>

using namespace physx;


int main() {
    // Initialize PhysX Foundation and Physics
    static PxDefaultAllocator gAllocator;
    static PxDefaultErrorCallback gErrorCallback;

    PxFoundation* foundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator
                                                    , gErrorCallback
    );
    if (!foundation) {
        LOG_E("PhysX Foundation creation failed");
        //printf("PhysX Foundation creation failed!\n");
        //return -1;
    }

    PxPhysics* physics = PxCreatePhysics(PX_PHYSICS_VERSION, *foundation
                                            , PxTolerancesScale());
    if (!physics) {
        LOG_E("PhysX Physics creation failed");
        //printf("PhysX Physics creation failed!\n");
        foundation->release();
        //return -1;
    }

    // Create a simple scene
    PxSceneDesc sceneDesc(physics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
    sceneDesc.cpuDispatcher = PxDefaultCpuDispatcherCreate(2);
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    PxScene* scene = physics->createScene(sceneDesc);
    if (!scene) {
        LOG_E("PhysX Scene creation failed");
        //printf("PhysX Scene creation failed!\n");
        physics->release();
        foundation->release();
        //return -1;
    }
    LOG_I("PhysX initialized successfully");
    //printf("PhysX initialized successfully!\n");

    // Cleanup
    scene->release();
    physics->release();
    foundation->release();

    // TODO i think these settings should be in the app
    // Set resource path and loading function
    clay::Resources::setResourcePath(DEMO_RESOURCE_PATH);
    clay::Resources::setFileLoader(clay::utils::loadFileToMemory_desktop);
    // TODO pass in name of app for window
    clay::Window window(800, 600);

    DemoApp app(window);

    try {
        app.run();
    } catch (const std::exception& e) {
        LOG_E("%s", e.what());
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}