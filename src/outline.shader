#shader vertex
#version 330 core
layout(location = 0) in vec4 position;
layout(location = 1) in vec4 normal;

uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

void main()
{
    // Push vertices outward along normal
    vec4 pushedPos = position + normal * 0.02f;
    gl_Position = u_projection * u_view * u_model * pushedPos;
}

#shader fragment
#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(0.0, 0.0, 0.0, 1.0);  // Black outline
}