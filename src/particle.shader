#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform vec3 u_cameraRight;
uniform vec3 u_cameraUp;
uniform vec3 u_particlePosition;
uniform float u_particleSize;
uniform float u_opacity;

out vec2 v_texCoord;
out float v_opacity;

void main()
{
    v_texCoord = aTexCoord;
    v_opacity = u_opacity;
    
    // Billboarding: position particle relative to camera
    vec3 pos = u_particlePosition;
    pos += u_cameraRight * aPos.x * u_particleSize;
    pos += u_cameraUp * aPos.y * u_particleSize;
    
    gl_Position = u_projection * u_view * vec4(pos, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 v_texCoord;
in float v_opacity;

uniform sampler2D u_particleTexture;

void main()
{
    vec4 color = texture(u_particleTexture, v_texCoord);
    
    // Use alpha from texture multiplied by particle opacity
    float alpha = color.a * v_opacity;
    
    // Discard fully transparent fragments
    if (alpha < 0.05) discard;
    
    FragColor = vec4(color.rgb, alpha);
}