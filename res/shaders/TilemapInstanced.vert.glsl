#version 450

// Per-vertex attributes (shared by all instances)
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;

// Uniform buffer (camera matrices)
layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

// Storage buffer for per-instance data
struct TileInstanceData {
    vec4 position; // xyz = position, w = unused
    vec4 uvOffsets; // x,y = offset, z,w = width,height
    vec4 color;    // rgba tint
};

layout(std140, binding = 2) readonly buffer InstanceBuffer {
    TileInstanceData instances[];
} instanceBuffer;

// Outputs to fragment shader
layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec4 fragColor;
layout(location = 2) out vec4 fragUvOffsets;

void main() {
    // Get instance data
    TileInstanceData instance = instanceBuffer.instances[gl_InstanceIndex];
    
    // Calculate tile transform - scale vertices by tile size and offset by position
    float tileScale = instance.position.w;
    vec3 scaledPosition = aPosition * tileScale;
    vec3 worldPos = scaledPosition + instance.position.xyz;
    
    gl_Position = ubo.proj * ubo.view * vec4(worldPos, 1.0);
    
    // Pass data to fragment shader
    fragTexCoord = aTexCoord;
    fragColor = instance.color;
    fragUvOffsets = instance.uvOffsets;
}
