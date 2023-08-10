#include "animalGenerators.h"


void generateAnimal(GLuint VAO,int vertCount,GLuint texture, gameObject& animal, gameObject& neck_joint, gameObject& leftHip_joint, gameObject& rightHip_joint, gameObject& leftShoulder_joint, gameObject& rightShoulder_joint)
{
	//generate a seed
	std::time_t t;
	std::srand((unsigned)std::time(&t));

	//Remove and delete all old data
	animal.setVAO(NULL);
	animal.setVertCount(0);
	animal.setTexture(0);
	for (int i = animal.getChildArray().size() - 1; i >= 0; i--)
	{
		animal.removeChildObject(i);
	}
	neck_joint.setVAO(NULL);
	neck_joint.setVertCount(0);
	neck_joint.setTexture(0);
	for (int i = neck_joint.getChildArray().size() - 1; i >= 0; i--)
	{
		neck_joint.removeChildObject(i);
	}
	leftHip_joint.setVAO(NULL);
	leftHip_joint.setVertCount(0);
	leftHip_joint.setTexture(0);
	for (int i = leftHip_joint.getChildArray().size() - 1; i >= 0; i--)
	{
		leftHip_joint.removeChildObject(i);
	}
	rightHip_joint.setVAO(NULL);
	rightHip_joint.setVertCount(0);
	rightHip_joint.setTexture(0);
	for (int i = rightHip_joint.getChildArray().size() - 1; i >= 0; i--)
	{
		rightHip_joint.removeChildObject(i);
	}
	leftShoulder_joint.setVAO(NULL);
	leftShoulder_joint.setVertCount(0);
	leftShoulder_joint.setTexture(0);
	for (int i = leftShoulder_joint.getChildArray().size() - 1; i >= 0; i--)
	{
		leftShoulder_joint.removeChildObject(i);
	}
	rightShoulder_joint.setVAO(NULL);
	rightShoulder_joint.setVertCount(0);
	rightShoulder_joint.setTexture(0);
	for (int i = rightShoulder_joint.getChildArray().size() - 1; i >= 0; i--)
	{
		rightShoulder_joint.removeChildObject(i);
	}

	//Set 'animal' as parent to all children 
	animal.addChildObject(&leftHip_joint);
	animal.addChildObject(&rightHip_joint);
	animal.addChildObject(&neck_joint);
	animal.addChildObject(&leftShoulder_joint);
	animal.addChildObject(&rightShoulder_joint);

	//Generate body shape values
	float xbody = std::rand() % 6 + 3;
	float ybody = std::rand() % 6 + 2;
	float zbody = std::rand() % 6 + 4;
	gameObject* mainBody = new gameObject;
	animal.addChildObject(mainBody);
	mainBody->setTransformScale(xbody, ybody, zbody);
	mainBody->setVAO(VAO);
	mainBody->setVertCount(vertCount);
	mainBody->setTexture(texture);

	//generate arm values
	float xArms = std::rand() % 6 + 3;
	float yArms = std::rand() % 1 + 1;
	float zArms = std::rand() % (int)(zbody)+1;
	gameObject* leftArm = new gameObject;
	gameObject* rightArm = new gameObject;
	leftArm->setVAO(VAO);
	rightArm->setVAO(VAO);
	leftArm->setVertCount(vertCount);
	rightArm->setVertCount(vertCount);
	leftArm->setTexture(texture);
	rightArm->setTexture(texture);
	leftShoulder_joint.addChildObject(leftArm);
	rightShoulder_joint.addChildObject(rightArm);
	leftArm->setTransformScale(xArms, yArms, zArms);
	rightArm->setTransformScale(xArms, yArms, zArms);
	leftArm->setTransformPosition(-xArms / 2, 0, 0);
	rightArm->setTransformPosition(xArms / 2, 0, 0);

	//generate posiiton for shoulders
	float xShoulder = xbody / 2;
	float yShoulder = std::rand() % (int)(ybody)+(-ybody / 2 + yArms);
	float zShoulder = std::rand() % (int)(zbody + (-zArms / 2)) + ((-zbody / 2) + (zArms / 2));
	leftShoulder_joint.setTransformPosition(-xShoulder, yShoulder, zShoulder);
	rightShoulder_joint.setTransformPosition(xShoulder, yShoulder, zShoulder);

	//generate leg values
	float xLegs = std::rand() % 2 + 0.75;
	float yLegs = std::rand() % 5 + 2;
	float zLegs = xLegs;
	gameObject* leftLeg = new gameObject;
	gameObject* rightLeg = new gameObject;
	leftLeg->setVAO(VAO);
	rightLeg->setVAO(VAO);
	leftLeg->setVertCount(vertCount);
	rightLeg->setVertCount(vertCount);
	leftLeg->setTexture(texture);
	rightLeg->setTexture(texture);
	leftHip_joint.addChildObject(leftLeg);
	rightHip_joint.addChildObject(rightLeg);
	leftLeg->setTransformScale(xLegs, yLegs, zLegs);
	rightLeg->setTransformScale(xLegs, yLegs, zLegs);
	leftLeg->setTransformPosition(0, -yLegs / 2, 0);
	rightLeg->setTransformPosition(0, -yLegs / 2, 0);

	leftHip_joint.setTransformPosition(-xbody / 2 + xLegs / 2, -ybody / 2, 0);
	rightHip_joint.setTransformPosition(xbody / 2 - xLegs / 2, -ybody / 2, 0);

	float xHead = std::rand() % 5 + 1;
	float yHead = std::rand() % 5 + 1;
	float zHead = std::rand() % 6 + 1;
	gameObject* head = new gameObject;
	head->setVAO(VAO);
	head->setVertCount(vertCount);
	head->setTexture(texture);
	head->setTransformScale(xHead, yHead, zHead);
	neck_joint.addChildObject(head);

	neck_joint.setTransformPosition(0.0f, ybody / 2, zbody / 2);

	animal.setTransformPosition(0, ybody / 2 + yLegs, 0);

}