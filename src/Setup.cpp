#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>

#include "Shader.h"
#include "Camera.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(640, 480, "Camera Example", NULL, NULL);
    if (!window)
        return -1;

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return -1;

    Camera camera(window, 90.0f, 3.0f);

    // Geometry
    float verts[] = {
        -0.5f,-0.5f,0,1, 0,0,1,0, 1,0,0,1,
         0.5f,-0.5f,0,1, 0,0,1,0, 0,1,0,1,
        -0.5f, 0.5f,0,1, 0,0,1,0, 0,0,1,1
    };

    GLuint VAO, VBO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1,&VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(verts),verts,GL_STATIC_DRAW);

    glVertexAttribPointer(0,4,GL_FLOAT,GL_FALSE,12*sizeof(float),(void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1,4,GL_FLOAT,GL_FALSE,12*sizeof(float),(void*)(4*sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2,4,GL_FLOAT,GL_FALSE,12*sizeof(float),(void*)(8*sizeof(float)));
    glEnableVertexAttribArray(2);

    Shader shader("src/basic.shader");

    glEnable(GL_DEPTH_TEST);

    glm::mat4 model = glm::mat4(1.0f);

    float lastTime = (float)glfwGetTime();

    while (!glfwWindowShouldClose(window))
    {
        float time = (float)glfwGetTime();
        float deltaTime = time - lastTime;
        lastTime = time;

        glfwPollEvents();

        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Bind();
        shader.SetUniform4m("u_model", model);

        camera.update(deltaTime);
        camera.sendToShader(&shader);

        glBindVertexArray(VAO);


		shader.Bind();
		camera.sendToShader(&shader);
	//	shader.SetUniform3f("u_viewPos", camera.position);
		glm::vec3 lightPos(2.0f, 4.0f, 2.0f);
		lightPos.x = sin(glfwGetTime()) * 2.0f;
		lightPos.y = cos(glfwGetTime()) * 2.0f;

		shader.SetUniform3f("u_lightPos", lightPos);
		shader.SetUniform3f("u_viewPos", glm::vec3(1.0f, 1.0f, 1.0f));

	//	shader.SetUniform3f("u_lightPos", lightPos);


        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}
