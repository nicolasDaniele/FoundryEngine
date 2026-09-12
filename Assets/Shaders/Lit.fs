#version 330 core
// MAX_LIGHTS is injected automatically by ShaderLoader (see
// EngineInterfaces/GraphicsTypes.h) - no need to define it here.

struct Light
{
    vec3  position;   float type; // type: 0 = directional, 1 = point, 2 = spot
    vec3  direction;  float intensity;
    vec3  color;      float constant;
    float linear;
    float quadratic;
    float cutOff;
    float outerCutOff;
};

layout(std140) uniform FrameData
{
    mat4  vp;
    vec3  viewPos;
    int   lightCount;
    Light lights[MAX_LIGHTS];
};

uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;
uniform float shininess;

in vec3 FragPos;
in vec3 Normal;
in vec3 VertexColor;

out vec4 FragColor;

vec3 ComputeLight(Light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir;
    float attenuation = 1.0;

    if (light.type < 0.5) // directional
    {
        lightDir = normalize(-light.direction);
    }
    else
    {
        vec3 toLight = light.position - FragPos;
        float dist = length(toLight);
        lightDir = toLight / dist;
        attenuation = 1.0 / (light.constant + light.linear * dist + light.quadratic * dist * dist);

        if (light.type > 1.5) // spot
        {
            float theta = dot(lightDir, normalize(-light.direction));
            float epsilon = light.cutOff - light.outerCutOff;
            attenuation *= clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
        }
    }

    vec3 ambient = ambientStrength * light.color;

    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * light.color;

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * light.color;

    return (ambient + diffuse + specular) * light.intensity * attenuation;
}

void main()
{
    vec3 normal  = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = vec3(0.0);
    for (int i = 0; i < lightCount; ++i)
        result += ComputeLight(lights[i], normal, viewDir);

    FragColor = vec4(result * VertexColor, 1.0);
}
