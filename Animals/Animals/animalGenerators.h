#pragma once
#include <vector>
#include "gameObject.h"
#include <limits>

void generateAnimal(GLuint VAO, int vertCount, std::vector<GLuint>* textures, gameObject& animal, gameObject& neck_joint, gameObject& leftHip_joint, gameObject& rightHip_joint, gameObject& leftShoulder_joint, gameObject& rightShoulder_joint);
double randomFloat(float max, float min);

class bipedalAnimal {

public:
    gameObject animalCore;
    gameObject leftHip_joint;
    gameObject rightHip_joint;
    gameObject neck_joint;
    gameObject leftShoulder_joint;
    gameObject rightShoulder_joint;
    double angle;

    bipedalAnimal(GLuint VAO, int vertCount,std::vector<GLuint>* textures) : angle{ randomFloat(1.0f, 0.0f) }
    {
        generateAnimal(VAO, vertCount, textures, animalCore, neck_joint, leftHip_joint, rightHip_joint, leftShoulder_joint, rightShoulder_joint);
        time_t t;
        std::srand((unsigned)time(&t) + rand());
        animalCore.setTransformPosition(randomFloat(100, -100), animalCore.getTransformPosition().y, randomFloat(100, 100));
        std::srand((unsigned)time(&t) + rand());
        animalCore.setTransformRotation(0, randomFloat(360,0), 0);
    }
    void animate(); 
    void reGenerate(glm::vec3 cameraPosition, GLuint VAO, int vertCount, std::vector<GLuint>* textures);
};