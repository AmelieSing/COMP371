#include <iostream>
#include <vector>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/gtc/type_ptr.hpp>
#include "Model.h"
//#include "Renderer.h"
#include "World.h"

using namespace std;
using namespace glm;



Model::Model(int vao, int vertexCount, int shaderProgram)
    :
    vao(vao),
    vertexCount(vertexCount),
    shaderProgram(shaderProgram),
    position(0.0f, 0.0f, 0.0f),
    scaling(1.0f, 1.0f, 1.0f),
    rotationAxis(0.0f, 0.0f, 1.0f),
    rotationAngleInDegrees(0.0f),
    velocity(0.0f),
    angularAxis(0, 0, 1),
    angularVelocityInDegrees(0.0f) {}

        Model::~Model() {}

        void Model::Update(float dt) {
            position += dt * velocity;
            rotationAxis = glm::mix(rotationAxis, angularAxis, dt);
            rotationAngleInDegrees += dt * angularVelocityInDegrees;
            Accelerate(vec3(0.0f, 1.2f, 0.0f), dt);
        }

        void Model::Draw(int textureID, int shaderProgram) {
            // TODO 3.5 - use the correct shader
       //     Renderer::SetShader(ShaderType::SHADER_SOLID_COLOR);
            glUseProgram(shaderProgram);

            GLuint worldMatrixLocation =
                glGetUniformLocation(shaderProgram, "WorldTransform");
            mat4 worldMatrix = GetWorldMatrix();
            glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, vertexCount);
            glBindVertexArray(0);
        }

        glm::mat4 Model::GetWorldMatrix() const {
            mat4 t = translate(mat4(1.0f), position);
            mat4 r =
                rotate(mat4(1.0f), glm::radians(rotationAngleInDegrees), rotationAxis);
            mat4 s = scale(mat4(1.0f), scaling);

            mat4 worldMatrix = t * r * s;

            return worldMatrix;
        }

        GLuint Model::getShaderProgram()
        {
            return shaderProgram;
        }

        void Model::Accelerate(glm::vec3 acceleration, float delta) {
            velocity += acceleration * delta;
        }

        void Model::SetPosition(glm::vec3 position) { this->position = position; }

        void Model::SetScaling(glm::vec3 scaling) { this->scaling = scaling; }

        void Model::SetRotation(glm::vec3 axis, float angleDegrees) {
            rotationAxis = axis;
            rotationAngleInDegrees = angleDegrees;
        }

        void Model::SetVelocity(glm::vec3 velocity) { this->velocity = velocity; }

        GLuint Model::createVAO(int& vertexCount, std::vector<glm::vec3> positions,
            std::vector<glm::vec3> normals,
            std::vector<glm::vec3> colors,
            std::vector<glm::vec2> uvs) {
            GLuint vao;
            GLuint vbo[4];

            glGenVertexArrays(1, &vao);
            glBindVertexArray(vao);

            // Upload Vertex Buffers to the GPU
            glGenBuffers(4, vbo);  // 4, for positions, normals, colors, uvs

            // Setup Positions
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);  // positions
            glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3),
                positions.data(), GL_STATIC_DRAW);

            glVertexAttribPointer(0,  // attribute. No particular reason for 0, but must
                // match the layout in the shader.
                3,  // size
                GL_FLOAT,      // type
                GL_FALSE,      // normalized?
                sizeof(vec3),  // stride
                (void*)0       // offset
            );
            glEnableVertexAttribArray(0);

            // Setup normals
            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);  // normal
            glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(vec3), normals.data(),
                GL_STATIC_DRAW);

            // 2nd attribute buffer : vertex normal
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
            glEnableVertexAttribArray(1);

            // Setup Colors
            glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);  // colors
            glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), colors.data(),
                GL_STATIC_DRAW);

            // 3rd attribute buffer : vertex color
            glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
            glEnableVertexAttribArray(2);

            // Setup UVs
            glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);  // UVs
            glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), uvs.data(),
                GL_STATIC_DRAW);

            // 3rd attribute buffer : vertex color
            glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);
            glEnableVertexAttribArray(3);

            glBindBuffer(GL_ARRAY_BUFFER, 0);  // unbind
            glBindVertexArray(0);

            vertexCount = positions.size();
            return vao;
        }
   