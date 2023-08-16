#pragma once
#include <vector>
#include "gameObject.h"
#include <limits>

void generateFrog(GLuint vaSphere, int vertCount, std::vector<GLuint>* textures, gameObject& animal, 
	gameObject& leftHip_joint, gameObject& rightHip_joint, gameObject& leftKnee_joint, gameObject& rightKnee_joint, gameObject& leftAnkle_joint, gameObject& rightAnkle_joint,
	gameObject& leftShoulder_joint, gameObject& rightShoulder_joint, gameObject& leftElbow_joint, gameObject& rightElbow_joint, gameObject& leftWrist_joint, gameObject& rightWrist_joint, 
	gameObject& leftCheek_joint, gameObject& rightCheek_joint);

float randomFloatFrog(float lowerBound, float upperBound);

class Frog {
private:
	gameObject leftHip_joint;
	gameObject rightHip_joint;
	gameObject leftKnee_joint;
	gameObject rightKnee_joint;
	gameObject leftAnkle_joint;
	gameObject rightAnkle_joint;

	gameObject leftShoulder_joint;
	gameObject rightShoulder_joint;
	gameObject leftElbow_joint;
	gameObject rightElbow_joint;
	gameObject leftWrist_joint;
	gameObject rightWrist_joint;

	float cheekMaxSize = 0.7;
	float cheekMinSize = 0.1;
	float cheekScale = cheekMinSize;
	bool cheekIncrease = true;

public:
	gameObject frog;

	gameObject leftCheek_joint;
	gameObject rightCheek_joint;

	void cheekPuff();

	Frog(GLuint VAO, int vertCount, std::vector<GLuint>* textures);
};