#shader vertex
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 TexCoord;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    gl_Position = u_projection * u_view * u_model * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}

#shader fragment
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D u_portalTexture;
uniform float u_time;  // Add this if you want time-based effects

void main()
{
    vec4 color = texture(u_portalTexture, TexCoord);
    
    // Optional: Add a subtle pulsing border
    float glow = 0.8 + sin(u_time * 5.0) * 0.2;
    
    // Check if we're at the edge of the texture (for border)
    if (TexCoord.x < 0.05 || TexCoord.x > 0.95 || TexCoord.y < 0.05 || TexCoord.y > 0.95) {
        // Golden border with pulsing glow
        FragColor = vec4(0.8, 0.6, 0.2, 1.0) * glow;
    } else {
        // Show the portal texture
        FragColor = color;
    }
}