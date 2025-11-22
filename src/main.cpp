// clay
#include <clay/utils/common/Logger.h>
#include <clay/utils/desktop/UtilsDesktop.h>
// project
#include "DemoApp.h"
#include "scenes/menu_scene/MenuScene.h"

int main() {
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