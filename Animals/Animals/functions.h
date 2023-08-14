#pragma once


#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/gtc/type_ptr.hpp>
using namespace glm;
using namespace std;

// shader variable setters
void SetUniformMat4(GLuint shader_id, const char* uniform_name, mat4 uniform_value);
void SetUniformVec3(GLuint shader_id, const char* uniform_name, vec3 uniform_value);
template <class T>
void SetUniform1Value(GLuint shader_id, const char* uniform_name, T uniform_value);

void createFloorShadow(int shadowShaderProgram, int vao);

void createFloor(int shaderProgram, int vao, GLint texture1Uniform);
