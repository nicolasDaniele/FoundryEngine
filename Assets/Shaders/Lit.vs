#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;

struct Light
{
    vec3  position;   float type;
    vec3  direction;  float intensity;
    vec3  color;      float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

layout(std140) uniform FrameData
{
    mat4 vp;
    vec3 viewPos;
    int  lightCount;
    // The light array is not used in the vertex shader, but must still be
    // declared here so this block's layout matches the fragment shader's.
    Light lights[MAX_LIGHTS];
};

uniform mat4 model;
uniform mat3 normalMatrix;

out vec3 FragPos;
out vec3 Normal;
out vec3 VertexColor;

void main()
{
    vec4 worldPos = model * vec4(aPos, 1.0);
    FragPos = worldPos.xyz;
    Normal = normalize(normalMatrix * aNormal);
    VertexColor = aColor;

    gl_Position = vp * worldPos;
}
