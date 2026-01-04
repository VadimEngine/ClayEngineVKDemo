// clay
#include <clay/utils/desktop/UtilsDesktop.h>
#include <clay/graphics/common/ShaderModule.h>
#include <clay/graphics/common/AnimatedMesh.h>
#include <clay/utils/common/Logger.h>
// project
#include "scenes/menu_scene/MenuScene.h"
// class
#include "DemoApp.h"

DemoApp::DemoApp(clay::Window& window)
    : clay::AppDesktop(window) {
    loadResources();
    setScene(new MenuScene(*this));
}

DemoApp::~DemoApp() {
}

void DemoApp::loadResources() {
    // audio
    {
        auto pongBounceData = clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "audio/beep_deep_1.wav").string()
        );
        mResources_.addResource<clay::Audio>(
            clay::Audio(pongBounceData),
            "BeepDeep1"
        );
    }
    {
        auto pongBounceData = clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "audio/Blip_1.wav").string()
        );
        mResources_.addResource<clay::Audio>(
            clay::Audio(pongBounceData),
            "Blip1"
        );
    }
    {
        auto pongBounceData = clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "audio/PatakasWorld.wav").string()
        );
        mResources_.addResource<clay::Audio>(
            clay::Audio(pongBounceData),
            "PatakasWorld"
        );
    }
    // Texture
    clay::ShaderModule textureVertShader(
        mpGraphicsContext_->getDevice(),
        vk::ShaderStageFlagBits::eVertex,
        clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "shaders/Texture.vert.spv").string()
        )
    );
    clay::ShaderModule textureFragShader(
        mpGraphicsContext_->getDevice(),
        vk::ShaderStageFlagBits::eFragment,
        clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "shaders/Texture.frag.spv").string()
        )
    );
    // sprite
    clay::ShaderModule spriteVertShader(
        mpGraphicsContext_->getDevice(),
        vk::ShaderStageFlagBits::eVertex,
        clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "shaders/Sprite.vert.spv").string()
        )
    );
    clay::ShaderModule spriteFragShader(
        mpGraphicsContext_->getDevice(),
        vk::ShaderStageFlagBits::eFragment,
        clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "shaders/Sprite.frag.spv").string()
        )
    );
    // tilemap instanced
    clay::ShaderModule tilemapInstancedVertShader(
        mpGraphicsContext_->getDevice(),
        vk::ShaderStageFlagBits::eVertex,
        clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "shaders/TilemapInstanced.vert.spv").string()
        )
    );
    clay::ShaderModule tilemapInstancedFragShader(
        mpGraphicsContext_->getDevice(),
        vk::ShaderStageFlagBits::eFragment,
        clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "shaders/TilemapInstanced.frag.spv").string()
        )
    );
    // flat shader
    clay::ShaderModule flatVertShader(
        mpGraphicsContext_->getDevice(),
        vk::ShaderStageFlagBits::eVertex,
        clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "shaders/Flat.vert.spv").string()
        )
    );
    clay::ShaderModule flatFragShader(
        mpGraphicsContext_->getDevice(),
        vk::ShaderStageFlagBits::eFragment,
        clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "shaders/Flat.frag.spv").string()
        )
    );
    // animation shader
    clay::ShaderModule animationVertShader(
        mpGraphicsContext_->getDevice(),
        vk::ShaderStageFlagBits::eVertex,
        clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "shaders/Animation.vert.spv").string()
        )
    );
    clay::ShaderModule animationFragShader(
        mpGraphicsContext_->getDevice(),
        vk::ShaderStageFlagBits::eFragment,
        clay::utils::loadFileToMemory_desktop(
            (clay::Resources::getResourcePath() / "shaders/Animation.frag.spv").string()
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
            vk::ShaderStageFlagBits::eVertex,
            clay::utils::loadFileToMemory_desktop(
                (clay::Resources::getResourcePath() / "shaders/Text.vert.spv").string()
            )
        );
        clay::ShaderModule fontFragShader(
            mpGraphicsContext_->getDevice(),
            vk::ShaderStageFlagBits::eFragment,
            clay::utils::loadFileToMemory_desktop(
                (clay::Resources::getResourcePath() / "shaders/Text.frag.spv").string()
            )
        );

        mResources_.addResource<clay::Font>(
            clay::Font(*mpGraphicsContext_, fontData, fontVertShader, fontFragShader, *mGraphicsContextDesktop_.mCameraUniform_),
            "Runescape"
        );
    }

    clay::Handle<clay::Texture> textureHandle_VTexture;
    clay::Handle<clay::Texture> textureHandle_SpriteSheet;
    clay::Handle<clay::Texture> textureHandle_SolidTexture;
    clay::Handle<clay::Texture> textureHandle_Sun;
    clay::Handle<clay::Texture> textureHandle_Moon;
    clay::Handle<clay::Texture> textureHandle_Earth;
    clay::Handle<clay::Texture> textureHandle_Stars;
    clay::Handle<clay::Texture> textureHandle_CloudSky;
    clay::Handle<clay::Texture> textureHandle_RpgMap;

    clay::Handle<clay::PipelineResource> pipelineHandle_TextureDepth;
    clay::Handle<clay::PipelineResource> pipelineHandle_TextureNoDepth;
    clay::Handle<clay::PipelineResource> pipelineHandle_SpriteSheet;
    clay::Handle<clay::PipelineResource> pipelineHandle_TilemapInstanced;
    clay::Handle<clay::PipelineResource> pipelineHandle_Flat;
    clay::Handle<clay::PipelineResource> pipelineHandle_Animation;

    clay::Handle<vk::Sampler> samplerHandle_Default;

    // Sampler
    {
        // default sampler
        vk::Sampler sampler;

        vk::SamplerCreateInfo samplerInfo{
            .magFilter = vk::Filter::eNearest,
            .minFilter = vk::Filter::eNearest,
            .mipmapMode = vk::SamplerMipmapMode::eNearest,
            .addressModeU = vk::SamplerAddressMode::eClampToBorder,
            .addressModeV = vk::SamplerAddressMode::eClampToBorder,
            .addressModeW = vk::SamplerAddressMode::eClampToBorder,
            .mipLodBias = 0.0f,
            .anisotropyEnable = vk::False,
            .maxAnisotropy = 1.0f,
            .compareEnable = vk::False,
            .compareOp = vk::CompareOp::eAlways,
            .minLod = 0.0f,
            .maxLod = 0.0f,
            .borderColor = vk::BorderColor::eFloatTransparentBlack,
            .unnormalizedCoordinates = vk::False,
        };

        sampler = mpGraphicsContext_->getDevice().createSampler(samplerInfo);
        // TODO confirm move is needed
        samplerHandle_Default = mResources_.addResource(std::move(sampler), "Default");
    }
    // Meshes
    // Sphere
    mAppProp.mMeshes.sphere = mResources_.loadResource<clay::Mesh>({(clay::Resources::getResourcePath() / "models/Sphere.obj").string()}, "Sphere");
    // Plane
    mAppProp.mMeshes.plane = mResources_.loadResource<clay::Mesh>({(clay::Resources::getResourcePath() / "models/Plane.obj").string()}, "Plane");
    // Cube
    mAppProp.mMeshes.cube = mResources_.loadResource<clay::Mesh>({(clay::Resources::getResourcePath() / "models/Cube.obj").string()}, "Cube");
    // Torus
    mAppProp.mMeshes.torus = mResources_.loadResource<clay::Mesh>({(clay::Resources::getResourcePath() / "models/Torus.obj").string()}, "Torus");
    // Plane Circle
    mAppProp.mMeshes.planeCircle = mResources_.loadResource<clay::Mesh>({(clay::Resources::getResourcePath() / "models/PlaneCircle.obj").string()}, "PlaneCircle");
    // Human
    mAppProp.mMeshes.human = mResources_.loadResource<clay::Mesh>({(clay::Resources::getResourcePath() / "models/rpg_3d/Human_lowPoly.obj").string()}, "Human");
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
    {
        // CloudSky
        clay::utils::ImageData imageData = clay::utils::loadImageFileToMemory_desktop(clay::Resources::getResourcePath() / "textures/CloudSky.jpg");
        clay::utils::convertRGBtoRGBA(imageData);

        clay::Texture texture(*mpGraphicsContext_);
        texture.initialize(imageData);
        texture.setSampler(mResources_[samplerHandle_Default]);

        textureHandle_CloudSky = mResources_.addResource(std::move(texture), "CloudSky");
    }
    {
        // rpg map
        clay::utils::ImageData imageData = clay::utils::loadImageFileToMemory_desktop(clay::Resources::getResourcePath() / "textures/rpg_2d/World1.png");
        clay::utils::convertRGBtoRGBA(imageData);

        clay::Texture texture(*mpGraphicsContext_);
        texture.initialize(imageData);
        texture.setSampler(mResources_[samplerHandle_Default]);

        textureHandle_RpgMap = mResources_.addResource(std::move(texture), "RpgMap");
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
        // Only use first 3 attributes (position, normal, texCoord) - shaders don't use tangent/bitangent
        pipelineConfig.pipelineLayoutInfo.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.begin() + 3};
        pipelineConfig.pipelineLayoutInfo.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        pipelineConfig.pipelineLayoutInfo.depthStencilState = {
            .depthTestEnable = vk::True,
            .depthWriteEnable = vk::True,
            .depthCompareOp = vk::CompareOp::eLessOrEqual,
            .depthBoundsTestEnable = vk::False,
            .stencilTestEnable = vk::False,
        };

        pipelineConfig.pipelineLayoutInfo.rasterizerState = {
            .depthClampEnable = vk::False,
            .rasterizerDiscardEnable = vk::False,
            .polygonMode = vk::PolygonMode::eFill,
            .cullMode = vk::CullModeFlagBits::eBack,
            .frontFace = vk::FrontFace::eCounterClockwise,
            .depthBiasEnable = vk::False,
            .lineWidth = 1.0f,
        };

        pipelineConfig.pipelineLayoutInfo.pushConstants = {
            {
                vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                0,
                sizeof(glm::mat4) + sizeof(glm::vec4)
            }
        };

        pipelineConfig.bindingLayoutInfo.bindings = {
            {
                0,
                vk::DescriptorType::eUniformBuffer,
                1,
                vk::ShaderStageFlagBits::eVertex,
                nullptr
            },
            {
                1,
                vk::DescriptorType::eCombinedImageSampler,
                1,
                vk::ShaderStageFlagBits::eFragment,
                nullptr
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
        // Only use first 3 attributes (position, normal, texCoord) - shaders don't use tangent/bitangent
        pipelineConfig.pipelineLayoutInfo.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.begin() + 3};
        pipelineConfig.pipelineLayoutInfo.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        pipelineConfig.pipelineLayoutInfo.depthStencilState = {
            .depthTestEnable = vk::True,
            .depthWriteEnable = vk::True,
            .depthCompareOp =  vk::CompareOp::eLessOrEqual,
            .depthBoundsTestEnable = vk::False,
            .stencilTestEnable = vk::False,
        };

        pipelineConfig.pipelineLayoutInfo.rasterizerState = {
            .depthClampEnable = vk::False,
            .rasterizerDiscardEnable = vk::False,
            .polygonMode = vk::PolygonMode::eFill,
            .cullMode = vk::CullModeFlagBits::eNone,
            .frontFace = vk::FrontFace::eCounterClockwise,
            .depthBiasEnable = vk::False,
            .lineWidth = 1.0f,
        };

        pipelineConfig.pipelineLayoutInfo.pushConstants = {
            {
                vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                0,
                sizeof(glm::mat4) + sizeof(glm::vec4) + sizeof(glm::vec4)
            }
        };

        pipelineConfig.bindingLayoutInfo.bindings = {
            {
                0,
                vk::DescriptorType::eUniformBuffer,
                1,
                vk::ShaderStageFlagBits::eVertex,
                nullptr
            },
            {
                1,
                vk::DescriptorType::eCombinedImageSampler,
                1,
                vk::ShaderStageFlagBits::eFragment,
                nullptr
            }
        };

        pipelineHandle_SpriteSheet = mResources_.addResource<clay::PipelineResource>(
            clay::PipelineResource(pipelineConfig),
            "SpriteSheet"
        );
    }
    {
        // (TilemapInstanced)
        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.pipelineLayoutInfo.shaders = {
            &tilemapInstancedVertShader, &tilemapInstancedFragShader
        };

        auto vertexAttrib = clay::Mesh::Vertex::getAttributeDescriptions();
        // Only use first 3 attributes (position, normal, texCoord) - shaders don't use tangent/bitangent
        pipelineConfig.pipelineLayoutInfo.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.begin() + 3};
        pipelineConfig.pipelineLayoutInfo.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        pipelineConfig.pipelineLayoutInfo.depthStencilState = {
            .depthTestEnable = vk::True,
            .depthWriteEnable = vk::True,
            .depthCompareOp =  vk::CompareOp::eLessOrEqual,
            .depthBoundsTestEnable = vk::False,
            .stencilTestEnable = vk::False,
        };

        pipelineConfig.pipelineLayoutInfo.rasterizerState = {
            .depthClampEnable = vk::False,
            .rasterizerDiscardEnable = vk::False,
            .polygonMode = vk::PolygonMode::eFill,
            .cullMode = vk::CullModeFlagBits::eNone,
            .frontFace = vk::FrontFace::eCounterClockwise,
            .depthBiasEnable = vk::False,
            .lineWidth = 1.0f,
        };

        // No push constants for instanced rendering
        pipelineConfig.pipelineLayoutInfo.pushConstants = {};

        pipelineConfig.bindingLayoutInfo.bindings = {
            {
                0,
                vk::DescriptorType::eUniformBuffer,
                1,
                vk::ShaderStageFlagBits::eVertex,
                nullptr
            },
            {
                1,
                vk::DescriptorType::eCombinedImageSampler,
                1,
                vk::ShaderStageFlagBits::eFragment,
                nullptr
            },
            {
                2,
                vk::DescriptorType::eStorageBuffer,
                1,
                vk::ShaderStageFlagBits::eVertex,
                nullptr
            }
        };

        pipelineHandle_TilemapInstanced = mResources_.addResource<clay::PipelineResource>(
            clay::PipelineResource(pipelineConfig),
            "TilemapInstanced"
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
        // Only use first 3 attributes (position, normal, texCoord) - shaders don't use tangent/bitangent
        pipelineConfig.pipelineLayoutInfo.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.begin() + 3};
        pipelineConfig.pipelineLayoutInfo.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        pipelineConfig.pipelineLayoutInfo.depthStencilState = {
            .depthTestEnable = vk::False,
            .depthWriteEnable = vk::False,
            .depthCompareOp = vk::CompareOp::eLess,
            .depthBoundsTestEnable = vk::False,
            .stencilTestEnable = vk::False,
        };

        pipelineConfig.pipelineLayoutInfo.rasterizerState = {
            .depthClampEnable = vk::False,
            .rasterizerDiscardEnable = vk::False,
            .polygonMode = vk::PolygonMode::eFill,
            .cullMode = vk::CullModeFlagBits::eNone,
            .frontFace = vk::FrontFace::eCounterClockwise,
            .depthBiasEnable = vk::False,
            .lineWidth = 1.0f,
        };

        pipelineConfig.pipelineLayoutInfo.pushConstants = {
            {
                vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                0,
                sizeof(glm::mat4) + sizeof(glm::vec4)
            }
        };

        pipelineConfig.bindingLayoutInfo.bindings = {
            {
                0,
                vk::DescriptorType::eUniformBuffer,
                1,
                vk::ShaderStageFlagBits::eVertex,
                nullptr
            },
            {
                1,
                vk::DescriptorType::eCombinedImageSampler,
                1,
                vk::ShaderStageFlagBits::eFragment,
                nullptr
            }
        };

        pipelineHandle_TextureNoDepth = mResources_.addResource<clay::PipelineResource>(
            clay::PipelineResource(pipelineConfig),
            "TextureNoDepth"
        );
    }
    // pipeline (flat)
    {
        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.pipelineLayoutInfo.shaders = {
            &flatVertShader, &flatFragShader
        };

        auto vertexAttrib = clay::Mesh::Vertex::getAttributeDescriptions();
        // Only use first 3 attributes (position, normal, texCoord) - shaders don't use tangent/bitangent
        pipelineConfig.pipelineLayoutInfo.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.begin() + 3};
        pipelineConfig.pipelineLayoutInfo.vertexInputBindingDescription = clay::Mesh::Vertex::getBindingDescription();

        pipelineConfig.pipelineLayoutInfo.depthStencilState = {
            .depthTestEnable = true,
            .depthWriteEnable = true,
            .depthCompareOp = vk::CompareOp::eLessOrEqual,
            .depthBoundsTestEnable = false,
            .stencilTestEnable = false,
        };

        pipelineConfig.pipelineLayoutInfo.rasterizerState = {
            .depthClampEnable = false,
            .rasterizerDiscardEnable = false,
            .polygonMode = vk::PolygonMode::eFill,
            .cullMode = vk::CullModeFlagBits::eNone,
            .frontFace = vk::FrontFace::eCounterClockwise,
            .depthBiasEnable = false,
            .lineWidth = 1.0f,
        };

        pipelineConfig.pipelineLayoutInfo.pushConstants = {
            {
                .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                .offset = 0,
                .size = sizeof(glm::mat4) + sizeof(glm::vec4)
            }
        };

        pipelineConfig.bindingLayoutInfo.bindings = {
            {
                .binding = 0,
                .descriptorType = vk::DescriptorType::eUniformBuffer,
                .descriptorCount = 1,
                .stageFlags = vk::ShaderStageFlagBits::eVertex,
                .pImmutableSamplers = nullptr
            },
        };

        pipelineHandle_Flat = mResources_.addResource<clay::PipelineResource>(
            clay::PipelineResource(pipelineConfig),
            "Flat"
        );
    }
    // pipeline (animation)
    {
        clay::PipelineResource::PipelineConfig pipelineConfig{
            .graphicsContext = *mpGraphicsContext_
        };

        pipelineConfig.pipelineLayoutInfo.shaders = {
            &animationVertShader, &animationFragShader
        };

        auto vertexAttrib = clay::AnimatedMesh::Vertex::getAttributeDescriptions();
        pipelineConfig.pipelineLayoutInfo.attributeDescriptions = {vertexAttrib.begin(), vertexAttrib.end()};
        pipelineConfig.pipelineLayoutInfo.vertexInputBindingDescription = clay::AnimatedMesh::Vertex::getBindingDescription();

        pipelineConfig.pipelineLayoutInfo.depthStencilState = {
            .depthTestEnable = true,
            .depthWriteEnable = true,
            .depthCompareOp = vk::CompareOp::eLessOrEqual,
            .depthBoundsTestEnable = false,
            .stencilTestEnable = false,
        };

        pipelineConfig.pipelineLayoutInfo.rasterizerState = {
            .depthClampEnable = false,
            .rasterizerDiscardEnable = false,
            .polygonMode = vk::PolygonMode::eFill,
            .cullMode = vk::CullModeFlagBits::eBack,
            .frontFace = vk::FrontFace::eCounterClockwise,
            .depthBiasEnable = false,
            .lineWidth = 1.0f,
        };

        pipelineConfig.pipelineLayoutInfo.pushConstants = {
            {
                .stageFlags = vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
                .offset = 0,
                .size = sizeof(glm::mat4) + sizeof(glm::vec4)
            }
        };

        pipelineConfig.bindingLayoutInfo.bindings = {
            {
                .binding = 0,
                .descriptorType = vk::DescriptorType::eUniformBuffer,
                .descriptorCount = 1,
                .stageFlags = vk::ShaderStageFlagBits::eVertex,
                .pImmutableSamplers = nullptr
            },
            {
                .binding = 1,
                .descriptorType = vk::DescriptorType::eUniformBuffer,
                .descriptorCount = 1,
                .stageFlags = vk::ShaderStageFlagBits::eVertex,
                .pImmutableSamplers = nullptr
            },
        };

        pipelineHandle_Animation = mResources_.addResource<clay::PipelineResource>(
            clay::PipelineResource(pipelineConfig),
            "Animation3D"
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
                .descriptorType = vk::DescriptorType::eUniformBuffer
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_SolidTexture].getSampler(),
                .imageView = mResources_[textureHandle_SolidTexture].getImageView(),
                .binding = 1,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler
            }
        };


        mAppProp.mMaterials.solid = mResources_.addResource<clay::Material>(
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
                .descriptorType = vk::DescriptorType::eUniformBuffer
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_VTexture].getSampler(),
                .imageView = mResources_[textureHandle_VTexture].getImageView(),
                .binding = 1,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler
            }
        };

        mAppProp.mMaterials.vTexture = mResources_.addResource<clay::Material>(
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
                .descriptorType = vk::DescriptorType::eUniformBuffer
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_SpriteSheet].getSampler(),
                .imageView = mResources_[textureHandle_SpriteSheet].getImageView(),
                .binding = 1,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler
            }
        };

        mAppProp.mMaterials.spriteSheet = mResources_.addResource<clay::Material>(
            clay::Material(matConfig),
            "SpriteSheet"
        );
    }
    {
        // TilemapInstanced (SSBO will be set by Tilemap class when load() is called)
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TilemapInstanced]
        };

        matConfig.bufferBindings = {
            {
                .buffer = mGraphicsContextDesktop_.mCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = vk::DescriptorType::eUniformBuffer
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_SpriteSheet].getSampler(),
                .imageView = mResources_[textureHandle_SpriteSheet].getImageView(),
                .binding = 1,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler
            }
        };
        // Note: SSBO binding (binding = 2) will be added by Tilemap when it creates the instance buffer

        mAppProp.mMaterials.tilemapInstanced = mResources_.addResource<clay::Material>(
            clay::Material(matConfig),
            "TilemapInstanced"
        );
    }
    {
        // RpgMap
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_SpriteSheet]
        };

        matConfig.bufferBindings = {
            {
                .buffer = mGraphicsContextDesktop_.mCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = vk::DescriptorType::eUniformBuffer
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_RpgMap].getSampler(),
                .imageView = mResources_[textureHandle_RpgMap].getImageView(),
                .binding = 1,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler
            }
        };

        mAppProp.mMaterials.rpgMap = mResources_.addResource<clay::Material>(
            clay::Material(matConfig),
            "RpgMap"
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
                .descriptorType = vk::DescriptorType::eUniformBuffer
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_Sun].getSampler(),
                .imageView = mResources_[textureHandle_Sun].getImageView(),
                .binding = 1,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler
            }
        };

        mAppProp.mMaterials.sun = mResources_.addResource<clay::Material>(
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
                .descriptorType = vk::DescriptorType::eUniformBuffer
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_Moon].getSampler(),
                .imageView = mResources_[textureHandle_Moon].getImageView(),
                .binding = 1,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler
            }
        };

        mAppProp.mMaterials.moon = mResources_.addResource<clay::Material>(
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
                .descriptorType = vk::DescriptorType::eUniformBuffer
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_Earth].getSampler(),
                .imageView = mResources_[textureHandle_Earth].getImageView(),
                .binding = 1,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler
            }
        };

        mAppProp.mMaterials.earth = mResources_.addResource<clay::Material>(
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
                .descriptorType = vk::DescriptorType::eUniformBuffer
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_Stars].getSampler(),
                .imageView = mResources_[textureHandle_Stars].getImageView(),
                .binding = 1,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler
            }
        };

        mAppProp.mMaterials.stars = mResources_.addResource<clay::Material>(
            clay::Material(matConfig),
            "Stars"
        );
    }
    {
        // CloudSky
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureNoDepth]
        };

        matConfig.bufferBindings = {
            {
                .buffer = mGraphicsContextDesktop_.mCameraUniformHeadLocked_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = vk::DescriptorType::eUniformBuffer
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_CloudSky].getSampler(),
                .imageView = mResources_[textureHandle_CloudSky].getImageView(),
                .binding = 1,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler
            }
        };

        mAppProp.mMaterials.cloudSkyNoDepth = mResources_.addResource<clay::Material>(
            clay::Material(matConfig),
            "CloudSky"
        );
    }
    {
        // CloudSky with Depth
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_TextureDepth]
        };

        matConfig.bufferBindings = {
            {
                .buffer = mGraphicsContextDesktop_.mCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = vk::DescriptorType::eUniformBuffer
            }
        };
        matConfig.imageBindings = {
            {
                .sampler = mResources_[textureHandle_CloudSky].getSampler(),
                .imageView = mResources_[textureHandle_CloudSky].getImageView(),
                .binding = 1,
                .descriptorType = vk::DescriptorType::eCombinedImageSampler
            }
        };

        mAppProp.mMaterials.cloudSkyDepth = mResources_.addResource<clay::Material>(
            clay::Material(matConfig),
            "CloudSkyDepth"
        );
    }
    // Flat
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_Flat]
        };

        matConfig.bufferBindings = {
            {
                .buffer = mGraphicsContextDesktop_.mCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = vk::DescriptorType::eUniformBuffer
            }
        };

        mAppProp.mMaterials.flat = mResources_.addResource<clay::Material>(
            std::move(clay::Material(matConfig)),
            "Flat"
        );
    }
    // Animation material
    // Note: Bone buffer (binding=1) is NOT included in the shared material
    // Each animated entity will bind its own bone buffer at render time
    {
        clay::Material::MaterialConfig matConfig {
            .graphicsContext = *mpGraphicsContext_,
            .pipelineResource = mResources_[pipelineHandle_Animation]
        };

        matConfig.bufferBindings = {
            {
                .buffer = mGraphicsContextDesktop_.mCameraUniform_->mBuffer_,
                .size = sizeof(clay::BaseScene::CameraConstant),
                .binding = 0,
                .descriptorType = vk::DescriptorType::eUniformBuffer
            }
            // Binding 1 (bone transforms) intentionally omitted - handled per-entity
        };
        
        mAppProp.mMaterials.animation = mResources_.addResource<clay::Material>(
            std::move(clay::Material(matConfig)),
            "Animation3D"
        );
    }
    
    // Animated Mesh and Skeletal Animation
    {
        std::filesystem::path fbxPath = clay::Resources::getResourcePath() / "models/rpg_3d/walk_cycle.fbx";
        clay::SkeletalAnimation tempAnimation;
        std::unique_ptr<clay::AnimatedMesh> animatedMesh = clay::AnimatedMesh::loadFromFBX(
            *mpGraphicsContext_,
            fbxPath.string(),
            &tempAnimation
        );
        
        if (animatedMesh) {
            tempAnimation.setMesh(animatedMesh.get());
            tempAnimation.setLooping(true);
            
            // Add mesh to resources first
            mResources_.addResource<clay::AnimatedMesh>(std::move(*animatedMesh), "WalkCycleCharacter");
            
            // Update animation to point to the mesh in resources (after move)
            auto meshHandle = mResources_.getHandle<clay::AnimatedMesh>("WalkCycleCharacter");
            tempAnimation.setMesh(&mResources_[meshHandle]);
            
            // Now add animation to resources
            mResources_.addResource<clay::SkeletalAnimation>(std::move(tempAnimation), "WalkCycleAnimation");
        }
    }
    
    // Models
    {
        // Solid Sphere
        clay::Model model;
        model.addElement({
            mAppProp.mMeshes.sphere,
            mAppProp.mMaterials.solid,
            glm::mat4(1),
        });
        mAppProp.mModels.solidSphere = mResources_.addResource(std::move(model), "SolidSphere");
    }
    {
        // V Texture Sphere
        clay::Model model;
        model.addElement({
            mAppProp.mMeshes.sphere,
            mAppProp.mMaterials.vTexture,
            glm::mat4(1),
        });
        mAppProp.mModels.vSphere = mResources_.addResource(std::move(model), "VSphere");
    }
    {
        // Solid Plane
        clay::Model model;
        model.addElement({
            mAppProp.mMeshes.plane,
            mAppProp.mMaterials.solid,
            glm::mat4(1),
        });
        mAppProp.mModels.solidPlane = mResources_.addResource(std::move(model), "SolidPlane");
    }
    {
        // Solid Torus
        clay::Model model;
        model.addElement({
            mAppProp.mMeshes.torus,
            mAppProp.mMaterials.solid,
            glm::mat4(1),
        });
        mAppProp.mModels.solidTorus = mResources_.addResource(std::move(model), "SolidTorus");
    }
    {
        // Sun
        clay::Model model;
        model.addElement({
            mAppProp.mMeshes.sphere,
            mAppProp.mMaterials.sun,
            glm::mat4(1),
        });
        mAppProp.mModels.sun = mResources_.addResource(std::move(model), "Sun");
    }
    {
        // Moon
        clay::Model model;
        model.addElement({
            mAppProp.mMeshes.sphere,
            mAppProp.mMaterials.moon,
            glm::mat4(1),
        });
        mAppProp.mModels.moon = mResources_.addResource(std::move(model), "Moon");
    }
    {
        // Earth
        clay::Model model;
        model.addElement({
            mAppProp.mMeshes.torus,
            mAppProp.mMaterials.earth,
            glm::mat4(1),
        });
        mAppProp.mModels.earth = mResources_.addResource(std::move(model), "Earth");
    }
    {
        // Solid Circle
        clay::Model model;
        model.addElement({
            mAppProp.mMeshes.planeCircle,
            mAppProp.mMaterials.solid,
            glm::mat4(1),
        });
        mAppProp.mModels.solidCircle = mResources_.addResource(std::move(model), "SolidCircle");
    }
    {
        // Cube
        clay::Model model;
        model.addElement({
            mAppProp.mMeshes.cube,
            mAppProp.mMaterials.solid,
            glm::mat4(1),
        });
        mAppProp.mModels.cube = mResources_.addResource(std::move(model), "Cube");
    }
    // human
    {
        clay::Model model;
        model.addElement({
            mAppProp.mMeshes.human,
            mAppProp.mMaterials.flat,
            glm::mat4(1),
        });
        mAppProp.mModels.human = mResources_.addResource(std::move(model), "Human");
    }
    {
        // sky plane
        clay::Model model;
        model.addElement({
            mAppProp.mMeshes.plane,
            mAppProp.mMaterials.cloudSkyDepth,
            glm::mat4(1),
        });
        mAppProp.mModels.skyPlane = mResources_.addResource(std::move(model), "SkyPlane");
    }
    
    // Animations
    {
        // Player walk down
        clay::Animation2D walkDown;
        walkDown.materialHandle = mAppProp.mMaterials.spriteSheet;
        walkDown.frames = {
            glm::vec4(3 * 16.0f / 512.0f, 0, 16.0f / 512.0f, 16.0f / 512.0f),
            glm::vec4(4 * 16.0f / 512.0f, 0, 16.0f / 512.0f, 16.0f / 512.0f),
            glm::vec4(3 * 16.0f / 512.0f, 0, 16.0f / 512.0f, 16.0f / 512.0f),
            glm::vec4(5 * 16.0f / 512.0f, 0, 16.0f / 512.0f, 16.0f / 512.0f)
        };
        walkDown.frameDuration = 1.0f / 4.0f;
        walkDown.loop = true;
        mResources_.addResource(std::move(walkDown), "PlayerWalkDown");
    }
    {
        // Player walk left
        clay::Animation2D walkLeft;
        walkLeft.materialHandle = mAppProp.mMaterials.spriteSheet;
        walkLeft.frames = {
            glm::vec4(6 * 16.0f / 512.0f, 0, 16.0f / 512.0f, 16.0f / 512.0f),
            glm::vec4(7 * 16.0f / 512.0f, 0, 16.0f / 512.0f, 16.0f / 512.0f),
            glm::vec4(6 * 16.0f / 512.0f, 0, 16.0f / 512.0f, 16.0f / 512.0f),
            glm::vec4(8 * 16.0f / 512.0f, 0, 16.0f / 512.0f, 16.0f / 512.0f)
        };
        walkLeft.frameDuration = 1.0f / 4.0f;
        walkLeft.loop = true;
        mResources_.addResource(std::move(walkLeft), "PlayerWalkLeft");
    }
    {
        // Player walk right
        clay::Animation2D walkRight;
        walkRight.materialHandle = mAppProp.mMaterials.spriteSheet;
        walkRight.frames = {
            glm::vec4(9 * 16.0f / 512.0f, 0, 16.0f / 512.0f, 16.0f / 512.0f),
            glm::vec4(10 * 16.0f / 512.0f, 0, 16.0f / 512.0f, 16.0f / 512.0f),
            glm::vec4(9 * 16.0f / 512.0f, 0, 16.0f / 512.0f, 16.0f / 512.0f),
            glm::vec4(11 * 16.0f / 512.0f, 0, 16.0f / 512.0f, 16.0f / 512.0f)
        };
        walkRight.frameDuration = 1.0f / 4.0f;
        walkRight.loop = true;
        mResources_.addResource(std::move(walkRight), "PlayerWalkRight");
    }
    {
        // Player walk up
        clay::Animation2D walkUp;
        walkUp.materialHandle = mAppProp.mMaterials.spriteSheet;
        walkUp.frames = {
            glm::vec4(12 * 16.0f / 512.0f, 0, 16.0f / 512.0f, 16.0f / 512.0f),
            glm::vec4(13 * 16.0f / 512.0f, 0, 16.0f / 512.0f, 16.0f / 512.0f),
            glm::vec4(12 * 16.0f / 512.0f, 0, 16.0f / 512.0f, 16.0f / 512.0f),
            glm::vec4(14 * 16.0f / 512.0f, 0, 16.0f / 512.0f, 16.0f / 512.0f)
        };
        walkUp.frameDuration = 1.0f / 4.0f;
        walkUp.loop = true;
        mResources_.addResource(std::move(walkUp), "PlayerWalkUp");
    }
}