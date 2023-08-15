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
      this->bodyAngleY = randomInRange(0.0f, 360.0f);

      legColor = vec3(randomInRange(0.0f, 1.0f), randomInRange(0.0f, 1.0f), randomInRange(0.0f, 1.0f));
      armColor = vec3(randomInRange(0.0f, 1.0f), randomInRange(0.0f, 1.0f), randomInRange(0.0f, 1.0f));
      bodyColor = vec3(randomInRange(0.0f, 1.0f), randomInRange(0.0f, 1.0f), randomInRange(0.0f, 1.0f));
      headColor = vec3(randomInRange(0.0f, 1.0f), randomInRange(0.0f, 1.0f), randomInRange(0.0f, 1.0f));

      headPosition = vec3(xPos, height*0.8, zPos);
      headSize = vec3(0.1f * height, 0.125f * height, 0.1f * height);

      bodySize = vec3(2.5f * headSize.x,
         3.0f * headSize.y,
         0.8 * headSize.z
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
      armPositionR = vec3(1.0f * (bodySize.x * 0.5f + armSize.x * 0.5f),
         -1.0f * (0.25f * headSize.y),
         0.0f
      );
      legSize = vec3(0.45f * bodySize.x,
         4.5f * headSize.y,
         bodySize.z
      );
      legPositionL = vec3(-1.0f * (bodySize.x * 0.25f),
         -0.9f * (bodySize.y * 0.5f + legSize.y * 0.55f),
         0.0f
      );
      legPositionR = vec3(1.0f * (bodySize.x * 0.25f),
         -0.9f * (bodySize.y * 0.5f + legSize.y * 0.55f),
         0.0f
      );
      modelMatrix = mat4(1.0f);
   }

   characterObject(int shaderProgram, int shaderShadowProgram, int vao, GLint texture1Uniform, vec3 cameraPosition) {
      this->shaderProgram = shaderProgram;
      this->vao = vao;
      this->texture1Uniform = texture1Uniform;
      this->shaderShadowProgram = shaderShadowProgram;
      this->height = 10.0f;

      headSize = vec3(0.1f * height, 0.125f * height, 0.1f * height);
      headPosition = cameraPosition - vec3(0.0f, headSize.y * 0.45f, 0.0f);

      bodySize = vec3(2.5f * headSize.x,
         3.0f * headSize.y,
         0.8 * headSize.z
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
      SetUniformVec3(shaderProgram, "customColor", headColor);
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
      SetUniformVec3(shaderProgram, "customColor", bodyColor);
      glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
      glBindVertexArray(0);

      // ARMS L
      modelMatrix = scale(mat4(1.0f), scaleFactor)
         * translate(mat4(1.0f), headPosition)
         * rotate(mat4(1.0f), radians(bodyAngleY), vec3(0.0f, 1.0f, 0.0f))
         * rotate(mat4(1.0f), radians(legsAngle), vec3(1.0f, 0.0f, 0.0f))
         * translate(mat4(1.0f), bodyPosition)
         * translate(mat4(1.0f), armPositionL)
         * scale(mat4(1.0f), armSize);
      SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
      SetUniformVec3(shaderProgram, "customColor", armColor);
      glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
      glBindVertexArray(0);

      // ARMS R
      modelMatrix = scale(mat4(1.0f), scaleFactor)
         * translate(mat4(1.0f), headPosition)
         * rotate(mat4(1.0f), radians(bodyAngleY), vec3(0.0f, 1.0f, 0.0f))
         * rotate(mat4(1.0f), radians(-1.0f * armsAngle), vec3(1.0f, 0.0f, 0.0f))
         * translate(mat4(1.0f), bodyPosition)
         * translate(mat4(1.0f), armPositionR)
         * scale(mat4(1.0f), armSize);
      SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
      SetUniformVec3(shaderProgram, "customColor", armColor);
      glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
      glBindVertexArray(0);

      // LEGS L
      modelMatrix = scale(mat4(1.0f), scaleFactor)
         * translate(mat4(1.0f), headPosition)
         * rotate(mat4(1.0f), radians(bodyAngleY), vec3(0.0f, 1.0f, 0.0f))
         * translate(mat4(1.0f), bodyPosition)
         * rotate(mat4(1.0f), radians(-1.0f * legsAngle), vec3(1.0f, 0.0f, 0.0f))
         * translate(mat4(1.0f), legPositionL)
         * scale(mat4(1.0f), legSize);
      SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
      SetUniformVec3(shaderProgram, "customColor", legColor);
      glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
      glBindVertexArray(0);

      // LEGS R
      modelMatrix = scale(mat4(1.0f), scaleFactor)
         * translate(mat4(1.0f), headPosition)
         * rotate(mat4(1.0f), radians(bodyAngleY), vec3(0.0f, 1.0f, 0.0f))
         * translate(mat4(1.0f), bodyPosition)
         * rotate(mat4(1.0f), radians(legsAngle), vec3(1.0f, 0.0f, 0.0f))
         * translate(mat4(1.0f), legPositionR)
         * scale(mat4(1.0f), legSize);
      SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
      SetUniformVec3(shaderProgram, "customColor", legColor);
      glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

      glBindVertexArray(vao);
      glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
      glBindVertexArray(0);

   }
   void moveAnimation() {
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
      bodyAngleY++;
   }
   void setBodyAngle(float cameraHorizontalAngle) {
      bodyAngleY = cameraHorizontalAngle + 90.0f;
   }
   void controlMove(vec3 cameraPosition) {
      // mVelocity += velocity * dt
      headPosition = cameraPosition - vec3(0.0f, headSize.y * 0.45f, 0.0f);
   }
   void controlMove(vec3 velocity, float dt) {
      mVelocity += velocity * dt;
      // headPosition = cameraPosition - vec3(0.0f, headSize.y * 0.45f, 0.0f);
   }
   void charJump() {
      headPosition = vec3(headPosition.x, headPosition.y + 0.1f, headPosition.z);
      mVelocity = vec3(mVelocity.x, 20.0f, mVelocity.z);
      onGround = false;
   }
   void updatePos(float dt) {
      if(headPosition.y <= height) {
         if(mVelocity.y!=0)
            mVelocity = vec3(mVelocity.x, 0.0f, mVelocity.z);
            onGround = true;
      } else {
         updateGravity(dt);
      }
      airResistance(dt);
      headPosition += dt * mVelocity;
   }
   void updateGravity(float dt) {
      mVelocity += dt * vec3(0.0f, -1.0f * gravity, 0.0f);
   }
   void airResistance(float dt) {
      if(mVelocity.x > 0) {
         mVelocity.x -= 0.1f;
      } else if(mVelocity.x < 0) {
         mVelocity.x += 0.1f;
      }
      if(mVelocity.z > 0) {
         mVelocity.z -= 0.1f;
      } else if(mVelocity.z < 0) {
         mVelocity.z += 0.1f;
      }
   }
   vec3 getHeadPosition() {
      return headPosition;
   }
   bool getOnGround() {
      return onGround;
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

   vec3 legColor = vec3(0.7f, 0.5f, 0.3f);
   vec3 bodyColor = vec3(0.3f, 0.7f, 0.5f);
   vec3 armColor = vec3(0.5f, 0.3f, 0.7f);
   vec3 headColor = vec3(1.0f, 1.0f, 1.0f);

   vec3 scaleFactor = vec3(1.0f, 1.0f, 1.0f);

   vec3 mVelocity = vec3(0.0f, 0.0f, 0.0f);
   float gravity = 9.81;

   float bodyAngleY = 0;
   float armsAngle = 0;
   bool armsUp = false;
   float legsAngle = 0;
   bool legsUp = false;

   bool onGround = false;

   mat4 modelMatrix;
   int vao;
   int shaderProgram;
   int shaderShadowProgram;
   GLint texture1Uniform;
};