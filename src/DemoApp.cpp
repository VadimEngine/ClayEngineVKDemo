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
            clay::Font(*mpGraphicsContext_, fontData, fontVertShader, fontFragShader, *mGraphicsContextDesktop_.mCameraUniform_),
            "Runescape"
        );
    }

    clay::Resources::Handle<VkSampler> samplerHandle_Default;

    clay::Resources::Handle<clay::Texture> textureHandle_VTexture;
    clay::Resources::Handle<clay::Texture> textureHandle_SpriteSheet;
    clay::Resources::Handle<clay::Texture> textureHandle_SolidTexture;
    clay::Resources::Handle<clay::Texture> textureHandle_Sun;
    clay::Resources::Handle<clay::Texture> textureHandle_Moon;
    clay::Resources::Handle<clay::Texture> textureHandle_Earth;
    clay::Resources::Handle<clay::Texture> textureHandle_Stars;

    clay::Resources::Handle<clay::Mesh> meshHandler_Sphere;
    clay::Resources::Handle<clay::Mesh> meshHandler_Plane;
    clay::Resources::Handle<clay::Mesh> meshHandler_Cube;
    clay::Resources::Handle<clay::Mesh> meshHandler_Torus;
    clay::Resources::Handle<clay::Mesh> meshHandler_PlaneCircle;

    clay::Resources::Handle<clay::PipelineResource> pipelineHandle_TextureDepth;
    clay::Resources::Handle<clay::PipelineResource> pipelineHandle_TextureNoDepth;
    clay::Resources::Handle<clay::PipelineResource> pipelineHandle_SpriteSheet;

    clay::Resources::Handle<clay::Material> materialHandle_Solid;
    clay::Resources::Handle<clay::Material> materialHandle_VTexture;
    clay::Resources::Handle<clay::Material> materialHandle_SpriteSheet;
    clay::Resources::Handle<clay::Material> materialHandle_Moon;
    clay::Resources::Handle<clay::Material> materialHandle_Sun;
    clay::Resources::Handle<clay::Material> materialHandle_Earth;
    clay::Resources::Handle<clay::Material> materialHandle_Stars;

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
        // TODO confirm move is needed
        samplerHandle_Default = mResources_.addResource(std::move(sampler), "Default");
    }
    // Meshes
    // Sphere
    meshHandler_Sphere = mResources_.loadResource<clay::Mesh>({(clay::Resources::getResourcePath() / "models/Sphere.obj").string()}, "Sphere");
    // Plane
    meshHandler_Plane = mResources_.loadResource<clay::Mesh>({(clay::Resources::getResourcePath() / "models/Plane.obj").string()}, "Plane");
    // Cube
    meshHandler_Cube = mResources_.loadResource<clay::Mesh>({(clay::Resources::getResourcePath() / "models/Cube.obj").string()}, "Cube");
    // Torus
    meshHandler_Torus = mResources_.loadResource<clay::Mesh>({(clay::Resources::getResourcePath() / "models/Torus.obj").string()}, "Torus");
    // Plane Circle
    meshHandler_PlaneCircle = mResources_.loadResource<clay::Mesh>({(clay::Resources::getResourcePath() / "models/PlaneCircle.obj").string()}, "PlaneCircle");
    // images
    {
        clay::utils::ImageData imageData = clay::utils::loadImageFileToMemory_desktop(clay::Resources::getResourcePath() / "textures/V.png");

        clay::Texture vTexture(*mpGraphicsContext_);
        vTexture.initialize(imageData);
        vTexture.setSampler(mResources_[samplerHandle_Default]);

        textureHandle_VTexture = mResources_.addResource(std::move(vTexture), "VTexture");
    }
    {
        clay::utils::ImageData imageData = clay::utils::loadImageFileToMemory_desktop(clay::Resources::getResourcePath() / "textures/Sprites.png");

        clay::Texture texture(*mpGraphicsContext_);
        texture.initialize(imageData);
        texture.setSampler(mResources_[samplerHandle_Default]);

        textureHandle_SpriteSheet = mResources_.addResource(std::move(texture), "SpriteSheet");
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

        clay::Texture solidTexture(*mpGraphicsContext_);
        solidTexture.initialize(singleRGBA);
        solidTexture.setSampler(mResources_[samplerHandle_Default]);

        textureHandle_SolidTexture = mResources_.addResource(std::move(solidTexture), "SolidTexture");
    }
    {
        // Sun
        clay::utils::ImageData imageData = clay::utils::loadImageFileToMemory_desktop(clay::Resources::getResourcePath() / "textures/sunmap.jpg");
        clay::utils::convertRGBtoRGBA(imageData);

        clay::Texture texture(*mpGraphicsContext_);
        texture.initialize(imageData);
        texture.setSampler(mResources_[samplerHandle_Default]);

        textureHandle_Sun = mResources_.addResource(std::move(texture), "Sun");
    }
    {
        // Moon
        clay::utils::ImageData imageData = clay::utils::loadImageFileToMemory_desktop(clay::Resources::getResourcePath() / "textures/2k_moon.jpg");
        clay::utils::convertRGBtoRGBA(imageData);

        clay::Texture texture (*mpGraphicsContext_);
        texture.initialize(imageData);
        texture.setSampler(mResources_[samplerHandle_Default]);

        textureHandle_Moon = mResources_.addResource(std::move(texture), "Moon");
    }
    {
        // Earth
        clay::utils::ImageData imageData = clay::utils::loadImageFileToMemory_desktop(clay::Resources::getResourcePath() / "textures/earthmap1k.jpg");
        clay::utils::convertRGBtoRGBA(imageData);

        clay::Texture texture(*mpGraphicsContext_);
        texture.initialize(imageData);
        texture.setSampler(mResources_[samplerHandle_Default]);

        textureHandle_Earth = mResources_.addResource(std::move(texture), "Earth");
    }
    {
        // Stars
        clay::utils::ImageData imageData = clay::utils::loadImageFileToMemory_desktop(clay::Resources::getResourcePath() / "textures/8k_stars_milky_way.jpg");
        clay::utils::convertRGBtoRGBA(imageData);

        clay::Texture texture(*mpGraphicsContext_);
        texture.initialize(imageData);
        texture.setSampler(mResources_[samplerHandle_Default]);

        textureHandle_Stars = mResources_.addResource(std::move(texture), "Stars");
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
            .cullMode = VK_CULL_MODE_BACK_BIT,
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

        pipelineHandle_TextureDepth = pipelineHandle_TextureDepth = mResources_.addResource<clay::PipelineResource>(
            clay::PipelineResource(pipelineConfig),
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

        pipelineHandle_SpriteSheet = mResources_.addResource<clay::PipelineResource>(
            clay::PipelineResource(pipelineConfig),
            "SpriteSheet"
        );
    }
    {
        // TextureNoDepth
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
            .depthTestEnable = VK_FALSE,
            .depthWriteEnable = VK_FALSE,
            .depthCompareOp = VK_COMPARE_OP_LESS,
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

        pipelineHandle_TextureNoDepth = mResources_.addResource<clay::PipelineResource>(
            clay::PipelineResource(pipelineConfig),
            "TextureNoDepth"
        );
    }
    // Material
    {
        // Single White
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureDepth]
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
                .sampler = mResources_[textureHandle_SolidTexture].getSampler(),
                .imageView = mResources_[textureHandle_SolidTexture].getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };


        materialHandle_Solid = mResources_.addResource<clay::Material>(
            clay::Material(matConfig),
            "SolidTexture"
        );
    }
    {
        // VTexture
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureDepth]
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
                .sampler = mResources_[textureHandle_VTexture].getSampler(),
                .imageView = mResources_[textureHandle_VTexture].getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        materialHandle_VTexture = mResources_.addResource<clay::Material>(
            clay::Material(matConfig),
            "VTexture"
        );
    }
    {
        // SpriteSheet
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_SpriteSheet]
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
                .sampler = mResources_[textureHandle_SpriteSheet].getSampler(),
                .imageView = mResources_[textureHandle_SpriteSheet].getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        materialHandle_SpriteSheet = materialHandle_SpriteSheet = mResources_.addResource<clay::Material>(
            clay::Material(matConfig),
            "SpriteSheet"
        );
    }
    {
        // Sun
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureDepth]
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
                .sampler = mResources_[textureHandle_Sun].getSampler(),
                .imageView = mResources_[textureHandle_Sun].getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        materialHandle_Sun = mResources_.addResource<clay::Material>(
            clay::Material(matConfig),
            "Sun"
        );
    }
    {
        // Moon
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureDepth]
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
                .sampler = mResources_[textureHandle_Moon].getSampler(),
                .imageView = mResources_[textureHandle_Moon].getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        materialHandle_Moon = mResources_.addResource<clay::Material>(
            clay::Material(matConfig),
            "Moon"
        );
    }
    {
        // Earth
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureDepth]
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
                .sampler = mResources_[textureHandle_Earth].getSampler(),
                .imageView = mResources_[textureHandle_Earth].getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        materialHandle_Earth = mResources_.addResource<clay::Material>(
            clay::Material(matConfig),
            "Earth"
        );
    }
    {
        // Stars
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureNoDepth]
        };

        matConfig.bufferBindings = {
            {
                .buffer = mGraphicsContextDesktop_.mCameraUniformHeadLocked_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_Stars].getSampler(),
                .imageView = mResources_[textureHandle_Stars].getImageView(),
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER
            }
        };

        materialHandle_Stars = mResources_.addResource<clay::Material>(
            clay::Material(matConfig),
            "Stars"
        );
    }
    // Models
    {
        // Solid Sphere
        clay::Model model(*mpGraphicsContext_);
        model.addElement({
            &mResources_[meshHandler_Sphere],
            &mResources_[materialHandle_Solid],
            glm::mat4(1),
        });
        mResources_.addResource(std::move(model), "SolidSphere");
    }
    {
        // V Texture Sphere
        clay::Model model(*mpGraphicsContext_);
        model.addElement({
            &mResources_[meshHandler_Sphere],
            &mResources_[materialHandle_VTexture],
            glm::mat4(1),
        });
        mResources_.addResource(std::move(model), "VSphere");
    }
    {
        // Solid Plane
        clay::Model model(*mpGraphicsContext_);
        model.addElement({
            &mResources_[meshHandler_Plane],
            &mResources_[materialHandle_Solid],
            glm::mat4(1),
        });
        mResources_.addResource(std::move(model), "SolidPlane");
    }
    {
        // Solid Torus
        clay::Model model(*mpGraphicsContext_);
        model.addElement({
            &mResources_[meshHandler_Torus],
            &mResources_[materialHandle_Solid],
            glm::mat4(1),
        });
        mResources_.addResource(std::move(model), "SolidTorus");
    }
    {
        // Sun
        clay::Model model(*mpGraphicsContext_);
        model.addElement({
            &mResources_[meshHandler_Sphere],
            &mResources_[materialHandle_Sun],
            glm::mat4(1),
        });
        mResources_.addResource(std::move(model), "Sun");
    }
    {
        // Moon
        clay::Model model(*mpGraphicsContext_);
        model.addElement({
            &mResources_[meshHandler_Sphere],
            &mResources_[materialHandle_Moon],
            glm::mat4(1),
        });
        mResources_.addResource(std::move(model), "Moon");
    }
    {
        // Earth
        clay::Model model(*mpGraphicsContext_);
        model.addElement({
            &mResources_[meshHandler_Torus],
            &mResources_[materialHandle_Earth],
            glm::mat4(1),
        });
        mResources_.addResource(std::move(model), "Earth");
    }
    {
        // Solid Circle
        clay::Model model(*mpGraphicsContext_);
        model.addElement({
            &mResources_[meshHandler_PlaneCircle],
            &mResources_[materialHandle_Solid],
            glm::mat4(1),
        });
        mResources_.addResource(std::move(model), "SolidCircle");
    }
    {
        // Solid Plane
        clay::Model model(*mpGraphicsContext_);
        model.addElement({
            &mResources_[meshHandler_Plane],
            &mResources_[materialHandle_Solid],
            glm::mat4(1),
        });
        mResources_.addResource(std::move(model), "SolidPlane");
    }
}