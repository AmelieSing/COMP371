#include "gameObject.h"

gameObject::gameObject()
{
	this->transform.position = glm::vec3(0.0f);
	this->transform.rotation = glm::vec3(0.0f);
	this->transform.scale = glm::vec3(1.0f);
	this->VAO = NULL;
	this->vertCount = 0;
	this->textureID = 0;
}

Transform gameObject::getTransform()
{
	return this->transform;
}

void gameObject::drawModel(GLenum drawMode, GLuint shaderProgram, GLuint worldMatrixLocation, GLuint colourVectorLocation, GLuint textureLocation)
{

	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glm::mat4 worldMatrix = glm::translate(glm::mat4(1.0f), this->transform.position) *
		glm::rotate(glm::mat4(1.0f), glm::radians(this->transform.rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f)) * //rotate x
		glm::rotate(glm::mat4(1.0f), glm::radians(this->transform.rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f)) * //rotate y
		glm::rotate(glm::mat4(1.0f), glm::radians(this->transform.rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f)) * //rotate z 
		glm::scale(glm::mat4(1.0f), this->transform.scale); //scale


	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]); //send transform to shader
	
	glUniform3f(colourVectorLocation, this->colourVector[0], this->colourVector[1], this->colourVector[2]); //send colour to shader
	//glUniform1i(textureLocation, 0);                // Set our Texture sampler to use Texture Unit 0
	if (VAO != NULL)
		glDrawArrays(drawMode, 0, this->vertCount);
	for (int i = 0; i < childGameObjects.size(); i++)
	{
		childGameObjects[i]->drawChildModel(drawMode, shaderProgram, worldMatrixLocation, worldMatrix, colourVectorLocation,textureLocation);
	}

	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &glm::mat4(1.0f)[0][0]);
	glUniform3f(colourVectorLocation, glm::vec3(1.0f)[0], glm::vec3(1.0f)[1], glm::vec3(1.0f)[2]);
	glBindVertexArray(0);
}

void gameObject::drawModelShadows(GLenum drawMode, GLuint shaderProgram, GLuint worldMatrixLocation) {
	
	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);

	glm::mat4 worldMatrix = glm::translate(glm::mat4(1.0f), this->transform.position) *
		glm::rotate(glm::mat4(1.0f), glm::radians(this->transform.rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f)) * //rotate x
		glm::rotate(glm::mat4(1.0f), glm::radians(this->transform.rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f)) * //rotate y
		glm::rotate(glm::mat4(1.0f), glm::radians(this->transform.rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f)) * //rotate z 
		glm::scale(glm::mat4(1.0f), this->transform.scale); //scale


	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]); //send transform to shader

	if (VAO != NULL)
		glDrawArrays(drawMode, 0, this->vertCount);
	for (int i = 0; i < childGameObjects.size(); i++)
	{
		childGameObjects[i]->drawChildModelShadows(drawMode, shaderProgram, worldMatrixLocation, worldMatrix);
	}

	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &glm::mat4(1.0f)[0][0]);
	glBindVertexArray(0);
}

void gameObject::setTransform(glm::vec3 t_pos, glm::vec3 t_rot, glm::vec3 t_sca)
{
	glm::vec3 posDf = t_pos - transform.position;
	transform.position = t_pos;
	glm::vec3 rotDf = t_rot - transform.rotation;
	transform.rotation = t_rot;
	glm::vec3 scaDf = t_sca - transform.scale;
	transform.scale = t_sca;
}

void gameObject::setTransformPosition(glm::vec3 t_pos)
{
	this->transform.position = t_pos;
}

void gameObject::setTransformPosition(float t_x, float t_y, float t_z)
{
	this->transform.position.x = t_x;
	this->transform.position.y = t_y;
	this->transform.position.z = t_z;
}


void gameObject::setTransformRotation(glm::vec3 t_rot)
{
	this->transform.rotation = t_rot;
}

void gameObject::setTransformRotation(float t_x, float t_y, float t_z)
{
	this->transform.rotation.x = t_x;
	this->transform.rotation.y = t_y;
	this->transform.rotation.z = t_z;
}

void gameObject::setTransformScale(glm::vec3 t_sca)
{
	this->transform.scale = t_sca;
}

void gameObject::setTransformScale(float t_x, float t_y, float t_z)
{
	this->transform.scale.x = t_x;
	this->transform.scale.y = t_y;
	this->transform.scale.z = t_z;
}


void gameObject::setColourVector(glm::vec3 t_coloutVector)
{
	this->colourVector = t_coloutVector;
}

void gameObject::addChildObject(gameObject* t_object)
{
	childGameObjects.push_back(t_object);
}

void gameObject::removeChildObject(int index)
{
	gameObject* temp = (childGameObjects.at(index));
	childGameObjects.erase(childGameObjects.begin()+index);
	if (temp->getChildArray().size() > 0)
	{
		for (int i = temp->getChildArray().size() - 1; i >= 0; i--)
		{
			delete temp->getChildObject(i);
		}
	}
	
}

void gameObject::generateAnimal(gameObject& animal, gameObject& neck_joint, gameObject& leftHip_joint, gameObject& rightHip_joint, gameObject& leftShoulder_joint, gameObject& rightShoulder_joint)
{
	//generate a seed
	std::time_t t;
	std::srand((unsigned)std::time(&t));

	//Remove and delete all old data
	for (int i = animal.getChildArray().size()-1; i >= 0 ; i--)
	{
		animal.removeChildObject(i);
	}
	for (int i = neck_joint.getChildArray().size() - 1; i >= 0; i--)
	{
		neck_joint.removeChildObject(i);
	}
	for (int i = leftHip_joint.getChildArray().size() - 1; i >= 0; i--)
	{
		leftHip_joint.removeChildObject(i);
	}
	for (int i = rightHip_joint.getChildArray().size() - 1; i >= 0; i--)
	{
		rightHip_joint.removeChildObject(i);
	}
	for (int i = leftShoulder_joint.getChildArray().size() - 1; i >= 0; i--)
	{
		leftShoulder_joint.removeChildObject(i);
	}
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
	mainBody->setVAO(this->VAO);
	mainBody->setVertCount(this->vertCount);
	mainBody->setTexture(this->textureID);

	//generate arm values
	float xArms = std::rand() % 6 + 3;
	float yArms = std::rand() % 1 + 1;
	float zArms = std::rand() % (int)(zbody)+1;
	gameObject* leftArm = new gameObject;
	gameObject* rightArm = new gameObject;
	leftArm->setVAO(this->VAO);
	rightArm->setVAO(this->VAO);
	leftArm->setVertCount(this->vertCount);
	rightArm->setVertCount(this->vertCount);
	leftArm->setTexture(this->textureID);
	rightArm->setTexture(this->textureID);
	leftShoulder_joint.addChildObject(leftArm);
	rightShoulder_joint.addChildObject(rightArm);
	leftArm->setTransformScale(xArms, yArms, zArms);
	rightArm->setTransformScale(xArms, yArms, zArms);
	leftArm->setTransformPosition(-xArms / 2, 0, 0);
	rightArm->setTransformPosition(xArms / 2, 0, 0);

	//generate posiiton for shoulders
	float xShoulder = xbody / 2;
	float yShoulder = std::rand() % (int)(ybody) + (-ybody/2+yArms);
	float zShoulder = std::rand() % (int)(zbody + (-zArms/2)) + ((-zbody/2)+(zArms/2));
	leftShoulder_joint.setTransformPosition(-xShoulder, yShoulder, zShoulder);
	rightShoulder_joint.setTransformPosition(xShoulder, yShoulder, zShoulder);

	//generate leg values
	float xLegs = std::rand() % 2 + 0.75;
	float yLegs = std::rand() % 5 + 2;
	float zLegs = xLegs;
	gameObject* leftLeg = new gameObject;
	gameObject* rightLeg = new gameObject;
	leftLeg->setVAO(this->VAO);
	rightLeg->setVAO(this->VAO);
	leftLeg->setVertCount(this->vertCount);
	rightLeg->setVertCount(this->vertCount);
	leftLeg->setTexture(this->textureID);
	rightLeg->setTexture(this->textureID);
	leftHip_joint.addChildObject(leftLeg);
	rightHip_joint.addChildObject(rightLeg);
	leftLeg->setTransformScale(xLegs, yLegs, zLegs);
	rightLeg->setTransformScale(xLegs, yLegs, zLegs);
	leftLeg->setTransformPosition(0, -yLegs/2, 0);
	rightLeg->setTransformPosition(0, -yLegs/2, 0);

	leftHip_joint.setTransformPosition(-xbody/2+xLegs/2,-ybody/2,0);
	rightHip_joint.setTransformPosition(xbody/2-xLegs/2,-ybody/2,0);

}

void gameObject::drawChildModel(GLenum drawMode, GLuint shaderProgram, GLuint worldMatrixLocation, glm::mat4 parentMatrix, GLuint colourVectorLocation, GLuint textureLocation)
{
	glUseProgram(shaderProgram);
	glBindVertexArray(this->VAO);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, this->textureID);
	glm::mat4 worldMatrix = glm::mat4(1.0f);
	worldMatrix *= parentMatrix;
	worldMatrix *= glm::translate(glm::mat4(1.0f), this->transform.position);
	worldMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(this->transform.rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate x
	worldMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(this->transform.rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f)); //rotate y
	worldMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(this->transform.rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f)); //rotate z
	worldMatrix *= glm::scale(glm::mat4(1.0f), this->transform.scale);

	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]); //send transform to shader

	glUniform3f(colourVectorLocation, this->colourVector[0], this->colourVector[1], this->colourVector[2]); //send colour to shader
	//glUniform1i(textureLocation, 0);                // Set our Texture sampler to use Texture Unit 0

	if (VAO != NULL)
		glDrawArrays(drawMode, 0, this->vertCount);
	for (int i = 0; i < childGameObjects.size(); i++)
	{
		childGameObjects[i]->drawChildModel(drawMode, shaderProgram, worldMatrixLocation, worldMatrix, colourVectorLocation,textureLocation);
	}

	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &glm::mat4(1.0f)[0][0]);
	glUniform3f(colourVectorLocation, glm::vec3(1.0f)[0], glm::vec3(1.0f)[1], glm::vec3(1.0f)[2]);
	glBindVertexArray(0);
}

void gameObject::drawChildModelShadows(GLenum drawMode, GLuint shaderProgram, GLuint worldMatrixLocation, glm::mat4 parentMatrix) {
	
	glUseProgram(shaderProgram);
	glBindVertexArray(this->VAO);

	glm::mat4 worldMatrix = glm::mat4(1.0f);
	worldMatrix *= parentMatrix;
	worldMatrix *= glm::translate(glm::mat4(1.0f), this->transform.position);
	worldMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(this->transform.rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f)); //rotate x
	worldMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(this->transform.rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f)); //rotate y
	worldMatrix *= glm::rotate(glm::mat4(1.0f), glm::radians(this->transform.rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f)); //rotate z
	worldMatrix *= glm::scale(glm::mat4(1.0f), this->transform.scale);

	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]); //send transform to shader

	if (VAO != NULL)
		glDrawArrays(drawMode, 0, this->vertCount);
	for (int i = 0; i < childGameObjects.size(); i++)
	{
		childGameObjects[i]->drawChildModelShadows(drawMode, shaderProgram, worldMatrixLocation, worldMatrix);
	}

	glBindVertexArray(0);
}