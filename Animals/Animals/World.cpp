#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <limits>

#include "World.h"
#include "Quad.h"
#include <iostream>
using namespace std;
using namespace glm;

World::World() : spawnTimer(0) {
    gravity = 9.807f;
    friction = 0.0f;
}

World::~World() {
    // Models
    for (list<Quad*>::iterator it = models.begin(); it != models.end(); ++it) {
        delete* it;
    }

    models.clear();

}

void World::Update(float dt) {

    spawnTimer += dt;
    if (spawnTimer > SPAWN_PERIOD) {
        int vertexCount;
        int vao = Quad::makeQuadVAO(vertexCount);
        Quad* model = new Quad(vao, vertexCount, 0);

        // TODO 1 - Give the particles some motion when created
         float angle = GetRandomFloat(0, 360);
         vec4 randomSidewaysVelocity(0.5, 2, 0, 0);
         randomSidewaysVelocity = glm::rotate(mat4(1), glm::radians(angle),
         vec3(0,1,0)) * randomSidewaysVelocity;
         model->SetVelocity(vec3(randomSidewaysVelocity));

        models.push_back(model);  // spawn
        spawnTimer -= SPAWN_PERIOD;
    }

    // TODO 4 - calculate billboard rotation parameters
    // vec3 originalAxis(0, 0, 1);
    // vec3 cameraLookAt(-GetCurrentCamera()->GetLookAt());
    // cameraLookAt.y = 0; //project onto xz plane
    // cameraLookAt.x *= -1;
    // cameraLookAt = normalize(cameraLookAt);

    // vec3 billboardRotationAxis(vec3(0,1,0));

    // float billboardRotation = glm::acos(-1 * dot(originalAxis, cameraLookAt)) *
    // 360 / (2 * pi<float>());
    //
    // if (cameraLookAt.x > 0) {
    //	billboardRotation = 360 - billboardRotation;
    //}

    // Update models
    list<Quad*>::iterator it = models.begin();
    while (it != models.end()) {
        // TODO 4 - Set the rotation
        //(*it)->SetRotation(billboardRotationAxis, billboardRotation);
        (*it)->Update(dt);

        if ((*it)->expired()) {
            it = models.erase(it);
        }
        else {
            ++it;
        }
    }
}

void World::Draw(int textureID, int shaderProgram) {
   // Renderer::BeginFrame();

    // Set shader to use
    glUseProgram(shaderProgram);

    // This looks for the MVP Uniform variable in the Vertex Program
  //  GLuint VPMatrixLocation = glGetUniformLocation(Renderer::GetShaderProgramID(),
  //      "ViewProjectionTransform");

    // Send the view projection constants to the shader
 //   mat4 VP = camera[currentCamera]->GetViewProjectionMatrix();
  //  glUniformMatrix4fv(VPMatrixLocation, 1, GL_FALSE, &VP[0][0]);

    // Draw models

    // TODO 3.4 Temporarily disable depth test for transparency
    // Disable depth test
     glDisable(GL_DEPTH_TEST);
    for (list<Quad*>::iterator it = models.begin(); it != models.end(); ++it) {
        (*it)->Draw(textureID, shaderProgram);
    }
    // TODO 3.4 - Make sure to re-enable it when done!!!
    // Re-enable depth test
     glEnable(GL_DEPTH_TEST);

  //  Renderer::EndFrame();
}

float GetRandomFloat(float min, float max) {
    float value = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

    return min + value * (max - min);
}
