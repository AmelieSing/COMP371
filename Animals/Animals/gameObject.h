#pragma once

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL

#include <GL/glew.h> // Include GLEW - OpenGL Extension Wrangler

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <time.h>
#include <vector>



struct Transform {
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 rotation = glm::vec3(0.0f);
	glm::vec3 scale = glm::vec3(1.0f);
};

class gameObject
{
public:
	gameObject();
	struct Transform getTransform();
	void drawModel(GLenum drawMode, GLuint shaderProgram, GLuint worldMatrixLocation, GLuint colourVectorLocation, GLuint textureLocation);
	void drawModelShadows(GLenum drawMode, GLuint shaderProgram, GLuint worldMatrixLocation);
	GLuint getVertCount() { return vertCount; };
	GLuint getVAO() { return VAO; };
	void setVertCount(int num) { vertCount = num; };
	void setVAO(int num) { VAO = num; };
	void setTransform(glm::vec3 t_pos, glm::vec3 t_rot, glm::vec3 t_sca);
	void setTransformPosition(glm::vec3 t_pos);
	void setTransformPosition(float t_x, float t_y, float t_z);
	void setTransformRotation(glm::vec3 t_rot);
	void setTransformRotation(float t_x, float t_y, float t_z);
	void setTransformScale(glm::vec3 t_sca);
	void setTransformScale(float t_x, float t_y, float t_z);
	void setColourVector(glm::vec3 t_coloutVector);
	void setTexture(int t_id) { textureID = t_id; };
	void setTextureScale(int t_scale) { textureScale = t_scale; };
	glm::vec3 getTransformPosition() { return transform.position; };
	glm::vec3 getTransformRotation() { return transform.rotation; };
	glm::vec3 getTransformScale() { return transform.scale; };
	glm::vec3 getColourVector() { return colourVector; };
	void addChildObject(gameObject* t_object);
	void removeChildObject(int index);
	gameObject* getChildObject(int index) { return childGameObjects.at(index); };
	std::vector<gameObject*> &getChildArray() { return childGameObjects; };
	void generateAnimal(gameObject& animal, gameObject& neck_joint, gameObject& leftHip_joint, gameObject& rightHip_joint, gameObject& leftShoulder_joint, gameObject& rightShoulder_joint);

private:
	glm::vec3 colourVector = glm::vec3(1.0f);
	GLuint vertCount;
	GLuint textureID;
	GLfloat textureScale = 1.0f;
	GLuint VAO = NULL; //model data, 
	Transform transform{ glm::vec3(0.0f),glm::vec3(0.0f) ,glm::vec3(1.0f) };
	std::vector<gameObject*> childGameObjects;
	void drawChildModel(GLenum drawMode, GLuint shaderProgram, GLuint worldMatrixLocation, glm::mat4 parentMatrix, GLuint colourVectorLocation, GLuint textureLocation);
	void drawChildModelShadows(GLenum drawMode, GLuint shaderProgram, GLuint worldMatrixLocation, glm::mat4 parentMatrix);
};