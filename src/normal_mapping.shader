#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec4 color;
layout(location = 3) in vec2 texCoord;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 v_normal;
out vec3 v_fragPos;
out vec2 v_texCoord;

void main()
{
    v_normal = mat3(transpose(inverse(u_model))) * normal.xyz;
    v_fragPos = (u_model * position).xyz;
    v_texCoord = texCoord;
    
    gl_Position = u_projection * u_view * u_model * position;
}

#shader fragment
#version 330 core

in vec3 v_normal;
in vec3 v_fragPos;
in vec2 v_texCoord;

uniform vec3 u_lightPos;
uniform vec3 u_viewPos;
uniform float u_useOverrideColor;
uniform vec3 u_overrideColor;
uniform sampler2D u_diffuseTexture;
uniform sampler2D u_normalTexture;

out vec4 FragColor;

void main()
{
    vec3 diffuseColor;
    
    if (u_useOverrideColor > 0.5) {
        diffuseColor = u_overrideColor;
    } else {
        diffuseColor = texture(u_diffuseTexture, v_texCoord).rgb;
    }
    
    // Sample and convert normal map
    vec3 normalMap = texture(u_normalTexture, v_texCoord).rgb;
    vec3 normal = normalize(normalMap * 2.0 - 1.0);
    
    // Combine vertex normal with normal map
    vec3 worldNormal = normalize(v_normal + normal * 0.5);
    
    // Lighting
    vec3 lightDir = normalize(u_lightPos - v_fragPos);
    float distance = length(u_lightPos - v_fragPos);
    
    float k_c = 1.0;
    float k_l = 0.09;
    float k_q = 0.032;
    float attenuation = 0.7;//1.0 / (k_c + k_l * distance + k_q * distance * distance);
    
    float ambientIntensity = 0.3;
    vec3 ambient = ambientIntensity * diffuseColor;
    
    float diff = max(dot(worldNormal, lightDir), 0.0);
    vec3 diffuse = diff * diffuseColor * attenuation;
    
    vec3 viewDir = normalize(u_viewPos - v_fragPos);
    vec3 reflectDir = reflect(-lightDir, worldNormal);
    float shininess = 32.0;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = spec * vec3(0.8) * attenuation;
    
    vec3 finalColor = ambient + diffuse + specular;
    FragColor = vec4(finalColor, 1.0);
}