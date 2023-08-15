#include "animalGenerators.h"


void generateAnimal(GLuint VAO,int vertCount, std::vector<GLuint>* textures, gameObject& animal, gameObject& neck_joint, gameObject& leftHip_joint, gameObject& rightHip_joint, gameObject& leftShoulder_joint, gameObject& rightShoulder_joint)
{
	//generate a seed
	time_t t;
	std::srand((unsigned)time(&t)+rand());

	//Remove and delete all old data
	animal.setVAO(NULL);
	animal.setVertCount(0);
	animal.setTexture(0);
	animal.getChildArray().clear();

	neck_joint.setVAO(NULL);
	neck_joint.setVertCount(0);
	neck_joint.setTexture(0);
	neck_joint.getChildArray().clear();
	
	leftHip_joint.setVAO(NULL);
	leftHip_joint.setVertCount(0);
	leftHip_joint.setTexture(0);
	leftHip_joint.getChildArray().clear();

	rightHip_joint.setVAO(NULL);
	rightHip_joint.setVertCount(0);
	rightHip_joint.setTexture(0);
	rightHip_joint.getChildArray().clear();

	leftShoulder_joint.setVAO(NULL);
	leftShoulder_joint.setVertCount(0);
	leftShoulder_joint.setTexture(0);
	leftShoulder_joint.getChildArray().clear();

	rightShoulder_joint.setVAO(NULL);
	rightShoulder_joint.setVertCount(0);
	rightShoulder_joint.setTexture(0);
	rightShoulder_joint.getChildArray().clear();

	//Set 'animal' as parent to all children 
	animal.addChildObject(&leftHip_joint);
	animal.addChildObject(&rightHip_joint);
	animal.addChildObject(&neck_joint);
	animal.addChildObject(&leftShoulder_joint);
	animal.addChildObject(&rightShoulder_joint);

	//texture selector
	GLuint textureChoice;
	textureChoice = rand() % textures->size();

	//colour randomizer
	float red = randomFloat(1.0, 0);
	float green = randomFloat(1.0, 0);
	float blue = randomFloat(1.0, 0);

	//Generate body shape values
	float xbody = randomFloat(2.2, 0.6);
	float ybody = randomFloat(1.5, 0.6);
	float zbody = randomFloat(2, 1);
	gameObject* mainBody = new gameObject;
	animal.addChildObject(mainBody);
	mainBody->setTransformScale(xbody, ybody, zbody);
	mainBody->setVAO(VAO);
	mainBody->setVertCount(vertCount);
	mainBody->setTexture(textures->at(textureChoice));
	mainBody->setColourVector(red, green, blue);

	red = randomFloat(1.0, 0);
	green = randomFloat(1.0, 0);
	blue = randomFloat(1.0, 0);

	textureChoice = rand() % textures->size();

	//Generate tail values
	float xTail = randomFloat(0.2,0.05);
	float yTail = randomFloat(0.5,0.3);
	float zTail = randomFloat(1.3,0.3);
	float tailAngle = randomFloat(40, 20);
	gameObject* tail = new gameObject;
	animal.addChildObject(tail);
	tail->setVAO(VAO);
	tail->setVertCount(vertCount);
	tail->setTexture(textures->at(textureChoice));
	tail->setTransformScale(xTail, yTail, zTail);
	tail->setTransformPosition(0.0f,ybody*2,-zbody*2);
	tail->setTransformRotation(tailAngle , 0.0f, 0.0f);
	tail->setColourVector(red, green, blue);

	red = randomFloat(1.0, 0);
	green = randomFloat(1.0, 0);
	blue = randomFloat(1.0, 0);

	textureChoice = rand() % textures->size();

	//generate arm values
	float xArms = randomFloat(2,0.5);
	float yArms = randomFloat(0.4,0.15);
	float zArms = randomFloat(zbody,0.5);
	gameObject* leftArm = new gameObject;
	gameObject* rightArm = new gameObject;
	leftArm->setVAO(VAO);
	rightArm->setVAO(VAO);
	leftArm->setVertCount(vertCount);
	rightArm->setVertCount(vertCount);
	leftArm->setTexture(textures->at(textureChoice));
	rightArm->setTexture(textures->at(textureChoice));
	leftShoulder_joint.addChildObject(leftArm);
	rightShoulder_joint.addChildObject(rightArm);
	leftArm->setTransformScale(xArms, yArms, zArms);
	rightArm->setTransformScale(xArms, yArms, zArms);
	leftArm->setTransformPosition(-xArms*2, 0, 0);
	rightArm->setTransformPosition(xArms*2, 0, 0);
	leftArm->setColourVector(red, green, blue);
	rightArm->setColourVector(red, green, blue);

	//generate posiiton for shoulders
	float xShoulder = xbody;
	float yShoulder = randomFloat(ybody*2,(-ybody / 2) + yArms);
	float zShoulder = randomFloat((zbody/2)-(zArms/2) ,(-zbody / 2) + (zArms / 2));
	leftShoulder_joint.setTransformPosition(-xShoulder*2, yShoulder, zShoulder);
	rightShoulder_joint.setTransformPosition(xShoulder*2, yShoulder, zShoulder);

	red = randomFloat(1.0, 0);
	green = randomFloat(1.0, 0);
	blue = randomFloat(1.0, 0);

	textureChoice = rand() % textures->size();

	//generate leg values
	float xLegs = randomFloat(0.5,0.2);
	float yLegs = randomFloat(zbody,0.75);
	float zLegs = xLegs;
	gameObject* leftLeg = new gameObject;
	gameObject* rightLeg = new gameObject;
	leftLeg->setVAO(VAO);
	rightLeg->setVAO(VAO);
	leftLeg->setVertCount(vertCount);
	rightLeg->setVertCount(vertCount);
	leftLeg->setTexture(textures->at(textureChoice));
	rightLeg->setTexture(textures->at(textureChoice));
	leftHip_joint.addChildObject(leftLeg);
	rightHip_joint.addChildObject(rightLeg);
	leftLeg->setTransformScale(xLegs, yLegs, zLegs);
	rightLeg->setTransformScale(xLegs, yLegs, zLegs);
	leftLeg->setTransformPosition(0, -yLegs, 0);
	rightLeg->setTransformPosition(0, -yLegs, 0);
	rightLeg->setColourVector(red, green, blue);
	leftLeg->setColourVector(red, green, blue);

	leftHip_joint.setTransformPosition(-xbody-xLegs, -ybody - yLegs, 0);
	rightHip_joint.setTransformPosition(xbody+xLegs, -ybody-yLegs, 0);
	
	red = randomFloat(1.0, 0);
	green = randomFloat(1.0, 0);
	blue = randomFloat(1.0, 0);

	textureChoice = rand() % textures->size();

	//generate head values
	float xHead = randomFloat(1, 0.3);
	float yHead = randomFloat(1, 0.5);
	float zHead = randomFloat(1.2, 0.6);
	gameObject* head = new gameObject;
	head->setVAO(VAO);
	head->setVertCount(vertCount);
	head->setTexture(textures->at(textureChoice));
	head->setTransformScale(xHead, yHead, zHead);
	neck_joint.addChildObject(head);
	neck_joint.setTransformPosition(0.0f, ybody+(yHead)*2, zbody+(zHead)*3.5);
	head->setColourVector(red, green, blue);

	red = randomFloat(1.0, 0);
	green = randomFloat(1.0, 0);
	blue = randomFloat(1.0, 0);

	textureChoice = rand() % textures->size();

	//generate eye values
	float xEye = randomFloat(0.3,0.2);
	float yEye = xEye;
	float zEye = xEye;
	gameObject* leftEye = new gameObject;
	gameObject* rightEye = new gameObject;
	leftEye->setVAO(VAO);
	leftEye->setVertCount(vertCount);
	leftEye->setTexture(textures->at(textureChoice));
	leftEye->setTransformScale(glm::vec3(xEye));
	rightEye->setVAO(VAO);
	rightEye->setVertCount(vertCount);
	rightEye->setTexture(textures->at(textureChoice));
	rightEye->setTransformScale(glm::vec3(xEye));
	head->addChildObject(leftEye);
	head->addChildObject(rightEye);
	leftEye->setTransformPosition(-xHead*2.8,yHead,zHead*2);
	rightEye->setTransformPosition(xHead*2.8,yHead,zHead*2);
	rightEye->setColourVector(red, green, blue);
	leftEye->setColourVector(red, green, blue);

	red = randomFloat(1.0, 0);
	green = randomFloat(1.0, 0);
	blue = randomFloat(1.0, 0);

	textureChoice = rand() % textures->size();

	//generate beak values
	float xBeak = randomFloat(0.3,0.15);
	float yBeak = xBeak;
	float zBeak = randomFloat(0.5,0.3);
	gameObject* beak = new gameObject;
	beak->setVAO(VAO);
	beak->setVertCount(vertCount);
	beak->setTexture(textures->at(textureChoice));
	head->addChildObject(beak);
	beak->setTransformScale(xBeak, yBeak, zBeak);
	beak->setTransformPosition(0, -yHead, zHead * 2.5);
	beak->setTransformRotation(35.0f, 0, 0);
	beak->setColourVector(red, green, blue);

	animal.setTransformPosition(0,(xbody)*2.2 + (yLegs)*3, 0);
	animal.setTransformRotation(0, 0, 0);

}

double randomFloat(float max,float min)
{
	time_t t;
	std::srand((unsigned)time(&t) + rand());
	double random = (float)(rand()) / (float)RAND_MAX;
	double dif = max - min;
	return min + (dif * random);
}

void bipedalAnimal::animate()
{
	this->angle += 0.01;

	leftHip_joint.setTransformRotation(glm::vec3(45 * std::sin(angle * 10), 0.0f, 0.0f));
	rightHip_joint.setTransformRotation(glm::vec3(-45 * std::sin(angle * 10), 0.0f, 0.0f));
	leftShoulder_joint.setTransformRotation(glm::vec3(0.0f, 0.0f, -30 * std::sin(angle * 15)));
	rightShoulder_joint.setTransformRotation(glm::vec3(0.0f, 0.0f, 30 * std::sin(angle * 15)));
	neck_joint.setTransformPosition(glm::vec3(neck_joint.getTransformPosition()[0], neck_joint.getTransformPosition()[1], neck_joint.getTransformPosition()[2] + (std::sin(angle * 25) / 12)));
}

void bipedalAnimal::reGenerate(glm::vec3 cameraPosition, GLuint VAO, int vertCount, std::vector<GLuint>* textures)
{

	generateAnimal(VAO, vertCount, textures, animalCore, neck_joint, leftHip_joint, rightHip_joint, leftShoulder_joint, rightShoulder_joint);
	
	float xTransform = randomFloat(100, -100) + cameraPosition.x;
	if (rand() % 2 == 0)
	{
		xTransform *= -1;
	}
	else
		xTransform *= 1;

	//std::srand((unsigned)time(&t) - rand());
	float zTransform = randomFloat(100, -100) + cameraPosition.z;
	if (rand() % 2 == 0)
	{
		zTransform *= -1;
	}
	else
		zTransform *= 1;

	animalCore.setTransformPosition(xTransform, animalCore.getTransformPosition().y, zTransform);
	animalCore.setTransformRotation(0, randomFloat(360, 0), 0);

}


