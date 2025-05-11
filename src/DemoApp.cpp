// third party
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
// class
#include "DemoApp.h"


DemoApp::DemoApp() : clay::AppDesktop() {
    loadResources();
}

DemoApp::~DemoApp() {}

void DemoApp::loadResources() {
    // default sampler
    mResources_.addResource<VkSampler>(
        std::make_unique<VkSampler>(mGraphicsContext_.createSampler()),
        "Default"
    );
    // sphere mesh
    {
       mResources_.loadResource<clay::Mesh>(
            {(clay::Resources::getResourcePath() / "models/Sphere.obj").string()}, 
            "Sphere"
        );
    }
    // images
    // VImage Textures
    {
        auto vImageData = clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "textures/V.png").string()
        );

        clay::utils::ImageData parsedImage;

        parsedImage.pixels.reset(
            stbi_load_from_memory(
                vImageData.data.get(),
                static_cast<int>(vImageData.size),
                &parsedImage.width, &parsedImage.height, &parsedImage.channels,
                0
            )
        );

        auto* pVTexture = new clay::Texture(mGraphicsContext_);
        pVTexture->initialize(parsedImage);
        pVTexture->setSampler(*mResources_.getResource<VkSampler>("Default"));

        mResources_.addResource(std::unique_ptr<clay::Texture>(pVTexture), "VTexture");
    }

    {
        // solid image
        clay::utils::ImageData singleRGBA;
        singleRGBA.width = 1;
        singleRGBA.height = 1;
        singleRGBA.channels = 4; // RGBA
        singleRGBA.pixels = std::make_unique<unsigned char[]>(4);

        singleRGBA.pixels[0] = 255;
        singleRGBA.pixels[1] = 255;
        singleRGBA.pixels[2] = 255;
        singleRGBA.pixels[3] = 255;

        auto* pSolidTexture = new clay::Texture(mGraphicsContext_);
        pSolidTexture->initialize(singleRGBA);
        pSolidTexture->setSampler(*mResources_.getResource<VkSampler>("Default"));

        mResources_.addResource(std::unique_ptr<clay::Texture>(pSolidTexture), "SolidTexture");
    }
    // pipeline (TextureDepth)
    {
        // shaders
        auto vertexFileData = clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "shaders/Texture.vert.spv").string()
        );
        VkShaderModule vertexShader = mGraphicsContext_.createShader(
            {VK_SHADER_STAGE_VERTEX_BIT, vertexFileData.data.get(), vertexFileData.size}
        );
        auto fragmentFileData = clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "shaders/Texture.frag.spv").string()
        );
        VkShaderModule fragmentShader = mGraphicsContext_.createShader(
            {VK_SHADER_STAGE_FRAGMENT_BIT, fragmentFileData.data.get(), fragmentFileData.size}
        );

        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = mGraphicsContext_
        };

        pipelineConfig.shaderStages = {
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_VERTEX_BIT,
                .module = vertexShader,
                .pName = "main"
            },
            {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
                .module = fragmentShader,
                .pName = "main"
            }
        };
        auto vertexAttrib = clay::Mesh::Vertex::getAttributeDescriptions();
        pipelineConfig.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.end()};
        pipelineConfig.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        pipelineConfig.pushConstants = {
            {
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .offset = 0,
                .size = sizeof(glm::mat4)
            },
            {
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .offset = 64,
                .size = sizeof(glm::vec4)
            }
        };
        pipelineConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        pipelineConfig.imageCreateInfos = {
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .pImmutableSamplers = nullptr,
            }
        };

        pipelineConfig.depthStencilState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable = VK_TRUE,
            .depthWriteEnable = VK_TRUE,
            .depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL,
            .depthBoundsTestEnable = VK_FALSE,
            .stencilTestEnable = VK_FALSE,
        };

        pipelineConfig.rasterizerState = {
            .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .depthClampEnable = VK_FALSE,
            .rasterizerDiscardEnable = VK_FALSE,
            .polygonMode = VK_POLYGON_MODE_FILL,
            .cullMode = VK_CULL_MODE_NONE,
            .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .depthBiasEnable = VK_FALSE,
            .lineWidth = 1.0f,
        };

        mResources_.addResource<clay::PipelineResource>(
            std::make_unique<clay::PipelineResource>(pipelineConfig),
            "TextureDepth"
        );
        // TODO delete after all pipelines are done with this
        vkDestroyShaderModule(mGraphicsContext_.mDevice_, vertexShader, nullptr);
        vkDestroyShaderModule(mGraphicsContext_.mDevice_, fragmentShader, nullptr);
    }
    // Material
    // Single White
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = mGraphicsContext_,
            .pipelineResource = *mResources_.getResource<clay::PipelineResource>("TextureDepth")
        };

        matConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        matConfig.imageCreateInfos = {
            {
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .imageView = mResources_.getResource<clay::Texture>("SolidTexture")->mImageView_,
                .sampler = mResources_.getResource<clay::Texture>("SolidTexture")->mSampler_,
                .binding = 1
            }
        };

        mResources_.addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "SolidTexture"
        );
    }
    // VTexture
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = mGraphicsContext_,
            .pipelineResource = *mResources_.getResource<clay::PipelineResource>("TextureDepth")
        };

        matConfig.bufferCreateInfos = {
            {
                .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                .stride = 0,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                .binding = 0
            },
        };

        matConfig.imageCreateInfos = {
            {
                .data = nullptr,
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                .imageView = mResources_.getResource<clay::Texture>("VTexture")->mImageView_,
                .sampler = mResources_.getResource<clay::Texture>("VTexture")->mSampler_,
                .binding = 1
            }
        };

        mResources_.addResource<clay::Material>(
            std::make_unique<clay::Material>(matConfig),
            "VTexture"
        );
    }
    // Models
    // solid sphere
    {
        std::unique_ptr<clay::Model> pSolidSphereModel = std::make_unique<clay::Model>(mGraphicsContext_);
        pSolidSphereModel->mMeshes_.push_back(mResources_.getResource<clay::Mesh>("Sphere"));
        pSolidSphereModel->mMaterials_.push_back(mResources_.getResource<clay::Material>("SolidTexture"));

        mResources_.addResource<clay::Model>(std::move(pSolidSphereModel), "SolidSphere");
    }
    {
        std::unique_ptr<clay::Model> pSolidSphereModel = std::make_unique<clay::Model>(mGraphicsContext_);
        pSolidSphereModel->mMeshes_.push_back(mResources_.getResource<clay::Mesh>("Sphere"));
        pSolidSphereModel->mMaterials_.push_back(mResources_.getResource<clay::Material>("VTexture"));

        mResources_.addResource<clay::Model>(std::move(pSolidSphereModel), "VTexture");
    }

}