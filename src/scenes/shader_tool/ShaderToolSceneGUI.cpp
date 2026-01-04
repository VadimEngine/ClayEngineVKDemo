// standard lib
#include <fstream>
#include <iostream>
#include <sstream>
#include <filesystem>
// clay
#include <clay/application/desktop/AppDesktop.h>
#include <clay/utils/common/Logger.h>
// third party
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
// project
#include "scenes/shader_tool/ShaderToolScene.h"
#include "scenes/menu_scene/MenuScene.h"
#include "DemoApp.h"
// class
#include "scenes/shader_tool/ShaderToolSceneGUI.h"

namespace shader_tool_scene {

ShaderToolSceneGUI::ShaderToolSceneGUI(ShaderToolScene& theScene)
    : mScene_(theScene) {
    mVSyncEnabled_ = ((clay::AppDesktop&)(mScene_.getApp())).tempVSyncValue;
    mCameraMode_ = static_cast<int>(mScene_.getFocusCamera()->getMode());

    // Setup available meshes
    mMeshList_.emplace_back("Sphere", &mScene_.getApp().getResources()[((DemoApp&)mScene_.getApp()).mAppProp.mMeshes.sphere]);
    mMeshList_.emplace_back("Cube", &mScene_.getApp().getResources()[((DemoApp&)mScene_.getApp()).mAppProp.mMeshes.cube]);
    mMeshList_.emplace_back("Plane", &mScene_.getApp().getResources()[((DemoApp&)mScene_.getApp()).mAppProp.mMeshes.plane]);

    for (const auto& eachMesh : mMeshList_) {
        meshLabels_.push_back(eachMesh.first.c_str());
    }

    // Initialize with basic uniforms for push constants
    mUniforms_ = {
        {
            .label="uProjection",
            .dataStr="",
            .values={},
            .enabled=true,
            .binding=0,
            .systemUniform=SystemUniform::CAMERA_PROJ
        },
        {
            .label="uView",
            .dataStr="",
            .values={},
            .enabled=true,
            .binding=0,
            .systemUniform=SystemUniform::CAMERA_VIEW
        },
        {
            .label="uModel",
            .dataStr="",
            .values={},
            .enabled=true,
            .binding=0,
            .systemUniform=SystemUniform::MODEL
        }
    };
}

ShaderToolSceneGUI::~ShaderToolSceneGUI() {}

void ShaderToolSceneGUI::render(vk::CommandBuffer cmdBuffer) {
    clay::ImGuiComponentDesktop::beginRender();

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(400, 600), ImGuiCond_FirstUseEver);
    ImGui::Begin("Shader Tool");
    {
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.f, 0.f, 0.f, 0.40f));
        if (ImGui::Button("Back")) {
            ((clay::AppDesktop&)mScene_.getApp()).setScene(new MenuScene(mScene_.getApp()));
        }
        ImGui::PopStyleColor();
        
        ImGui::Separator();
        ImGui::Text("FPS: %.1f", double(ImGui::GetIO().Framerate));
        
        if (ImGui::Checkbox("VSync", &mVSyncEnabled_)) {
            ((clay::AppDesktop&)mScene_.getApp()).tempVSyncFlag = true;
            ((clay::AppDesktop&)mScene_.getApp()).tempVSyncValue = mVSyncEnabled_;
        }
        
        ImGui::Separator();
        buildCameraSection();
        ImGui::Separator();
        buildModelTransformSection();
        ImGui::Separator();
        buildShaderSection();
    }
    ImGui::End();

    clay::ImGuiComponentDesktop::endRender(cmdBuffer);
}

void ShaderToolSceneGUI::buildCameraSection() {
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Camera Mode");
        
        if (ImGui::RadioButton("Perspective", &mCameraMode_, 0)) {
            mScene_.getFocusCamera()->setMode(static_cast<clay::Camera::Mode>(mCameraMode_));
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Orthographic", &mCameraMode_, 1)) {
            mScene_.getFocusCamera()->setMode(static_cast<clay::Camera::Mode>(mCameraMode_));
        }

        glm::vec3 camPosition = mScene_.getFocusCamera()->getPosition();
        ImGui::Text("Position: %.2f, %.2f, %.2f", camPosition.x, camPosition.y, camPosition.z);
        
        glm::vec3 camForward = mScene_.getFocusCamera()->getForward();
        ImGui::Text("Direction: %.2f, %.2f, %.2f", camForward.x, camForward.y, camForward.z);
        
        ImGui::Text("FOV: %.2f", mScene_.getFocusCamera()->getFOV());
    }
}

void ShaderToolSceneGUI::buildModelTransformSection() {
    if (ImGui::CollapsingHeader("Model Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
        // Decompose transformation matrix
        glm::vec3 position, scale, skew;
        glm::vec4 perspective;
        glm::quat rotationQuat;
        
        glm::decompose(mTransformMat_, scale, rotationQuat, position, skew, perspective);
        
        // Convert quaternion to Euler angles
        glm::vec3 rotation = glm::eulerAngles(rotationQuat); 
        rotation = glm::degrees(rotation);

        float positionArr[3] = { position.x, position.y, position.z };
        float rotationArr[3] = { rotation.x, rotation.y, rotation.z };
        float scaleArr[3] = { scale.x, scale.y, scale.z };

        if (ImGui::SliderFloat3("Position", positionArr, -10.f, 10.f, "%.2f")) {
            mTransformMat_[3] = glm::vec4(positionArr[0], positionArr[1], positionArr[2], 1.0f);
            mScene_.setTransformMat(mTransformMat_);
        }

        if (ImGui::SliderFloat3("Rotation", rotationArr, -180.f, 180.f, "%.2f")) {
            glm::vec3 newRotation = glm::radians(glm::vec3(rotationArr[0], rotationArr[1], rotationArr[2]));
            glm::mat4 rotationMat = glm::eulerAngleXYZ(newRotation.x, newRotation.y, newRotation.z);
            glm::vec3 prevPosition(mTransformMat_[3]);
            glm::mat4 scaleMat = glm::scale(glm::mat4(1.0f), scale);
            mTransformMat_ = glm::translate(glm::mat4(1.0f), prevPosition) * rotationMat * scaleMat;
            mScene_.setTransformMat(mTransformMat_);
        }

        if (ImGui::SliderFloat3("Scale", scaleArr, 0.1f, 10.f, "%.2f")) {
            glm::vec3 prevPosition(mTransformMat_[3]);
            glm::mat4 rotationMat = glm::mat4_cast(rotationQuat);
            mTransformMat_ = glm::translate(glm::mat4(1.0f), prevPosition) * rotationMat * 
                            glm::scale(glm::mat4(1.0f), glm::vec3(scaleArr[0], scaleArr[1], scaleArr[2]));
            mScene_.setTransformMat(mTransformMat_);
        }
    }
}

void ShaderToolSceneGUI::buildShaderSection() {
    if (ImGui::CollapsingHeader("Shader Editor", ImGuiTreeNodeFlags_DefaultOpen)) {
        static ImGuiInputTextFlags flags = ImGuiInputTextFlags_AllowTabInput;

        // Mesh selection
        ImGui::Text("Mesh:");
        ImGui::SameLine();
        if (ImGui::Combo("##MeshSelect", &mSelectedMeshIndex_, meshLabels_.data(), 
                        static_cast<int>(meshLabels_.size()))) {
            mScene_.setMesh(mMeshList_[mSelectedMeshIndex_].second);
        }

        ImGui::Separator();

        // Vertex Shader
        if (ImGui::CollapsingHeader("Vertex Shader", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::InputTextMultiline("##VertexEditor", vertexBuffer, sizeof(vertexBuffer), 
                                     ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
        }

        // Fragment Shader
        if (ImGui::CollapsingHeader("Fragment Shader", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::InputTextMultiline("##FragmentEditor", fragmentBuffer, sizeof(fragmentBuffer), 
                                     ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 16), flags);
        }

        // Geometry Shader (optional)
        if (ImGui::CollapsingHeader("Geometry Shader (Optional)")) {
            ImGui::Checkbox("Enable Geometry Shader", &geometryEnabled);
            if (geometryEnabled) {
                ImGui::InputTextMultiline("##GeometryEditor", geometryBuffer, sizeof(geometryBuffer), 
                                         ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * 12), flags);
            }
        }

        ImGui::Separator();

        // Compile button
        if (ImGui::Button("Compile Shaders", ImVec2(150, 30))) {
            compileShader();
        }
        
        ImGui::SameLine();
        
        // Status indicator
        if (mCompileError_) {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.0f, 0.0f, 1.0f));
        } else {
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 1.0f, 0.0f, 1.0f));
        }
        ImGui::Text("Status: %s", mCompileStatus_.c_str());
        ImGui::PopStyleColor();

        ImGui::Separator();

        // Save shaders
        static char saveNameBuffer[256] = "MyShader";
        ImGui::Text("Save Shaders:");
        ImGui::SameLine();
        ImGui::InputText("##SaveName", saveNameBuffer, sizeof(saveNameBuffer));
        ImGui::SameLine();
        if (ImGui::Button("Save##SaveShaders")) {
            if (strlen(saveNameBuffer) > 0) {
                saveShaders(saveNameBuffer);
                mCompileStatus_ = "Shaders saved!";
                mCompileError_ = false;
            }
        }
    }

    // Uniforms section (for future expansion with descriptor sets)
    if (ImGui::CollapsingHeader("Uniforms (Advanced)")) {
        ImGui::Text("Push Constants are automatically handled.");
        ImGui::Text("Support for descriptor set uniforms coming soon!");
        
        // Show current uniforms
        for (const auto& uniform : mUniforms_) {
            ImGui::Text("  %s: %s", 
                       uniform.label, 
                       mSystemValues_[static_cast<int>(uniform.systemUniform)].c_str());
        }
    }
}

void ShaderToolSceneGUI::compileShader() {
    try {
        mScene_.compileAndSetShader(
            vertexBuffer, 
            fragmentBuffer, 
            geometryEnabled ? geometryBuffer : "", 
            geometryEnabled
        );
        mCompileStatus_ = "Compilation successful!";
        mCompileError_ = false;
    } catch (const std::exception& e) {
        mCompileStatus_ = std::string("Error: ") + e.what();
        mCompileError_ = true;
        LOG_E("Shader compilation failed: %s", e.what());
    }
}

void ShaderToolSceneGUI::saveShaders(const std::string& shaderName) {
    auto save = [](const std::string& filename, const char* buffer) {
        std::ofstream file(filename, std::ios::binary);
        if (!file) {
            std::cerr << "Error: Unable to open file for writing: " << filename << std::endl;
            return;
        }

        size_t length = std::strlen(buffer);
        file.write(buffer, length);
        file.close();

        if (!file) {
            std::cerr << "Error: Failed to write to file: " << filename << std::endl;
        } else {
            LOG_I("Saved shader: %s", filename.c_str());
        }
    };

    // Save shaders to res/shaders directory
    std::string basePath = "res/shaders/";
    save(basePath + shaderName + ".vert.glsl", vertexBuffer);
    save(basePath + shaderName + ".frag.glsl", fragmentBuffer);
    
    if (geometryEnabled) {
        save(basePath + shaderName + ".geom.glsl", geometryBuffer);
    }
}

std::vector<float> ShaderToolSceneGUI::parseFloatString(const std::string& input) {
    std::vector<float> result;
    std::istringstream iss(input);
    float value;
    std::string token;
    
    // Validate
    std::istringstream validateStream(input);
    while (validateStream >> token) {
        try {
            std::stof(token);
        } catch (const std::exception&) {
            return {}; // Return empty if conversion fails
        }
    }

    // Parse values
    while (iss >> value) {
        result.push_back(value);
    }

    return result;
}

} // namespace shader_tool_scene
