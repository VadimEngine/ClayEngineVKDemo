// standard lib
#include <fstream>
#include <sstream>
#include <array>
#include <cstring>
// clay
#include <clay/application/desktop/AppDesktop.h>
#include <clay/utils/common/Logger.h>
// third party
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
// class
#include "scenes/shader_tool/ShaderToolScene.h"
#include "scenes/menu_scene/MenuScene.h"

namespace shader_tool_scene {

ShaderToolScene::ShaderToolScene(clay::BaseApp& app)
    : clay::BaseScene(app),
      mGui_(*this) {
    
    mCamera_.setPosition({0, 2, 5});
    const auto [frameWidth, frameHeight] = app.getGraphicsContext().getFrameDimensions();
    mCamera_.setAspectRatio(static_cast<float>(frameWidth) / static_cast<float>(frameHeight));
    
    // Set default mesh in constructor (GUI is initialized by now)
    if (!mGui_.mMeshList_.empty()) {
        mCurrentMesh_ = mGui_.mMeshList_[0].second;
    }
}

ShaderToolScene::~ShaderToolScene() {
    destroyResources();
}

void ShaderToolScene::initialize() {
    // Try to compile initial shaders, but don't block if it fails
    // User can manually compile from GUI
    try {
        compileAndSetShader(mGui_.vertexBuffer, mGui_.fragmentBuffer, "", false);
        mGui_.mCompileStatus_ = "Initial shaders compiled";
        mGui_.mCompileError_ = false;
        LOG_I("Initial shader compilation succeeded");
    } catch (const std::exception& e) {
        LOG_E("Failed to compile initial shaders: %s", e.what());
        mGui_.mCompileStatus_ = std::string("Click 'Compile Shaders' to start");
        mGui_.mCompileError_ = true;
        mPipelineValid_ = false; // Ensure pipeline is marked invalid
    }
}

void ShaderToolScene::update(float dt) {
    // Camera updates handled by BaseScene if needed
}

void ShaderToolScene::render(vk::CommandBuffer cmdBuffer) {
    // Only render 3D scene if pipeline is valid
    if (!mPipelineValid_ || !mCurrentMesh_ || !mMaterial_) {
        return;
    }

    // Prepare push constants
    PushConstants pushConstants;
    pushConstants.projection = mCamera_.getProjectionMatrix();
    pushConstants.view = mCamera_.getViewMatrix();
    pushConstants.model = mTransformMat_;

    // Bind pipeline and material
    cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mMaterial_->getPipeline());
    
    // Push constants
    mMaterial_->pushConstants(
        cmdBuffer, 
        &pushConstants, 
        sizeof(PushConstants), 
        vk::ShaderStageFlagBits::eVertex
    );

    // Bind material (descriptor sets if any)
    mMaterial_->bindMaterial(cmdBuffer);

    // Bind and draw mesh
    mCurrentMesh_->bindMesh(cmdBuffer);
    cmdBuffer.drawIndexed(mCurrentMesh_->getIndicesCount(), 1, 0, 0, 0);

    renderGUI(cmdBuffer);
}

void ShaderToolScene::renderGUI(vk::CommandBuffer cmdBuffer) {
    mGui_.render(cmdBuffer);
}

void ShaderToolScene::destroyResources() {
    destroyPipelineResources();
}

void ShaderToolScene::compileAndSetShader(
    const std::string& vertexSource, 
    const std::string& fragmentSource,
    const std::string& geometrySource,
    bool useGeometry) {
    
    // Destroy old pipeline resources
    destroyPipelineResources();
    
    // Compile GLSL to SPIR-V
    if (!compileGLSLToSPIRV(vertexSource, "vertex", mCurrentVertexSPIRV_)) {
        throw std::runtime_error("Failed to compile vertex shader");
    }
    
    if (!compileGLSLToSPIRV(fragmentSource, "fragment", mCurrentFragmentSPIRV_)) {
        throw std::runtime_error("Failed to compile fragment shader");
    }
    
    if (useGeometry && !geometrySource.empty()) {
        if (!compileGLSLToSPIRV(geometrySource, "geometry", mCurrentGeometrySPIRV_)) {
            throw std::runtime_error("Failed to compile geometry shader");
        }
    }
    
    mUseGeometryShader_ = useGeometry && !geometrySource.empty();
    
    // Create new shader modules and pipeline
    createPipelineAndMaterial();
    
    mPipelineValid_ = true;
}

bool ShaderToolScene::compileGLSLToSPIRV(
    const std::string& source, 
    const std::string& stageName, 
    std::vector<uint32_t>& spirv) {
    
    // Create temporary files for glslangValidator
    std::string tempGLSL = "temp_shader_" + stageName + ".glsl";
    std::string tempSPV = "temp_shader_" + stageName + ".spv";
    
    // Write GLSL source to file
    {
        std::ofstream glslFile(tempGLSL);
        if (!glslFile) {
            LOG_E("Failed to create temp GLSL file: %s", tempGLSL.c_str());
            return false;
        }
        glslFile << source;
    }
    
    // Determine stage flag for glslc
    std::string stageArg;
    if (stageName == "vertex") {
        stageArg = "vert";
    } else if (stageName == "fragment") {
        stageArg = "frag";
    } else if (stageName == "geometry") {
        stageArg = "geom";
    }
    
    // Compile using glslc (from Vulkan SDK)
    std::string command = "glslc -fshader-stage=" + stageArg + " " + tempGLSL + " -o " + tempSPV;
    int result = system(command.c_str());
    
    if (result != 0) {
        LOG_E("glslc failed for %s shader", stageName.c_str());
        // Clean up temp files
        std::remove(tempGLSL.c_str());
        return false;
    }
    
    // Read SPIR-V binary
    std::ifstream spvFile(tempSPV, std::ios::binary | std::ios::ate);
    if (!spvFile) {
        LOG_E("Failed to open compiled SPIR-V file: %s", tempSPV.c_str());
        std::remove(tempGLSL.c_str());
        return false;
    }
    
    size_t fileSize = spvFile.tellg();
    spvFile.seekg(0);
    
    spirv.resize(fileSize / sizeof(uint32_t));
    spvFile.read(reinterpret_cast<char*>(spirv.data()), fileSize);
    spvFile.close();
    
    // Clean up temp files
    std::remove(tempGLSL.c_str());
    std::remove(tempSPV.c_str());
    
    LOG_I("Successfully compiled %s shader (%zu bytes)", stageName.c_str(), fileSize);
    return true;
}

void ShaderToolScene::createPipelineAndMaterial() {
    auto& gContext = mApp_.getGraphicsContext();
    
    // Create shader modules
    clay::utils::FileData vertexData;
    vertexData.data = std::make_unique<uint8_t[]>(mCurrentVertexSPIRV_.size() * sizeof(uint32_t));
    std::memcpy(vertexData.data.get(), mCurrentVertexSPIRV_.data(), mCurrentVertexSPIRV_.size() * sizeof(uint32_t));
    vertexData.size = mCurrentVertexSPIRV_.size() * sizeof(uint32_t);
    
    mVertexShader_ = std::make_unique<clay::ShaderModule>(
        gContext.getDevice(),
        vk::ShaderStageFlagBits::eVertex,
        vertexData
    );
    
    clay::utils::FileData fragmentData;
    fragmentData.data = std::make_unique<uint8_t[]>(mCurrentFragmentSPIRV_.size() * sizeof(uint32_t));
    std::memcpy(fragmentData.data.get(), mCurrentFragmentSPIRV_.data(), mCurrentFragmentSPIRV_.size() * sizeof(uint32_t));
    fragmentData.size = mCurrentFragmentSPIRV_.size() * sizeof(uint32_t);
    
    mFragmentShader_ = std::make_unique<clay::ShaderModule>(
        gContext.getDevice(),
        vk::ShaderStageFlagBits::eFragment,
        fragmentData
    );
    
    if (mUseGeometryShader_) {
        clay::utils::FileData geometryData;
        geometryData.data = std::make_unique<uint8_t[]>(mCurrentGeometrySPIRV_.size() * sizeof(uint32_t));
        std::memcpy(geometryData.data.get(), mCurrentGeometrySPIRV_.data(), mCurrentGeometrySPIRV_.size() * sizeof(uint32_t));
        geometryData.size = mCurrentGeometrySPIRV_.size() * sizeof(uint32_t);
        
        mGeometryShader_ = std::make_unique<clay::ShaderModule>(
            gContext.getDevice(),
            vk::ShaderStageFlagBits::eGeometry,
            geometryData
        );
    }
    
    // Create pipeline resource
    clay::PipelineResource::PipelineConfig pipelineConfig{
        .graphicsContext = gContext
    };
    
    // Setup shaders
    pipelineConfig.pipelineLayoutInfo.shaders = {
        mVertexShader_.get(),
        mFragmentShader_.get()
    };
    
    if (mUseGeometryShader_) {
        pipelineConfig.pipelineLayoutInfo.shaders.push_back(mGeometryShader_.get());
    }
    
    // Setup vertex input
    auto vertexAttribs = clay::Mesh::Vertex::getAttributeDescriptions();
    pipelineConfig.pipelineLayoutInfo.attributeDescriptions = {vertexAttribs.begin(), vertexAttribs.end()};
    pipelineConfig.pipelineLayoutInfo.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();
    
    // Setup depth stencil
    pipelineConfig.pipelineLayoutInfo.depthStencilState = {
        .depthTestEnable = VK_TRUE,
        .depthWriteEnable = VK_TRUE,
        .depthCompareOp = vk::CompareOp::eLessOrEqual,
        .depthBoundsTestEnable = VK_FALSE,
        .stencilTestEnable = VK_FALSE
    };
    
    // Setup rasterizer
    pipelineConfig.pipelineLayoutInfo.rasterizerState = {
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode = vk::CullModeFlagBits::eBack,
        .frontFace = vk::FrontFace::eCounterClockwise,
        .depthBiasEnable = VK_FALSE,
        .lineWidth = 1.0f
    };
    
    // Setup push constants for MVP matrices
    vk::PushConstantRange pushConstantRange{
        .stageFlags = vk::ShaderStageFlagBits::eVertex,
        .offset = 0,
        .size = sizeof(PushConstants)
    };
    pipelineConfig.pipelineLayoutInfo.pushConstants = {pushConstantRange};
    
    // Create pipeline
    mPipelineResource_ = std::make_unique<clay::PipelineResource>(pipelineConfig);
    
    // Create material (currently with no additional descriptor sets)
    clay::Material::MaterialConfig materialConfig{
        .graphicsContext = gContext,
        .pipelineResource = *mPipelineResource_,
        .bufferBindings = {},
        .imageBindings = {},
        .imageArrayBindings = {}
    };
    
    mMaterial_ = std::make_unique<clay::Material>(materialConfig);
}

void ShaderToolScene::destroyPipelineResources() {
    mMaterial_.reset();
    mPipelineResource_.reset();
    mVertexShader_.reset();
    mFragmentShader_.reset();
    mGeometryShader_.reset();
    mPipelineValid_ = false;
}

void ShaderToolScene::setMesh(clay::Mesh* newMesh) {
    mCurrentMesh_ = newMesh;
}

void ShaderToolScene::setUniforms(
    const std::vector<UniformGroup>& uniforms, 
    const std::vector<TextureUniform>& textureUniforms) {
    mCurrentUniforms_ = uniforms;
    mCurrentTextureUniforms_ = textureUniforms;
    // TODO: Implement descriptor set updates for custom uniforms
}

void ShaderToolScene::setTransformMat(const glm::mat4& newTransform) {
    mTransformMat_ = newTransform;
}

} // namespace shader_tool_scene
