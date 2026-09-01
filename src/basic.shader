#shader vertex
#version 330 core

layout(location = 0) in vec4 position; //vertex position in model space
layout(location = 1) in vec4 normal; //surface normal vector @ vertex 
layout(location = 2) in vec4 color; //color @ vertex

uniform mat4 u_model;  //model matrix, transform from model space to world space
uniform mat4 u_view;  //view matrix, transform from world to view space
uniform mat4 u_projection;  //projection matrix, transform from view to screen

out vec3 v_normal;  //passing normal to fragment shader
out vec3 v_fragPos;  //passing fragment position to fragment shader
out vec3 v_color;   //passing color to fragment shader

void main()
{
    //transforming the normal to world space
    v_normal = mat3(transpose(inverse(u_model))) * normal.xyz;
    v_fragPos = (u_model * position).xyz; //
    v_color = color.xyz;

    gl_Position = u_projection * u_view * u_model * position;
}



#shader fragment
#version 330 core

//from vertex shader
in vec3 v_normal;
in vec3 v_fragPos;
in vec3 v_color;

uniform vec3 u_lightPos;       //light position
uniform vec3 u_viewPos;        //camera position
uniform float u_useOverrideColor;  
uniform vec3 u_overrideColor;      //color to override with

out vec4 FragColor;

void main()
{
    vec3 baseColor = v_color;
    if(u_useOverrideColor > 0.5)
        baseColor = u_overrideColor;

    //normalize the normal and calculate light direction
    vec3 norm = normalize(v_normal);
    vec3 lightDir = u_lightPos - v_fragPos;
    float distance = length(lightDir);
    lightDir = normalize(lightDir);

    //attenuation calculation
    float k_c = 1.0;       //constant term, prevents division by 0
    float k_l = 0.09;      //linear term, light diminishes linearly
    float k_q = 0.032;     //quadratic term, light diminishes, distance^2
    //float attenuation = 1.0 / (k_c + k_l * distance + k_q * distance * distance);
    float attenuation = 0.7;//1.0 / (k_c + k_l * distance + k_q * distance * distance);

    //ambient component
    float ambientIntensity = 0.2; 
    vec3 ambient = ambientIntensity * baseColor;

    //diffuse component
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * baseColor * attenuation;

    //specular component
    vec3 viewDir = normalize(u_viewPos - v_fragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  //reflection vector from light direction
    float shininess = 32.0;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);  //measure angle between view and reflection
    vec3 specularColor = vec3(0.8);
    vec3 specular = spec * specularColor * attenuation;

    //final color calculation
    vec3 finalColor = ambient + diffuse + specular;

    FragColor = vec4(clamp(finalColor, 0.0, 1.0), 1.0);
}
