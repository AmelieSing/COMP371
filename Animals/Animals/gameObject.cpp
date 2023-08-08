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

transform gameObject::getTransform()
{
	return transform;
}

void gameObject::drawModel(GLenum drawMode, GLuint shaderProgram, GLuint worldMatrixLocation, GLuint colourVectorLocation, GLuint textureLocation)
{

	glUseProgram(shaderProgram);
	glBindVertexArray(VAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glm::mat4 worldMatrix = glm::translate(glm::mat4(1.0f), this->transform.position) *
		glm::rotate(glm::mat4(1.0f), glm::radians(this->transform.rotation[0]), glm::vec3(1.0f, 0.0f, 0.0f)) * //rotate x
		glm::rotate(glm::mat4(1.0f), glm::radians(this->transform.rotation[1]), glm::vec3(0.0f, 1.0f, 0.0f)) * //rotate y
		glm::rotate(glm::mat4(1.0f), glm::radians(this->transform.rotation[2]), glm::vec3(0.0f, 0.0f, 1.0f)) * //rotate z 
		glm::scale(glm::mat4(1.0f), this->transform.scale); //scale


	glUniformMatrix4fv(worldMatrixLocation, 1, GL_FALSE, &worldMatrix[0][0]); //send transform to shader
	glUniform3f(colourVectorLocation, this->colourVector[0], this->colourVector[1], this->colourVector[2]); //send colour to shader
	glUniform1i(textureLocation, 0);                // Set our Texture sampler to use Texture Unit 0
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
	childGameObjects.erase(childGameObjects.begin() + index);
}

void gameObject::drawChildModel(GLenum drawMode, GLuint shaderProgram, GLuint worldMatrixLocation, glm::mat4 parentMatrix, GLuint colourVectorLocation, GLuint textureLocation)
{

	glUseProgram(shaderProgram);
	glBindVertexArray(this->VAO);
	glActiveTexture(GL_TEXTURE0);
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
	glUniform1i(textureLocation, 0);                // Set our Texture sampler to use Texture Unit 0

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