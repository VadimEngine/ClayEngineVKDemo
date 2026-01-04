#version 450

layout(binding = 1) uniform sampler2D texSampler;

// Inputs from vertex shader
layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 fragColor;
layout(location = 2) in vec4 fragUvOffsets;

layout(location = 0) out vec4 outColor;

vec3 magenta1 = vec3(1.0, 0.0, 1.0);
vec3 magenta2 = vec3(0.60, 0.0, 0.60); //GL_SRGB of (204,0,204)
float epsilon = 0.01;

void main() {
    vec2 flippedTexCoords = vec2(fragTexCoord.x, 1.0 - fragTexCoord.y);

    // Apply sprite sheet UV transformation using per-instance offsets
    vec2 spriteUV = fragUvOffsets.xy + flippedTexCoords * fragUvOffsets.zw;

    vec4 sampledColor = texture(texSampler, spriteUV);

    // Discard magenta pixels (transparency)
    if (length(sampledColor.rgb - magenta1) < epsilon || length(sampledColor.rgb - magenta2) < epsilon) {
        discard;
    } else {
        outColor = sampledColor * fragColor;
    }
}
