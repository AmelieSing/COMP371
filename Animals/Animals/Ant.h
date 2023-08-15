#pragma once

#include <iostream>
#include <vector>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace std;

float randomInRangeAnt(float lowerBound, float upperBound);

class Ant {
public: 
	Ant();
	Ant(int shaderProgram, int shaderShadowProgram, int sphereVao, int cubeVao, GLint texture1Uniform, vec3 bodyPos, int vertex, GLuint textureID, GLuint defaultID);
	void drawShadow();
	void draw();
	void moveAntennas();
    vec3 getPosition();

private:
    vec3 bodyPosition;
    vec3 bodySize;
    vec3 headPosition;
    vec3 eyePositionL;
    vec3 eyePositionR;
    vec3 tailPosition;
    vec3 leg1PositionL;
    vec3 leg1PositionR;
    vec3 leg2PositionL;
    vec3 leg2PositionR;
    vec3 leg3PositionL;
    vec3 leg3PositionR;
    vec3 legSize;
    vec3 antennaPositionL;
    vec3 antennaPositionR;
    vec3 antenna2PositionL;
    vec3 antenna2PositionR;
    vec3 antennaSize;
    vec3 rotationPointAntennaL;
    vec3 rotationPointAntennaR;
    float antScale = randomInRangeAnt(0.5f, 2.0f);
    vec3 scaleFactor = vec3(antScale, antScale, antScale);
    vec3 color = vec3(randomInRangeAnt(0.0, 1.0), randomInRangeAnt(0.0, 1.0), randomInRangeAnt(0.0, 1.0));


    float antennaAngle = -90.0f;
    bool antennaUp = false;
    bool antennaDown = false;
    bool goingUp = true;
    bool goingDown = false;
    float yaw = 0;
    float pitch = 0;
    float roll = 0;

    mat4 modelMatrix;
    int sphereVao;
    int cubeVao;
    int shaderProgram;
    int shaderShadowProgram;
    GLint texture1Uniform;
    int vertex;
    GLuint textureID;
    GLuint defaultID;
};

void SetUniformMat4Ant(GLuint shader_id, const char* uniform_name, mat4 uniform_value);
void SetUniformVec3Ant(GLuint shader_id, const char* uniform_name, vec3 uniform_value);
template <class T>
void SetUniform1ValueAnt(GLuint shader_id, const char* uniform_name, T uniform_value);
void createFloorShadowAnt(int shadowShaderProgram, int vao);
void createFloorAnt(int shaderProgram, int vao, GLint texture1Uniform);

