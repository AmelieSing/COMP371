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
  
    // Set shader to use
    glUseProgram(shaderProgram);

    // Draw models
    // Disable depth test
     glDisable(GL_DEPTH_TEST);
    for (list<Quad*>::iterator it = models.begin(); it != models.end(); ++it) {
        (*it)->Draw(textureID, shaderProgram);
    }
    // Re-enable depth test
     glEnable(GL_DEPTH_TEST);

}

float GetRandomFloat(float min, float max) {
    float value = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

    return min + value * (max - min);
}
