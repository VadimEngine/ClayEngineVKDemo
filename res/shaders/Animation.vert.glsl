#version 450

// Maximum number of bones supported
#define MAX_BONES 100
#define MAX_BONE_INFLUENCE 4

layout(binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
} ubo;

// Bone transformation matrices
layout(binding = 1) uniform BoneTransforms {
    mat4 bones[MAX_BONES];
} boneTransforms;

layout(push_constant) uniform PushConstants {
    mat4 model;
    vec4 color;
} pc;

// Vertex attributes
layout(location = 0) in vec3 aPosition;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in ivec4 aBoneIds;      // Bone indices (up to 4 per vertex)
layout(location = 4) in vec4 aBoneWeights;   // Bone weights (up to 4 per vertex)

// Output to fragment shader
layout(location = 0) out vec3 FragPos;
layout(location = 1) out vec3 Normal;
layout(location = 2) out vec2 TexCoords;

void main() {
    // Calculate skinned position and normal
    vec4 totalPosition = vec4(0.0);
    vec3 totalNormal = vec3(0.0);
    float totalWeight = 0.0;
    
    for(int i = 0; i < MAX_BONE_INFLUENCE; i++) {
        if(aBoneIds[i] == -1) 
            continue;
            
        if(aBoneIds[i] >= MAX_BONES) {
            totalPosition = vec4(aPosition, 1.0);
            totalNormal = aNormal;
            totalWeight = 1.0;
            break;
        }
        
        float weight = aBoneWeights[i];
        if(weight > 0.0) {
            vec4 localPosition = boneTransforms.bones[aBoneIds[i]] * vec4(aPosition, 1.0);
            totalPosition += localPosition * weight;
            
            vec3 localNormal = mat3(boneTransforms.bones[aBoneIds[i]]) * aNormal;
            totalNormal += localNormal * weight;
            
            totalWeight += weight;
        }
    }
    
    // If no bone influences, use original position
    if(totalWeight == 0.0) {
        totalPosition = vec4(aPosition, 1.0);
        totalNormal = aNormal;
    } else {
        // Normalize by total weight to ensure proper blending
        totalPosition /= totalWeight;
        totalNormal = normalize(totalNormal);
    }
    
    // Transform to world space
    mat4 modelMatrix = pc.model;
    FragPos = vec3(modelMatrix * totalPosition);
    Normal = normalize(mat3(transpose(inverse(modelMatrix))) * totalNormal);
    TexCoords = aTexCoord;
    
    // Final position
    gl_Position = ubo.proj * ubo.view * vec4(FragPos, 1.0);
}
