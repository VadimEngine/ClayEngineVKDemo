# Clay Engine Demo Desktop
Clay Engine with Vulkan demo application for desktop platform

# Load all submodules
- `git submodule update --init --recursive`

### Build
- `cmake -S . -B build`
- `cmake --build ./build/`

### Run
- `./build/Debug/Sandbox.exe`

### Compile shader
- `C:/VulkanSDK/1.3.296.0/Bin/glslc.exe -fshader-stage=vert shader.vert -g -o vert.spv`
- `C:/VulkanSDK/1.3.296.0/Bin/glslc.exe -fshader-stage=frag shader.frag -g -o frag.spv`
