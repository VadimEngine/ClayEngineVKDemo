#version 450

layout(push_constant) uniform PushConstants {
    mat4 model;
    vec4 color;
} pc;

// Input from vertex shader
layout(location = 0) in vec3 FragPos;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec2 TexCoords;

// Output color
layout(location = 0) out vec4 outColor;

void main() {
    vec3 lightDir = normalize(vec3(0.0, 1.0, 0.0));

    vec3 normal = normalize(Normal);

    // Simple Lambertian shading (diffuse only)
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 color = pc.color.rgb * diff + pc.color.rgb * 0.2; // Adding some ambient light
    
    outColor = vec4(color, 1.0);
}
