
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

void SetUniformMat4(GLuint shader_id, const char* uniform_name, mat4 uniform_value);
void SetUniformVec3(GLuint shader_id, const char* uniform_name, vec3 uniform_value);
template <class T>
void SetUniform1Value(GLuint shader_id, const char* uniform_name, T uniform_value);


class Bird {
   public:
      Bird() {
         bodyPosition = vec3(0.0f, 0.0f, 0.0f);
         bodySize = vec3(2.0f, 0.5f, 1.0f);
         headPosition = vec3( bodyPosition.x + bodySize.x/2 + 0.5f,
                              bodyPosition.y + bodySize.y/2,
                              bodyPosition.z);
         wingsSize = vec3( bodySize.x/2, 
                           bodySize.y/5, 
                           bodySize.x);
         wingsPositionL = vec3(  bodySize.x/4, 
                                 bodySize.y/2, 
                                 -1.0f * wingsSize.z/2);
         wingsPositionR = vec3(  bodySize.x/4, 
                                 bodySize.y/2, 
                                 1.0f * wingsSize.z/2);
      }
      Bird(int shaderProgram, int shaderShadowProgram, int vao, GLint texture1Uniform, vec3 bodyPos   ) {
         this->shaderProgram = shaderProgram;
         this->vao = vao;
         this->texture1Uniform = texture1Uniform;
         this-> shaderShadowProgram = shaderShadowProgram;

         bodyPosition = bodyPos;
         bodySize = vec3(2.0f, 0.5f, 1.0f);
         headPosition = vec3( bodySize.x/2,
                              bodySize.y/2,
                              bodyPosition.z);
         wingsSize = vec3( bodySize.x/3, 
                           bodySize.y/5, 
                           bodySize.x);
         wingsPositionL = vec3(  bodySize.x/5, 
                                 bodySize.y/2, 
                                 -1.0f * wingsSize.z/2);
         wingsPositionR = vec3(  bodySize.x/5, 
                                 bodySize.y/2, 
                                 1.0f * wingsSize.z/2);
         legPositionL = vec3( -3 * bodySize.x/7, 
                              -1 * bodySize.y/2, 
                              -1.0f * bodySize.z/3);
         legPositionR = vec3( -3 * bodySize.x/7, 
                              -1 * bodySize.y/2, 
                              1.0f * bodySize.z/3);
      }
      void drawShadow() {
         // BODY
         modelMatrix = scale(mat4(1.0f), scaleFactor) 
         * translate(mat4(1.0f), bodyPosition) 
         * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
         * scale(mat4(1.0f), bodySize);
         SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

         glBindVertexArray(vao);
         glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
         glBindVertexArray(0);

         // HEAD
         modelMatrix = scale(mat4(1.0f), scaleFactor) 
         * translate(mat4(1.0f), bodyPosition)
         * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
         * translate(mat4(1.0f), headPosition) 
         * scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f));
         SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

         glBindVertexArray(vao);
         glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
         glBindVertexArray(0);

         // WINGS L
         modelMatrix = scale(mat4(1.0f), scaleFactor) 
         * translate(mat4(1.0f), bodyPosition)
         * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
         * translate(mat4(1.0f), wingsPositionL) 
         * scale(mat4(1.0f), wingsSize);
         SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

         glBindVertexArray(vao);
         glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
         glBindVertexArray(0);

         // WINGS R
         modelMatrix = scale(mat4(1.0f), scaleFactor) 
         * translate(mat4(1.0f), bodyPosition)
         * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
         * translate(mat4(1.0f), wingsPositionR) 
         * scale(mat4(1.0f), wingsSize);
         SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

         glBindVertexArray(vao);
         glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
         glBindVertexArray(0);

         // LEGS L
         modelMatrix = scale(mat4(1.0f), scaleFactor) 
         * translate(mat4(1.0f), bodyPosition)
         * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
         * translate(mat4(1.0f), legPositionL) 
         * scale(mat4(1.0f), vec3(0.5f, 0.2f, 0.2f));
         SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

         glBindVertexArray(vao);
         glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
         glBindVertexArray(0);

         // LEGS R
         modelMatrix = scale(mat4(1.0f), scaleFactor) 
         * translate(mat4(1.0f), bodyPosition)
         * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
         * translate(mat4(1.0f), legPositionR) 
         * scale(mat4(1.0f), vec3(0.5f, 0.2f, 0.2f));
         SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

         glBindVertexArray(vao);
         glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
         glBindVertexArray(0);
      }

      void draw() {
         glUseProgram(shaderProgram);
         // BODY
         modelMatrix = scale(mat4(1.0f), scaleFactor) 
         * translate(mat4(1.0f), bodyPosition) 
         * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
         * scale(mat4(1.0f), bodySize);
         SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
         SetUniformVec3(shaderProgram, "customColor", vec3(1.0f, 0.0f, 0.0f));
         glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

         glBindVertexArray(vao);
         glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
         glBindVertexArray(0);

         // HEAD
         modelMatrix = scale(mat4(1.0f), scaleFactor) 
         * translate(mat4(1.0f), bodyPosition)
         * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
         * translate(mat4(1.0f), headPosition) 
         * scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f));
         SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
         SetUniformVec3(shaderProgram, "customColor", vec3(0.0f, 1.0f, 0.0f));
         glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

         glBindVertexArray(vao);
         glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
         glBindVertexArray(0);

         // WINGS L
         modelMatrix = scale(mat4(1.0f), scaleFactor) 
         * translate(mat4(1.0f), bodyPosition)
         * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
         * translate(mat4(1.0f), wingsPositionL) 
         * scale(mat4(1.0f), wingsSize);
         SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
         SetUniformVec3(shaderProgram, "customColor", vec3(0.0f, 0.0f, 1.0f));
         glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

         glBindVertexArray(vao);
         glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
         glBindVertexArray(0);

         // WINGS R
         modelMatrix = scale(mat4(1.0f), scaleFactor) 
         * translate(mat4(1.0f), bodyPosition)
         * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
         * translate(mat4(1.0f), wingsPositionR) 
         * scale(mat4(1.0f), wingsSize);
         SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
         SetUniformVec3(shaderProgram, "customColor", vec3(0.0f, 1.0f, 1.0f));
         glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

         glBindVertexArray(vao);
         glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
         glBindVertexArray(0);

         // LEGS L
         modelMatrix = scale(mat4(1.0f), scaleFactor) 
         * translate(mat4(1.0f), bodyPosition)
         * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
         * translate(mat4(1.0f), legPositionL) 
         * scale(mat4(1.0f), vec3(0.5f, 0.2f, 0.2f));
         SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
         SetUniformVec3(shaderProgram, "customColor", vec3(0.0f, 0.0f, 1.0f));
         glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

         glBindVertexArray(vao);
         glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
         glBindVertexArray(0);

         // LEGS R
         modelMatrix = scale(mat4(1.0f), scaleFactor) 
         * translate(mat4(1.0f), bodyPosition)
         * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
         * translate(mat4(1.0f), legPositionR) 
         * scale(mat4(1.0f), vec3(0.5f, 0.2f, 0.2f));
         SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
         SetUniformVec3(shaderProgram, "customColor", vec3(0.0f, 1.0f, 1.0f));
         glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

         glBindVertexArray(vao);
         glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
         glBindVertexArray(0);
      }
      void move() {
         bodyPosition += vec3(0.05f * cos(radians(yaw)), 0.0f, -0.05f * sin(radians(yaw)));
         yaw += 1.0f;
      }
   private:
      vec3 bodyPosition;
      vec3 bodySize;
      vec3 headPosition;
      vec3 wingsPositionL;
      vec3 wingsPositionR;
      vec3 wingsSize;
      vec3 legPositionL;
      vec3 legPositionR;
      vec3 scaleFactor = vec3(3.0f, 3.0f, 3.0f);

      float yaw = 0;
      float pitch = 0;
      float roll = 0;

      mat4 modelMatrix;
      int vao;
      int shaderProgram;
      int shaderShadowProgram;
      GLint texture1Uniform;

      
};

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