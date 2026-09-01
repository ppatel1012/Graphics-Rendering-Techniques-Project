#pragma once
#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <string>
#include "Shader.h"

class Model {
private:
    GLuint vao = 0;
    GLuint vbo = 0;
    int vertexCount = 0;
    glm::mat4 modelMatrix;

    // Helper to upload 12-float stride data (Pos4, Norm4, Col4)
    void uploadToGPU(const std::vector<float>& data);


public:
    Model();
    ~Model();

    // OPTION A: Load from File (Uses TinyObjLoader)
    void init(const std::string& filepath);

    // OPTION B: Load from Hardcoded Array
    void init(float* data, int count);

    // STUDENT TASK: Apply transforms
    void update(glm::vec3 position, glm::vec3 rotation, float scale);
	
	void update(glm::mat4 modelMat);

	void draw(Shader* shader);

    void initWithUV(const std::vector<float>& data);  // For models with UVs
};
