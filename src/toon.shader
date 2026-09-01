#shader vertex
#version 330 core

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;
layout(location = 2) in vec4 color;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec3 v_normal;
out vec3 v_fragPos;
out vec4 v_color;

void main()
{
    v_normal = mat3(transpose(inverse(u_model))) * normal.xyz;
    v_fragPos = (u_model * position).xyz;
    v_color = color;
    gl_Position = u_projection * u_view * u_model * position;
}

#shader fragment
#version 330 core

in vec3 v_normal;
in vec3 v_fragPos;
in vec4 v_color;
uniform float u_lightIntensity; 
uniform vec3 u_lightPos;
uniform vec3 u_viewPos;
uniform float u_useOverrideColor;
uniform vec3 u_overrideColor;

out vec4 FragColor;

void main()
{
    vec3 baseColor;
    if (u_useOverrideColor > 0.5) {
        baseColor = u_overrideColor;
    } else {
        baseColor = v_color.rgb;
    }
    
    vec3 norm = normalize(v_normal);
    vec3 lightDir = normalize(u_lightPos - v_fragPos);
    
    // Calculate diffuse intensity
    float diff = max(dot(norm, lightDir), 0.0);
    
    
    //toon shading: quantize diffuse into bands
    vec3 brightColor = baseColor;                    // Normal color in light
    vec3 midColor = baseColor * 0.6;                 // Darker midtone  
    vec3 darkColor = baseColor * 0.2;                // Very dark in shadow
    vec3 shadowColor = baseColor * 0.05;             // Almost black
    
    vec3 diffuseColor;
    if (diff > 0.66)         // Bright band (top 33%)
        diffuseColor = brightColor;
    else if (diff > 0.33)    // Medium band (middle 33%)
        diffuseColor = midColor;
    else                     // Dark band (bottom 33%)
        diffuseColor = darkColor;
    
    // Ambient component
    float ambientStrength = 0.15;
    vec3 ambient = ambientStrength * baseColor;
    
    // Toon specular 
    vec3 viewDir = normalize(u_viewPos - v_fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    
    // Quantize specular
    float specularLevel = spec > 0.3 ? 0.8 : 0.0;
    vec3 specular = specularLevel * vec3(1.0);
    vec3 finalColor = ambient + diffuseColor + specular;
    finalColor = finalColor * u_lightIntensity;  // Apply intensity
    FragColor = vec4(finalColor, 1.0);
    
    // vec3 finalColor = ambient + diffuseColor + specular;
    // FragColor = vec4(finalColor, 1.0);
}