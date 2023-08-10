#pragma once
#include <vector>
#include "gameObject.h"
#include <limits>

void generateAnimal(GLuint VAO, int vertCount, GLuint texture, gameObject& animal, gameObject& neck_joint, gameObject& leftHip_joint, gameObject& rightHip_joint, gameObject& leftShoulder_joint, gameObject& rightShoulder_joint);
double randomFloat(float max, float min);