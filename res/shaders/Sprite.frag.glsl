#version 450

layout(binding = 1) uniform sampler2D texSampler;
layout(location = 0) in vec2 fragTexCoord;
layout(location = 0) out vec4 outColor;

vec3 magenta1 = vec3(1.0, 0.0, 1.0);
vec3 magenta2 = vec3(0.60, 0.0, 0.60); //GL_SRGB of (204,0,204)
float epsilon = 0.01;

layout(push_constant) uniform PushConstants {
    mat4 model;
    vec4 color;
    vec4 offsets; // x,y,width,height
} pc;

void main() {
    vec2 flippedTexCoords = vec2(fragTexCoord.x, 1.0 - fragTexCoord.y);

    // Apply sprite sheet UV transformation
    vec2 spriteUV = pc.offsets.xy + flippedTexCoords * pc.offsets.zw;

    vec4 sampledColor = texture(texSampler, spriteUV);

    if (length(sampledColor.rgb - magenta1) < epsilon || length(sampledColor.rgb - magenta2) < epsilon) {
        //outColor = vec4(0.0, 0.0, 0.0, 0.0); // background color TODO find best way to be work when rendering behind text
        discard;
    } else {
        outColor = sampledColor * pc.color;
    }
}