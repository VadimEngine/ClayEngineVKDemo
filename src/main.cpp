// projet
#include "DemoApp.h"
#include "scenes/menu_scene/MenuScene.h"

int main() {
    // Set resource path and loading function
    clay::Resources::setResourcePath(DEMO_RESOURCE_PATH);
    clay::Resources::setFileLoader(clay::utils::loadFileToMemory_desktop);

    DemoApp app;
    // create scene and pass it to app
    auto theScene = new MenuScene(app);
    app.setScene(theScene); 

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}