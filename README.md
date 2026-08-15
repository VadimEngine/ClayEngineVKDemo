# Clay Engine Demo Desktop

Demo application of the ClayEngineVK static library (https://github.com/VadimEngine/ClayEngineVK) showcasing building a Desktop application with a variety of different scenes and games. Builds and runs on Windows and Linux

![alt text](./ScreenShots/VKDesktopDemoGif2.gif)

### Requirements
- A checkout of [ClayEngineVK](https://github.com/VadimEngine/ClayEngineVK) somewhere on disk, with **its own** submodules initialized (see that repo's README) — `git submodule update --init --recursive` inside the ClayEngineVK checkout, not inside this one.

### Point this project at your ClayEngineVK checkout
`CMakePresets.json` ships two presets:
- `main` → `../ClayEngineVK` (sibling of this repo)
- `dev` → `../dev/ClayEngineVK` (a second sibling checkout, e.g. a worktree/branch used for testing engine changes)

If your ClayEngineVK checkout lives somewhere else, either:
- pass it directly on the command line: `cmake -S . -B build -DCLAY_ENGINE_DIR=/path/to/ClayEngineVK`
- or define your own preset in a local `CMakeUserPresets.json` (gitignored — not shared/committed), e.g.:
  ```json
  {
      "version": 6,
      "configurePresets": [
          {
              "name": "my-local",
              "inherits": "main",
              "cacheVariables": {
                  "CLAY_ENGINE_DIR": "C:/path/to/your/ClayEngineVK"
              }
          }
      ]
  }
  ```
  then build with the `my-local` preset instead of `main`/`dev` below.

### Compile Shaders
- `./res/compile_shader.sh` 

### Build
- `cmake --preset dev` (or `main`, or a custom preset — see above)
- `cmake --build build/dev`

### Run
- `./build/dev/Debug/ClayEngineVKDemo.exe`

### Compile Individual shader
- `C:/VulkanSDK/1.3.296.0/Bin/glslc.exe -fshader-stage=vert shader.vert -g -o vert.spv`
- `C:/VulkanSDK/1.3.296.0/Bin/glslc.exe -fshader-stage=frag shader.frag -g -o frag.spv`
