#pragma once
#include "functions.h"
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

// shader variable setters
void SetUniformMat4(GLuint shader_id, const char* uniform_name, mat4 uniform_value) {
    glUseProgram(shader_id);
    glUniformMatrix4fv(glGetUniformLocation(shader_id, uniform_name), 1, GL_FALSE, &uniform_value[0][0]);

}
void SetUniformVec3(GLuint shader_id, const char* uniform_name, vec3 uniform_value) {
    glUseProgram(shader_id);
    glUniform3fv(glGetUniformLocation(shader_id, uniform_name), 1, value_ptr(uniform_value));

}
template <class T>
void SetUniform1Value(GLuint shader_id, const char* uniform_name, T uniform_value) {
  glUseProgram(shader_id);
  glUniform1i(glGetUniformLocation(shader_id, uniform_name), uniform_value);
  glUseProgram(0);
}

void createFloorShadow(int shadowShaderProgram, int vao) {
   mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) 
                * scale(mat4(1.0f), vec3(100.0f, 0.5f, 100.0f));

   SetUniformMat4(shadowShaderProgram, "worldMatrix", groundWorldMatrix);

   glBindVertexArray(vao);
   glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
   glBindVertexArray(0);
}

void createFloor(int shaderProgram, int vao, GLint texture1Uniform) {
   mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f)) 
                * scale(mat4(1.0f), vec3(100.0f, 0.5f, 100.0f));
   SetUniformMat4(shaderProgram, "worldMatrix", groundWorldMatrix);
   SetUniformVec3(shaderProgram, "customColor", vec3(1.0f, 1.0f, 1.0f));
   
   glUniform1i(texture1Uniform, 2); // Texture unit 2 is now bound to texture1

   glBindVertexArray(vao);
   glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
   glBindVertexArray(0);
}