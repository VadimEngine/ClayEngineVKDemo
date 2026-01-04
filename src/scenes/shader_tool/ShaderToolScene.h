#pragma once
// standard lib
#include <memory>
#include <vector>
// clay
#include <clay/application/common/BaseScene.h>
#include <clay/graphics/common/ShaderModule.h>
#include <clay/graphics/common/PipelineResource.h>
#include <clay/graphics/common/Material.h>
#include <clay/graphics/common/Mesh.h>
// project
#include "scenes/shader_tool/ShaderToolSceneGUI.h"

namespace shader_tool_scene {

class ShaderToolScene : public clay::BaseScene {
public:
    /**
     * Constructor
     * @param app Parent app handling this Scene
     */
    ShaderToolScene(clay::BaseApp& app);

    /** Destructor */
    ~ShaderToolScene();

    /**
     * Update the Scene
     * @param dt Time since last update in seconds
     */
    void update(const float dt) override;

    /**
     * Render this scene
     * @param cmdBuffer Command buffer for Vulkan rendering
     */
    void render(vk::CommandBuffer cmdBuffer) override;

    /**
     * Render just the gui for this scene
     */
    void renderGUI(vk::CommandBuffer cmdBuffer) override;

    /**
     * @brief Loads and builds resources that are used for this scene
     */
    void initialize() override;

    /**
     * @brief Cleanup resources
     */
    void destroyResources() override;

    // Methods called from GUI
    void compileAndSetShader(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geometrySource, bool useGeometry);

    void setMesh(clay::Mesh* newMesh);

    void setUniforms(const std::vector<UniformGroup>& uniforms, const std::vector<TextureUniform>& textureUniforms);

    void setTransformMat(const glm::mat4& newTransform);

private:
    struct PushConstants {
        glm::mat4 projection;
        glm::mat4 view;
        glm::mat4 model;
    };

    bool compileGLSLToSPIRV(const std::string& source, const std::string& stageName, std::vector<uint32_t>& spirv);

    void createPipelineAndMaterial();

    void destroyPipelineResources();

    /** GUI for this Scene*/
    ShaderToolSceneGUI mGui_;

    std::vector<UniformGroup> mCurrentUniforms_;
    std::vector<TextureUniform> mCurrentTextureUniforms_;

    clay::Mesh* mCurrentMesh_ = nullptr;
    glm::mat4 mTransformMat_ = glm::identity<glm::mat4>();

    // Shader and pipeline resources
    std::unique_ptr<clay::ShaderModule> mVertexShader_;
    std::unique_ptr<clay::ShaderModule> mFragmentShader_;
    std::unique_ptr<clay::ShaderModule> mGeometryShader_;
    
    std::unique_ptr<clay::PipelineResource> mPipelineResource_;
    std::unique_ptr<clay::Material> mMaterial_;

    // Cached SPIR-V for current shaders
    std::vector<uint32_t> mCurrentVertexSPIRV_;
    std::vector<uint32_t> mCurrentFragmentSPIRV_;
    std::vector<uint32_t> mCurrentGeometrySPIRV_;
    bool mUseGeometryShader_ = false;

    bool mPipelineValid_ = false;
};

} // namespace shader_tool_scene
