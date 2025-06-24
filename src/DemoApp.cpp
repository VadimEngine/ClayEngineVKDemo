// clay
#include <clay/utils/desktop/UtilsDesktop.h>
#include <clay/graphics/common/ShaderModule.h>
// project
#include "scenes/menu_scene/MenuScene.h"
// class
#include "DemoApp.h"

DemoApp::DemoApp(clay::Window& window) 
    : clay::AppDesktop(window) {
    loadResources();
    setScene(new MenuScene(*this));
}

DemoApp::~DemoApp() {}

// TODO backface culling to pipelines
void DemoApp::loadResources() {
    // Texture
    clay::ShaderModule textureVertShader(
        mpGraphicsContext_->getDevice(),
        VK_SHADER_STAGE_VERTEX_BIT,
        clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "shaders/Texture.vert.spv").string()
        )
    );
    clay::ShaderModule textureFragShader(
        mpGraphicsContext_->getDevice(),
        VK_SHADER_STAGE_FRAGMENT_BIT,
        clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "shaders/Texture.frag.spv").string()
        )
    );
    // sprite
    clay::ShaderModule spriteVertShader(
        mpGraphicsContext_->getDevice(),
        VK_SHADER_STAGE_VERTEX_BIT,
        clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "shaders/Sprite.vert.spv").string()
        )
    );
    clay::ShaderModule spriteFragShader(
        mpGraphicsContext_->getDevice(),
        VK_SHADER_STAGE_FRAGMENT_BIT,
        clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "shaders/Sprite.frag.spv").string()
        )
    );

    // Font
    {
        // load font
        auto fontData = clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "fonts/runescape_uf.ttf").string()
        );

        clay::ShaderModule fontVertShader(
            mpGraphicsContext_->getDevice(),
            VK_SHADER_STAGE_VERTEX_BIT,
            clay::utils::loadFileToMemory_desktop(
                (clay::Resources::getResourcePath() / "shaders/Text.vert.spv").string()
            )
        );
        clay::ShaderModule fontFragShader(
            mpGraphicsContext_->getDevice(),
            VK_SHADER_STAGE_FRAGMENT_BIT,
            clay::utils::loadFileToMemory_desktop(
                (clay::Resources::getResourcePath() / "shaders/Text.frag.spv").string()
            )
        );

        mResources_.addResource<clay::Font>(
            std::make_unique<clay::Font>(*mpGraphicsContext_, fontData, fontVertShader, fontFragShader, *mGraphicsContextDesktop_.mCameraUniform_),
            "Runescape"
        );
    }

    // Sampler
    {
        // default sampler
        VkSampler sampler;

        VkSamplerCreateInfo samplerInfo{};
        samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        samplerInfo.magFilter = VK_FILTER_NEAREST;
        samplerInfo.minFilter = VK_FILTER_NEAREST;
        samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        samplerInfo.anisotropyEnable = VK_FALSE;
        samplerInfo.maxAnisotropy = 1;
        samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
        samplerInfo.unnormalizedCoordinates = VK_FALSE;
        samplerInfo.compareEnable = VK_FALSE;
        samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
        samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
        samplerInfo.minLod = 0.0f;
        samplerInfo.maxLod = 0.0f;
        samplerInfo.mipLodBias = 0.0f;

        if (vkCreateSampler(mpGraphicsContext_->getDevice(), &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
            throw std::runtime_error("failed to create texture sampler!");
        }
        mResources_.addResource<VkSampler>(
            std::make_unique<VkSampler>(sampler),
            "Default"
        );
    }
    // Meshes
    // Sphere
    mResources_.loadResource<clay::Mesh>({(clay::Resources::getResourcePath() / "models/Sphere.obj").string()}, "Sphere");
    // Plane
    mResources_.loadResource<clay::Mesh>({(clay::Resources::getResourcePath() / "models/Plane.obj").string()}, "Plane");
    // Cube
    mResources_.loadResource<clay::Mesh>({(clay::Resources::getResourcePath() / "models/Cube.obj").string()}, "Cube");
    // Torus
    mResources_.loadResource<clay::Mesh>({(clay::Resources::getResourcePath() / "models/Torus.obj").string()}, "Torus");
    // images
    {
        clay::utils::ImageData imageData = clay::utils::loadImageFileToMemory_desktop(clay::Resources::getResourcePath() / "textures/V.png");

        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(imageData);
        pVTexture->setSampler(*mResources_.getResource<VkSampler>("Default"));

        mResources_.addResource(std::unique_ptr<clay::Texture>(pVTexture), "VTexture");
    }
    {
        clay::utils::ImageData imageData = clay::utils::loadImageFileToMemory_desktop(clay::Resources::getResourcePath() / "textures/Sprites.png");

        auto* pVTexture = new clay::Texture(*mpGraphicsContext_);
        pVTexture->initialize(imageData);
        pVTexture->setSampler(*mResources_.getResource<VkSampler>("Default"));

        mResources_.addResource(std::unique_ptr<clay::Texture>(pVTexture), "SpriteSheet");
    }
    {
        // solid image
        clay::utils::ImageData singleRGBA;
        singleRGBA.width = 1;
        singleRGBA.height = 1;
        singleRGBA.channels = 4; // RGBA
        singleRGBA.pixels = std::make_unique<uint8_t[]>(4);

        singleRGBA.pixels[0] = 255;
        singleRGBA.pixels[1] = 255;
        singleRGBA.pixels[2] = 255;
        singleRGBA.pixels[3] = 255;

        auto* pSolidTexture = new clay::Texture(*mpGraphicsContext_);
        pSolidTexture->initialize(singleRGBA);
        pSolidTexture->setSampler(*mResources_.getResource<VkSampler>("Default"));

        mResources_.addResource(std::unique_ptr<clay::Texture>(pSolidTexture), "SolidTexture");
    }
    // pipeline 
    {
        // (TextureDepth)
        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.pipelineLayoutInfo.shaders = {
            &textureVertShader, &textureFragShader
        };

        auto vertexAttrib = clay::Mesh::Vertex::getAttributeDescriptions();
        pipelineConfig.pipelineLayoutInfo.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.end()};
        pipelineConfig.pipelineLayoutInfo.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        pipelineConfig.pipelineLayoutInfo.depthStencilState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
        };

        pipelineConfig.pipelineLayoutInfo.rasterizerState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f,
        };

        pipelineConfig.pipelineLayoutInfo.pushConstants = {
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset = 0,
                .size = sizeof(glm::mat4) + sizeof(glm::vec4)
            }
        };

        pipelineConfig.bindingLayoutInfo.bindings = {
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr
            },
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr
            }
        };

        mResources_.addResource<clay::PipelineResource>(
            std::make_unique<clay::PipelineResource>(pipelineConfig),
            "TextureDepth"
        );
    }
    {
        // (SpriteSheet)
        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.pipelineLayoutInfo.shaders = {
            &spriteVertShader, &spriteFragShader
        };

        auto vertexAttrib = clay::Mesh::Vertex::getAttributeDescriptions();
        pipelineConfig.pipelineLayoutInfo.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.end()};
        pipelineConfig.pipelineLayoutInfo.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        pipelineConfig.pipelineLayoutInfo.depthStencilState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
        };

        pipelineConfig.pipelineLayoutInfo.rasterizerState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f,
        };

        pipelineConfig.pipelineLayoutInfo.pushConstants = {
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset = 0,
                .size = sizeof(glm::mat4) + sizeof(glm::vec4) + sizeof(glm::vec4)
            }
        };

        pipelineConfig.bindingLayoutInfo.bindings = {
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = nullptr
            },
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr
            }
        };

        mResources_.addResource<clay::PipelineResource>(
            std::make_unique<clay::PipelineResource>(pipelineConfig),
            "SpriteSheet"
        );
    }
    // Material
    {
        // Single White
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mResources_.getResource<clay::PipelineResource>("TextureDepth")
        };

        matConfig.bufferBindings = {
            {
                .buffer = mGraphicsContextDesktop_.mCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_.getResource<clay::Texture>("SolidTexture")->getSampler(),
                .imageView = mResources_.getResource<clay::Texture>("SolidTexture")->getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };


        mResources_.addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "SolidTexture"
        );
    }
    {
        // VTexture
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mResources_.getResource<clay::PipelineResource>("TextureDepth")
        };

        matConfig.bufferBindings = {
            {
                .buffer = mGraphicsContextDesktop_.mCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_.getResource<clay::Texture>("VTexture")->getSampler(),
                .imageView = mResources_.getResource<clay::Texture>("VTexture")->getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        mResources_.addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "VTexture"
        );
    }
    {
        // SpriteSheet
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = *mResources_.getResource<clay::PipelineResource>("SpriteSheet")
        };

        matConfig.bufferBindings = {
            {
                .buffer = mGraphicsContextDesktop_.mCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_.getResource<clay::Texture>("SpriteSheet")->getSampler(),
                .imageView = mResources_.getResource<clay::Texture>("SpriteSheet")->getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        mResources_.addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "SpriteSheet"
        );
    }
    // Models
    {
        // Solid Sphere
        std::unique_ptr<clay::Model> pSolidSphereModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pSolidSphereModel->addElement({
            mResources_.getResource<clay::Mesh>("Sphere"),
            mResources_.getResource<clay::Material>("SolidTexture"),
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(pSolidSphereModel), "SolidSphere");
    }
    {
        // V Texture Sphere
        std::unique_ptr<clay::Model> pSolidSphereModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pSolidSphereModel->addElement({
            mResources_.getResource<clay::Mesh>("Sphere"),
            mResources_.getResource<clay::Material>("VTexture"),
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(pSolidSphereModel), "VTexture");
    }
    {
        // Solid Plane
        std::unique_ptr<clay::Model> pSolidSphereModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pSolidSphereModel->addElement({
            mResources_.getResource<clay::Mesh>("Plane"),
            mResources_.getResource<clay::Material>("SolidTexture"),
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(pSolidSphereModel), "SolidPlane");
    }
    {
        // Solid Torus
        std::unique_ptr<clay::Model> pSolidSphereModel = std::make_unique<clay::Model>(*mpGraphicsContext_);
        pSolidSphereModel->addElement({
            mResources_.getResource<clay::Mesh>("Torus"),
            mResources_.getResource<clay::Material>("SolidTexture"),
            glm::mat4(1),
        });
        mResources_.addResource<clay::Model>(std::move(pSolidSphereModel), "SolidTorus");
    }
}