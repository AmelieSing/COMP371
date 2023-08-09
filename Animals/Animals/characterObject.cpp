#include <iostream>
#include <vector>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/gtc/type_ptr.hpp>

#include "functions.cpp"

using namespace glm;
using namespace std;

class characterObject {
public:
   characterObject() {
      
   }
   characterObject(int shaderProgram, int shaderShadowProgram, int vao, GLint texture1Uniform, float height, float xPos, float zPos) {
      this->shaderProgram = shaderProgram;
      this->vao = vao;
      this->texture1Uniform = texture1Uniform;
      this->shaderShadowProgram = shaderShadowProgram;
      this->height = height;

      headPosition = vec3(xPos, height, zPos);
      headSize = vec3(0.1f * height, 0.125f * height, 0.1f * height);

      bodySize = vec3(2.5f * headSize.x,
         3.2f * headSize.y,
         1.2 * headSize.z
      );
      bodyPosition = vec3(0.0f,
         -1.0f * ((bodySize.y * 0.5f) + (headSize.y * 0.5f)),
         0.0f
      );
      armSize = vec3(0.4f * bodySize.x,
         3.7f * headSize.y,
         bodySize.z
      );
      armPositionL = vec3(-1.0f * (bodySize.x * 0.5f + armSize.x * 0.5f),
         -1.0f * (0.25f * headSize.y),
         0.0f
      );
      armPositionR = vec3( 1.0f * (bodySize.x * 0.5f + armSize.x * 0.5f),
         -1.0f *  (0.25f * headSize.y),
         0.0f
      );
      legSize = vec3(0.45f * bodySize.x,
         4.0f * headSize.y,
         bodySize.z
      );
      legPositionL = vec3(-1.0f *  (bodySize.x * 0.25f),
         -0.9f * (bodySize.y * 0.5f + legSize.y * 0.5f),
         0.0f
      );
      legPositionR = vec3(1.0f * (bodySize.x * 0.25f),
         -0.9f * (bodySize.y * 0.5f + legSize.y * 0.5f),
         0.0f
      );
      modelMatrix = mat4(1.0f);
   }

   characterObject(int shaderProgram, int shaderShadowProgram, int vao, GLint texture1Uniform, vec3 cameraPosition) {
      this->shaderProgram = shaderProgram;
      this->vao = vao;
      this->texture1Uniform = texture1Uniform;
      this->shaderShadowProgram = shaderShadowProgram;
      this->height = cameraPosition.y;

      headPosition = cameraPosition;
      headSize = vec3(0.1f * height, 0.125f * height, 0.1f * height);

      bodySize = vec3(2.5f * headSize.x,
         3.0f * headSize.y,
         1.2 * headSize.z
      );
      bodyPosition = vec3(0.0f,
         -1.0f * ((bodySize.y * 0.5f) + (headSize.y * 0.5f)),
         0.0f
      );
      armSize = vec3(0.4f * bodySize.x,
         3.5f * headSize.y,
         bodySize.z
      );
      armPositionL = vec3(-1.0f * (bodySize.x * 0.5f + armSize.x * 0.5f),
         -1.0f * (0.25f * headSize.y),
         0.0f
      );
      armPositionR = vec3( 1.0f * (bodySize.x * 0.5f + armSize.x * 0.5f),
         -1.0f *  (0.25f * headSize.y),
         0.0f
      );
      legSize = vec3(0.45f * bodySize.x,
         4.0f * headSize.y,
         bodySize.z
      );
      legPositionL = vec3(-1.0f *  (bodySize.x * 0.25f),
         -0.9f * (bodySize.y * 0.5f + legSize.y * 0.5f),
         0.0f
      );
      legPositionR = vec3(1.0f * (bodySize.x * 0.25f),
         -0.9f * (bodySize.y * 0.5f + legSize.y * 0.5f),
         0.0f
      );
      modelMatrix = mat4(1.0f);
   }

   void drawShadow() {
      // HEAD
      modelMatrix = scale(mat4(1.0f), scaleFactor)
         * translate(mat4(1.0f), headPosition)
         * scale(mat4(1.0f), headSize);
      SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
      glBindVertexArray(0);

      // BODY
      modelMatrix = scale(mat4(1.0f), scaleFactor)
         * translate(mat4(1.0f), headPosition)
         * translate(mat4(1.0f), bodyPosition)
         * rotate(mat4(1.0f), radians(bodyAngleY), vec3(0.0f, 1.0f, 0.0f))
         * scale(mat4(1.0f), bodySize);
      SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
      glBindVertexArray(0);

      // ARMS L
      modelMatrix = scale(mat4(1.0f), scaleFactor)
         * translate(mat4(1.0f), headPosition)
         * translate(mat4(1.0f), bodyPosition)
         * translate(mat4(1.0f), armPositionL)
         * rotate(mat4(1.0f), radians(bodyAngleY), vec3(0.0f, 1.0f, 0.0f))
         * scale(mat4(1.0f), armSize);
      SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
      glBindVertexArray(0);

      // ARMS R
      modelMatrix = scale(mat4(1.0f), scaleFactor)
         * translate(mat4(1.0f), headPosition)
         * translate(mat4(1.0f), bodyPosition)
         * translate(mat4(1.0f), armPositionR)
         * rotate(mat4(1.0f), radians(bodyAngleY), vec3(0.0f, 1.0f, 0.0f))
         * scale(mat4(1.0f), armSize);
      SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
      glBindVertexArray(0);

      // LEGS L
      modelMatrix = scale(mat4(1.0f), scaleFactor)
         * translate(mat4(1.0f), headPosition)
         * translate(mat4(1.0f), bodyPosition)
         * translate(mat4(1.0f), legPositionL)
         * rotate(mat4(1.0f), radians(bodyAngleY), vec3(0.0f, 1.0f, 0.0f))
         * scale(mat4(1.0f), legSize);
      SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
      glBindVertexArray(0);

      // LEGS R
      modelMatrix = scale(mat4(1.0f), scaleFactor)
         * translate(mat4(1.0f), headPosition)
         * translate(mat4(1.0f), bodyPosition)
         * translate(mat4(1.0f), legPositionR)
         * rotate(mat4(1.0f), radians(bodyAngleY), vec3(0.0f, 1.0f, 0.0f))
         * scale(mat4(1.0f), legSize);
      SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
      glBindVertexArray(0);
   }

   void draw() {
      glUseProgram(shaderProgram);
      // HEAD
      modelMatrix = scale(mat4(1.0f), scaleFactor)
         * translate(mat4(1.0f), headPosition)
         * rotate(mat4(1.0f), radians(bodyAngleY), vec3(0.0f, 1.0f, 0.0f))
         * scale(mat4(1.0f), headSize);
      SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
      SetUniformVec3(shaderProgram, "customColor", vec3(1.0f, 0.0f, 0.0f));
      glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
      glBindVertexArray(0);

      // BODY
      modelMatrix = scale(mat4(1.0f), scaleFactor)
         * translate(mat4(1.0f), headPosition)
         * translate(mat4(1.0f), bodyPosition)
         * rotate(mat4(1.0f), radians(bodyAngleY), vec3(0.0f, 1.0f, 0.0f))
         * scale(mat4(1.0f), bodySize);
      SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
      SetUniformVec3(shaderProgram, "customColor", vec3(1.0f, 0.0f, 1.0f));
      glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
      glBindVertexArray(0);

      // ARMS L
      modelMatrix = scale(mat4(1.0f), scaleFactor)
         * translate(mat4(1.0f), headPosition)
         * rotate(mat4(1.0f), radians(legsAngle), vec3(1.0f, 0.0f, 0.0f))
         * translate(mat4(1.0f), bodyPosition)
         * rotate(mat4(1.0f), radians(bodyAngleY), vec3(0.0f, 1.0f, 0.0f))
         * translate(mat4(1.0f), armPositionL)
         * scale(mat4(1.0f), armSize);
      SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
      SetUniformVec3(shaderProgram, "customColor", vec3(1.0f, 0.0f, 0.0f));
      glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
      glBindVertexArray(0);

      // ARMS R
      modelMatrix = scale(mat4(1.0f), scaleFactor)
         * translate(mat4(1.0f), headPosition)
         * rotate(mat4(1.0f), radians(-1.0f * armsAngle), vec3(1.0f, 0.0f, 0.0f))
         * translate(mat4(1.0f), bodyPosition)
         * rotate(mat4(1.0f), radians(bodyAngleY), vec3(0.0f, 1.0f, 0.0f))
         * translate(mat4(1.0f), armPositionR)
         * scale(mat4(1.0f), armSize);
      SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
      SetUniformVec3(shaderProgram, "customColor", vec3(1.0f, 0.0f, 0.0f));
      glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
      glBindVertexArray(0);

      // LEGS L
      modelMatrix = scale(mat4(1.0f), scaleFactor)
         * translate(mat4(1.0f), headPosition)
         * rotate(mat4(1.0f), radians(-1.0f * legsAngle), vec3(1.0f, 0.0f, 0.0f))
         * translate(mat4(1.0f), bodyPosition)
         * rotate(mat4(1.0f), radians(bodyAngleY), vec3(0.0f, 1.0f, 0.0f))
         * translate(mat4(1.0f), legPositionL)
         * scale(mat4(1.0f), legSize);
      SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
      SetUniformVec3(shaderProgram, "customColor", vec3(1.0f, 0.0f, 0.0f));
      glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
      glBindVertexArray(0);

      // LEGS R
      modelMatrix = scale(mat4(1.0f), scaleFactor)
         * translate(mat4(1.0f), headPosition)
         * rotate(mat4(1.0f), radians(legsAngle), vec3(1.0f, 0.0f, 0.0f))
         * translate(mat4(1.0f), bodyPosition)
         * rotate(mat4(1.0f), radians(bodyAngleY), vec3(0.0f, 1.0f, 0.0f))
         * translate(mat4(1.0f), legPositionR)
         * scale(mat4(1.0f), legSize);
      SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
      SetUniformVec3(shaderProgram, "customColor", vec3(1.0f, 0.0f, 0.0f));
      glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
      glBindVertexArray(0);


   }
   void moveAnimation() {
      // headPosition += vec3(0.05f * cos(radians(bodyAngleY)), 0.0f, -0.05f * sin(radians(bodyAngleY)));
      if(legsAngle == -25) {
         legsUp = true;
      } else if(legsAngle == 25) {
         legsUp = false;
      }
      if(legsUp) {
         legsAngle++;
      } else {
         legsAngle--;
      }
      if(armsAngle == -25) {
         armsUp = true;
      } else if(armsAngle == 25) {
         armsUp = false;
      }
      if(armsUp) {
         armsAngle++;
      } else {
         armsAngle--;
      }
   }
   void move() {
      headPosition += vec3(0.0f, 0.0f, -0.01f);
   }
   void rotateSelf() {
      bodyAngleY += 1.0f;
   }
   void controlMove(vec3 movement) {

   }

private:
   float height;
   vec3 headPosition;
   vec3 headSize;

   vec3 bodySize;
   vec3 bodyPosition;
   

   vec3 armSize;
   vec3 armPositionL;
   vec3 armPositionR;

   vec3 legSize;
   vec3 legPositionL;
   vec3 legPositionR;

   vec3 scaleFactor = vec3(1.0f, 1.0f, 1.0f);

   float bodyAngleY = 0;
   float armsAngle = 0;
   bool armsUp = false;
   float legsAngle = 0;
   bool legsUp = false;

   mat4 modelMatrix;
   int vao;
   int shaderProgram;
   int shaderShadowProgram;
   GLint texture1Uniform;
};