#include "animalFrog.h"
#include <random>

void generateFrog(GLuint vaSphere, int vertCount, std::vector<GLuint>* textures, gameObject& frog,
	gameObject& leftHip_joint, gameObject& rightHip_joint, gameObject& leftKnee_joint, gameObject& rightKnee_joint, gameObject& leftAnkle_joint, gameObject& rightAnkle_joint,
	gameObject& leftShoulder_joint, gameObject& rightShoulder_joint, gameObject& leftElbow_joint, gameObject& rightElbow_joint, gameObject& leftWrist_joint, gameObject& rightWrist_joint,
	gameObject& leftCheek_joint, gameObject& rightCheek_joint) {


	//Remove and delete all old data
	frog.setVAO(NULL);
	frog.setVertCount(0);
	frog.setTexture(0);
	frog.getChildArray().clear();

	leftHip_joint.setVAO(NULL);
	leftHip_joint.setVertCount(0);
	leftHip_joint.setTexture(0);
	leftHip_joint.getChildArray().clear();

	rightHip_joint.setVAO(NULL);
	rightHip_joint.setVertCount(0);
	rightHip_joint.setTexture(0);
	rightHip_joint.getChildArray().clear();

	leftKnee_joint.setVAO(NULL);
	leftKnee_joint.setVertCount(0);
	leftKnee_joint.setTexture(0);
	leftKnee_joint.getChildArray().clear();

	rightKnee_joint.setVAO(NULL);
	rightKnee_joint.setVertCount(0);
	rightKnee_joint.setTexture(0);
	rightKnee_joint.getChildArray().clear();

	leftAnkle_joint.setVAO(NULL);
	leftAnkle_joint.setVertCount(0);
	leftAnkle_joint.setTexture(0);
	leftAnkle_joint.getChildArray().clear();

	rightAnkle_joint.setVAO(NULL);
	rightAnkle_joint.setVertCount(0);
	rightAnkle_joint.setTexture(0);
	rightAnkle_joint.getChildArray().clear();

	leftShoulder_joint.setVAO(NULL);
	leftShoulder_joint.setVertCount(0);
	leftShoulder_joint.setTexture(0);
	leftShoulder_joint.getChildArray().clear();

	rightShoulder_joint.setVAO(NULL);
	rightShoulder_joint.setVertCount(0);
	rightShoulder_joint.setTexture(0);
	rightShoulder_joint.getChildArray().clear();

	leftElbow_joint.setVAO(NULL);
	leftElbow_joint.setVertCount(0);
	leftElbow_joint.setTexture(0);
	leftElbow_joint.getChildArray().clear();

	rightElbow_joint.setVAO(NULL);
	rightElbow_joint.setVertCount(0);
	rightElbow_joint.setTexture(0);
	rightElbow_joint.getChildArray().clear();

	leftCheek_joint.setVAO(NULL);
	leftCheek_joint.setVertCount(0);
	leftCheek_joint.setTexture(0);
	leftCheek_joint.getChildArray().clear();

	rightWrist_joint.setVAO(NULL);
	rightWrist_joint.setVertCount(0);
	rightWrist_joint.setTexture(0);
	rightWrist_joint.getChildArray().clear();

	leftWrist_joint.setVAO(NULL);
	leftWrist_joint.setVertCount(0);
	leftWrist_joint.setTexture(0);
	leftWrist_joint.getChildArray().clear();

	rightCheek_joint.setVAO(NULL);
	rightCheek_joint.setVertCount(0);
	rightCheek_joint.setTexture(0);
	rightCheek_joint.getChildArray().clear();

	leftCheek_joint.setVAO(NULL);
	leftCheek_joint.setVertCount(0);
	leftCheek_joint.setTexture(0);
	leftCheek_joint.getChildArray().clear();

	//Set 'frog' as parent to children 
	frog.addChildObject(&leftHip_joint);
	frog.addChildObject(&rightHip_joint);

	frog.addChildObject(&leftShoulder_joint);
	frog.addChildObject(&rightShoulder_joint);

	frog.addChildObject(&rightCheek_joint);
	frog.addChildObject(&leftCheek_joint);

	//Torso
	//texture selector
	GLuint textureChoice;
	textureChoice = 1;


	//colour 
	float red = randomFloatFrog(0.0,1.0);
	float green = randomFloatFrog(0.0, 1.0);
	float blue = randomFloatFrog(0.0, 1.0);

	//Generate body shape values
	float xTorso = 1.0f;
	float yTorso = 0.5f;
	float zTorso = 1.5f;

	gameObject* torso = new gameObject;
	frog.addChildObject(torso);
	torso->setTransformScale(xTorso, yTorso, zTorso);
	torso->setVAO(vaSphere);
	torso->setVertCount(vertCount);
	torso->setTexture(textures->at(textureChoice));
	torso->setColourVector(red, green, blue);
	torso->setTransformRotation(-45.0f, 0.0f, 0.0f);

	//Upper Arms

	float xArm = 0.2f;
	float yArm = 0.2f;
	float zArm = 0.7f;

	gameObject* upperRArm = new gameObject;

	rightShoulder_joint.addChildObject(upperRArm);
	frog.addChildObject(upperRArm);
	upperRArm->setTransformScale(xArm, yArm, zArm);
	upperRArm->setVAO(vaSphere);
	upperRArm->setVertCount(vertCount);
	upperRArm->setTexture(textures->at(textureChoice));
	upperRArm->setColourVector(red, green, blue);
	upperRArm->setTransformPosition(xTorso * 3, -yTorso*1, zTorso * 0.1);
	upperRArm->setTransformRotation(70.0f, 0.0f, 0.0f);
	upperRArm->addChildObject(&rightElbow_joint);
	rightElbow_joint.setTransformScale(0.0f, 0, 0);

	gameObject* upperLArm = new gameObject;
	leftShoulder_joint.addChildObject(upperLArm);
	frog.addChildObject(upperLArm);
	upperLArm->setTransformScale(xArm, yArm, zArm);
	upperLArm->setVAO(vaSphere);
	upperLArm->setVertCount(vertCount);
	upperLArm->setTexture(textures->at(textureChoice));
	upperLArm->setColourVector(red, green, blue);
	upperLArm->setTransformPosition(-xTorso * 3, -yTorso * 1 , zTorso * 0.1);
	upperLArm->setTransformRotation(70.0f, 0.0f, 0.0f);
	upperLArm->addChildObject(&leftElbow_joint);
	leftElbow_joint.setTransformScale(0.0f, 0, 0);


	//Lower Arms

	gameObject* lowerRArm = new gameObject;
	float xLowerArm = 0.15f;
	float yLowerArm = 0.15f;
	float zLowerArm = 0.6;

	rightElbow_joint.addChildObject(lowerRArm);
	frog.addChildObject(lowerRArm);
	lowerRArm->setTransformScale(xLowerArm, yLowerArm, zLowerArm);
	lowerRArm->setVAO(vaSphere);
	lowerRArm->setVertCount(vertCount);
	lowerRArm->setTexture(textures->at(textureChoice));
	lowerRArm->setColourVector(red, green, blue);
	lowerRArm->setTransformPosition(xTorso * 3, -yTorso * 6, zTorso * 1.2);
	lowerRArm->setTransformRotation(45.0f, 0.0f, 0.0f);
	lowerRArm->addChildObject(&rightWrist_joint);
	rightWrist_joint.setTransformScale(0.0f, 0, 0);

	gameObject* lowerLArm = new gameObject;
	leftElbow_joint.addChildObject(lowerLArm);
	frog.addChildObject(lowerLArm);
	lowerLArm->setTransformScale(xLowerArm, yLowerArm, zLowerArm);
	lowerLArm->setVAO(vaSphere);
	lowerLArm->setVertCount(vertCount);
	lowerLArm->setTexture(textures->at(textureChoice));
	lowerLArm->setColourVector(red, green, blue);
	lowerLArm->setTransformPosition(-xTorso * 3, -yTorso * 6, zTorso * 1.2);
	lowerLArm->setTransformRotation(45.0f, 0.0f, 0.0f);
	lowerLArm->addChildObject(&leftWrist_joint);
	leftWrist_joint.setTransformScale(0.0f, 0, 0);



	//Upper Legs

	//Generate body shape values
	float xUpperLeg = 0.3f;
	float yUpperLeg = 0.3f;
	float zUpperLeg = 1.0f;

	gameObject* upperRLeg = new gameObject;
	
	rightHip_joint.addChildObject(upperRLeg);
	frog.addChildObject(upperRLeg);
	upperRLeg->setTransformScale(xUpperLeg, yUpperLeg, zUpperLeg);
	upperRLeg->setVAO(vaSphere);
	upperRLeg->setVertCount(vertCount);
	upperRLeg->setTexture(textures->at(textureChoice));
	upperRLeg->setColourVector(red, green, blue);
	upperRLeg->setTransformPosition(xTorso * 2.5, -yTorso*6 , -zTorso * 1);
	upperRLeg->setTransformRotation(10.0f, 30.0f, 0.0f);
	upperRLeg->addChildObject(&rightKnee_joint);
	rightKnee_joint.setTransformScale(0.0f,0,0);

	gameObject* upperLLeg = new gameObject;
	leftHip_joint.addChildObject(upperLLeg);
	frog.addChildObject(upperLLeg);
	upperLLeg->setTransformScale(xUpperLeg, yUpperLeg, zUpperLeg);
	upperLLeg->setVAO(vaSphere);
	upperLLeg->setVertCount(vertCount);
	upperLLeg->setTexture(textures->at(textureChoice));
	upperLLeg->setColourVector(red, green, blue);
	upperLLeg->setTransformPosition(-xTorso * 2.5, -yTorso * 6, -zTorso * 1);
	upperLLeg->setTransformRotation(10.0f, -30.0f, 0.0f);
	upperLLeg->addChildObject(&leftKnee_joint);
	leftKnee_joint.setTransformScale(0.0f, 0, 0);

	//Lower Legs

	//Generate body shape values
	float xLowerLeg = 0.25f;
	float yLowerLeg = 0.25f;
	float zLowerLeg = 1.0f;

	gameObject* lowerRLeg = new gameObject;
	frog.addChildObject(lowerRLeg);
	rightKnee_joint.addChildObject(lowerRLeg);
	lowerRLeg->setTransformScale(xLowerLeg, yLowerLeg, zLowerLeg);
	lowerRLeg->setVAO(vaSphere);
	lowerRLeg->setVertCount(vertCount);
	lowerRLeg->setTexture(textures->at(textureChoice));
	lowerRLeg->setColourVector(red, green, blue);
	lowerRLeg->setTransformPosition(xTorso * 2.5, -yTorso * 8, -zTorso * 1);
	lowerRLeg->setTransformRotation(-10.0f, 30.0f, 0.0f);
	lowerRLeg->addChildObject(&rightAnkle_joint);
	rightAnkle_joint.setTransformScale(0, 0.0, 0.0);
	

	gameObject* lowerLLeg = new gameObject;
	frog.addChildObject(lowerLLeg);
	leftKnee_joint.addChildObject(lowerLLeg);
	lowerLLeg->setTransformScale(xLowerLeg, yLowerLeg, zLowerLeg);
	lowerLLeg->setVAO(vaSphere);
	lowerLLeg->setVertCount(vertCount);
	lowerLLeg->setTexture(textures->at(textureChoice));
	lowerLLeg->setColourVector(red, green, blue);
	lowerLLeg->setTransformPosition(-xTorso * 2.5, -yTorso * 8, -zTorso * 1);
	lowerLLeg->setTransformRotation(-10.0f, -30.0f, 0.0f);
	lowerLLeg->addChildObject(&leftAnkle_joint);
	leftAnkle_joint.setTransformScale(0, 0.0, 0.0);

	//Feet



	//Cheeks

	float xCheek = 0.1f;
	float yCheek = 0.1f;
	float zCheek = 0.1f;


	//rightCheek_joint.setTransformScale(xCheek, yCheek, zCheek);
	rightCheek_joint.setVAO(vaSphere);
	rightCheek_joint.setVertCount(vertCount);
	rightCheek_joint.setTexture(textures->at(textureChoice));
	rightCheek_joint.setColourVector(red, green, blue);
	rightCheek_joint.setTransformPosition(xTorso * 1.5, yTorso * 2, zTorso * 1);
	rightCheek_joint.setTransformRotation(0.0f, 135.0f, 0.0f);


	//leftCheek_joint.setTransformScale(xCheek, yCheek, zCheek);
	leftCheek_joint.setVAO(vaSphere);
	leftCheek_joint.setVertCount(vertCount);
	leftCheek_joint.setTexture(textures->at(textureChoice));
	leftCheek_joint.setColourVector(red, green, blue);
	leftCheek_joint.setTransformPosition(-xTorso * 1.5, yTorso * 2, zTorso * 1);
	leftCheek_joint.setTransformRotation(0.0f, 135.0f, 0.0f);

	//Eyes
	textureChoice = 0;

	red = 40/255.0f;
	blue = 25/255.0f;
	green = 26/255.0f;

	float xEye = 0.3f;
	float yEye = 0.3f;
	float zEye = 0.3f;
	
	gameObject* rightEye = new gameObject;
	frog.addChildObject(rightEye);
	rightEye->setTransformScale(xEye, yEye, zEye);
	rightEye->setVAO(vaSphere);
	rightEye->setVertCount(vertCount);
	rightEye->setTexture(textures->at(textureChoice));
	rightEye->setColourVector(red, green, blue);
	rightEye->setTransformPosition(xTorso *1.5, yTorso * 6.5, zTorso * 1.5);
	rightEye->setTransformRotation(0.0f, 135.0f, 0.0f);

	gameObject* leftEye = new gameObject;
	frog.addChildObject(leftEye);
	leftEye->setTransformScale(xEye, yEye, zEye);
	leftEye->setVAO(vaSphere);
	leftEye->setVertCount(vertCount);
	leftEye->setTexture(textures->at(textureChoice));
	leftEye->setColourVector(red, green, blue);
	leftEye->setTransformPosition(-xTorso * 1.5, yTorso * 6.5, zTorso * 1.5);
	leftEye->setTransformRotation(0.0f, 135.0f, 0.0f);

	//Generate body shape values
	float xFoot = 0.2f;
	float yFoot = 0.2f;
	float zFoot = 0.7f;

	gameObject* rightFoot = new gameObject;
	frog.addChildObject(rightFoot);
	rightAnkle_joint.addChildObject(rightFoot);
	rightFoot->setTransformScale(xFoot, yFoot, zFoot);
	rightFoot->setVAO(vaSphere);
	rightFoot->setVertCount(vertCount);
	rightFoot->setTexture(textures->at(textureChoice));
	rightFoot->setColourVector(red, green, blue);
	rightFoot->setTransformPosition(xTorso * 2.5, -yTorso * 10, -zTorso * 1.5);
	rightFoot->setTransformRotation(15.0f, 30.0f, 0.0f);

	gameObject* leftFoot = new gameObject;
	frog.addChildObject(leftFoot);
	leftAnkle_joint.addChildObject(leftFoot);
	leftFoot->setTransformScale(xFoot, yFoot, zFoot);
	leftFoot->setVAO(vaSphere);
	leftFoot->setVertCount(vertCount);
	leftFoot->setTexture(textures->at(textureChoice));
	leftFoot->setColourVector(red, green, blue);
	leftFoot->setTransformPosition(-xTorso * 2.5, -yTorso * 10, -zTorso * 1.5);
	leftFoot->setTransformRotation(15.0f, -30.0f, 0.0f);

	//Hands

	float xFinger = 0.1f;
	float yFinger = 0.1f;
	float zFinger = 0.3;

	gameObject* rightRFinger = new gameObject;
	gameObject* middleRFinger = new gameObject;
	gameObject* leftRFinger = new gameObject;

	rightWrist_joint.addChildObject(rightRFinger);
	rightWrist_joint.addChildObject(middleRFinger);
	rightWrist_joint.addChildObject(leftRFinger);
	frog.addChildObject(rightRFinger);
	frog.addChildObject(middleRFinger);
	frog.addChildObject(leftRFinger);

	rightRFinger->setTransformScale(xFinger, yFinger, zFinger);
	rightRFinger->setVAO(vaSphere);
	rightRFinger->setVertCount(vertCount);
	rightRFinger->setTexture(textures->at(textureChoice));
	rightRFinger->setColourVector(red, green, blue);
	rightRFinger->setTransformPosition(xTorso * 3.5, -yTorso * 9, zTorso * 2.2);
	rightRFinger->setTransformRotation(30.0f, 45.0f, 0.0f);

	middleRFinger->setTransformScale(xFinger, yFinger, zFinger);
	middleRFinger->setVAO(vaSphere);
	middleRFinger->setVertCount(vertCount);
	middleRFinger->setTexture(textures->at(textureChoice));
	middleRFinger->setColourVector(red, green, blue);
	middleRFinger->setTransformPosition(xTorso * 3.0, -yTorso * 9, zTorso * 2.2);
	middleRFinger->setTransformRotation(30.0f, 0.0f, 0.0f);

	leftRFinger->setTransformScale(xFinger, yFinger, zFinger);
	leftRFinger->setVAO(vaSphere);
	leftRFinger->setVertCount(vertCount);
	leftRFinger->setTexture(textures->at(textureChoice));
	leftRFinger->setColourVector(red, green, blue);
	leftRFinger->setTransformPosition(xTorso * 2.5, -yTorso * 9, zTorso * 2.2);
	leftRFinger->setTransformRotation(30.0f, -45.0f, 0.0f);

	gameObject* rightLFinger = new gameObject;
	gameObject* middleLFinger = new gameObject;
	gameObject* leftLFinger = new gameObject;

	leftWrist_joint.addChildObject(rightLFinger);
	leftWrist_joint.addChildObject(middleLFinger);
	leftWrist_joint.addChildObject(leftLFinger);
	frog.addChildObject(rightLFinger);
	frog.addChildObject(middleLFinger);
	frog.addChildObject(leftLFinger);

	rightLFinger->setTransformScale(xFinger, yFinger, zFinger);
	rightLFinger->setVAO(vaSphere);
	rightLFinger->setVertCount(vertCount);
	rightLFinger->setTexture(textures->at(textureChoice));
	rightLFinger->setColourVector(red, green, blue);
	rightLFinger->setTransformPosition(-xTorso * 3.5, -yTorso * 9, zTorso * 2.2);
	rightLFinger->setTransformRotation(30.0f, -45.0f, 0.0f);

	middleLFinger->setTransformScale(xFinger, yFinger, zFinger);
	middleLFinger->setVAO(vaSphere);
	middleLFinger->setVertCount(vertCount);
	middleLFinger->setTexture(textures->at(textureChoice));
	middleLFinger->setColourVector(red, green, blue);
	middleLFinger->setTransformPosition(-xTorso * 3.0, -yTorso * 9, zTorso * 2.2);
	middleLFinger->setTransformRotation(30.0f, 0.0f, 0.0f);

	leftLFinger->setTransformScale(xFinger, yFinger, zFinger);
	leftLFinger->setVAO(vaSphere);
	leftLFinger->setVertCount(vertCount);
	leftLFinger->setTexture(textures->at(textureChoice));
	leftLFinger->setColourVector(red, green, blue);
	leftLFinger->setTransformPosition(-xTorso * 2.5, -yTorso * 9, zTorso * 2.2);
	leftLFinger->setTransformRotation(30.0f, 45.0f, 0.0f);
}

float randomFloatFrog(float lowerBound, float upperBound) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dist(lowerBound, upperBound);
	return dist(gen);
}

Frog::Frog(GLuint VAO, int vertCount, std::vector<GLuint>* textures) {
	generateFrog(VAO, vertCount, textures,  frog,
		 leftHip_joint,  rightHip_joint,  leftKnee_joint,  rightKnee_joint,  leftAnkle_joint,  rightAnkle_joint,
		 leftShoulder_joint,  rightShoulder_joint,  leftElbow_joint,  rightElbow_joint,  leftWrist_joint,  rightWrist_joint,
		 leftCheek_joint,  rightCheek_joint);
}

void Frog::cheekPuff() {

	leftCheek_joint.setTransformScale(cheekScale, cheekScale, cheekScale);
	rightCheek_joint.setTransformScale(cheekScale, cheekScale, cheekScale);

	if (cheekIncrease == true && cheekScale <= cheekMaxSize) {
		cheekScale += 0.01;
	}
	else if (cheekIncrease == false && cheekScale >= cheekMinSize) {
		cheekScale -= 0.01;
	}
	else if (cheekScale > cheekMaxSize && cheekIncrease == true) {
		cheekIncrease = false;
	}
	else if (cheekScale < cheekMinSize && cheekIncrease == false) {
		cheekIncrease = true;
	}
}