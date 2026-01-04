#pragma once
// standard lib
#include <map>
#include <vector>
// clay
#include <clay/gui/desktop/ImGuiComponentDesktop.h>

namespace shader_tool_scene {
// Forward declare Scene
class ShaderToolScene;

enum class SystemUniform {
    NONE = 0,
    CAMERA_VIEW,
    CAMERA_PROJ,
    MODEL,
    TIME_NS,
    FRAME,
    SCREEN_SIZE
};

struct UniformGroup {
    char label[256];
    char dataStr[256];
    std::vector<float> values;
    bool enabled;
    int binding;
    SystemUniform systemUniform;
};

struct TextureUniform {
    char label[256];
    int textureId;
    unsigned int binding;
    bool enabled;
};

class ShaderToolSceneGUI: public clay::ImGuiComponentDesktop {
public:
    /**
     * Constructor
     * @param theScene The scene this gui is for
     */
    ShaderToolSceneGUI(ShaderToolScene& theScene);

    /** Destructor */
    ~ShaderToolSceneGUI();

    /** Build the GUI ImGui components every frame */
    void render(vk::CommandBuffer cmdBuffer);

    void compileShader();

public:
    /** Build the section to list/control the camera of this scene */
    void buildCameraSection();

    void buildShaderSection();

    void buildModelTransformSection();

    void saveShaders(const std::string& shaderName);

    std::vector<float> parseFloatString(const std::string& input);

    /** The Scene this GUI is for */
    ShaderToolScene& mScene_;
    /** If the GUI is set to enable VSync */
    bool mVSyncEnabled_;
    /** The Camera Mode this GUI applies*/
    int mCameraMode_;

    char vertexBuffer[1024 * 16] = 
R"(#version 450

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoords;

layout(push_constant) uniform PushConstants {
    mat4 uProjection;
    mat4 uView;
    mat4 uModel;
} pc;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = pc.uProjection * pc.uView * pc.uModel * vec4(aPos, 1.0);
    fragNormal = aNormal;
    fragTexCoord = aTexCoords;
})";

    char fragmentBuffer[1024 * 16] = 
R"(#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    outColor = vec4(1.0, 1.0, 1.0, 1.0);
}
)";

    char geometryBuffer[1024 * 16] = {};
    bool geometryEnabled = false;

    // ordered to match enum class SystemUniform  
    const std::vector<std::string> mSystemValues_ = {
        "None", 
        "CameraView", 
        "CameraProj", 
        "Model", 
        "Time", 
        "Frame", 
        "ScreenSize"
    };

    int mSelectedMeshIndex_ = 0;

    std::vector<UniformGroup> mUniforms_;

    std::vector<TextureUniform> mTextureUniforms_;

    bool mHDREnabled_ = false;

    std::vector<std::pair<std::string, clay::Mesh*>> mMeshList_;

    std::vector<const char*> meshLabels_;

    // ordered to make iterating deterministic
    std::map<unsigned int, std::string> mTextureNamesById_;

    glm::mat4 mTransformMat_ = glm::identity<glm::mat4>();

    std::string mCompileStatus_ = "Ready";
    bool mCompileError_ = false;
};

} // namespace shader_tool_scene
