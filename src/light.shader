#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec3 u_sunPos;
uniform float u_size;

void main()
{
    TexCoord = aTexCoord;
    
    // Billboard: always face camera
    vec3 cameraRight = vec3(u_view[0][0], u_view[1][0], u_view[2][0]);
    vec3 cameraUp = vec3(u_view[0][1], u_view[1][1], u_view[2][1]);
    
    vec3 pos = u_sunPos;
    pos += cameraRight * aPos.x * u_size;
    pos += cameraUp * aPos.y * u_size;
    
    gl_Position = u_projection * u_view * vec4(pos, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform float u_time;

void main()
{
    // Center the UV coordinates 
    vec2 uv = TexCoord - 0.5;
    float radius = length(uv) * 2.0;  // 0 at center, 1 at edge
    
    // removepixels outside the circle!
    if (radius > 1.0) discard;
    
    // Create concentric rings
    vec3 bandColor;
    
    if (radius < 0.2) {
        bandColor = vec3(1.0, 1.0, 0.9);
    } else if (radius < 0.35) {
        bandColor = vec3(1.0, 0.95, 0.6);
    } else if (radius < 0.5) {
        bandColor = vec3(1.0, 0.85, 0.4);
    } else if (radius < 0.65) {
        bandColor = vec3(1.0, 0.7, 0.25);
    } else if (radius < 0.8) {
        bandColor = vec3(1.0, 0.5, 0.15);
    } else {
        bandColor = vec3(0.9, 0.3, 0.05);
    }
    
    // Fade edges for smoother circle
    float alpha = 1.0;
    if (radius > 0.9) {
        alpha = (1.0 - (radius - 0.9) / 0.1);
    }
    
    float pulse = 0.85 + sin(u_time * 4.0) * 0.15;
    
    FragColor = vec4(bandColor * pulse, alpha);
}