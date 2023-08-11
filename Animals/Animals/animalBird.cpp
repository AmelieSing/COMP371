
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
         legPositionL = vec3( -3 * bodySize.x / 7,
                              -1 * bodySize.y / 2,
                              -1.0f * bodySize.z / 3);
         legPositionR = vec3( -3 * bodySize.x / 7,
                              -1 * bodySize.y / 2,
                              1.0f * bodySize.z / 3);
         modelMatrix = mat4(1.0f);
      }
      Bird(int shaderProgram, int shaderShadowProgram, int vao, int sphereVAO, int sphereVertices, GLint texture1Uniform, vec3 bodyPos, float size, float yaw) {
         this->shaderProgram = shaderProgram;
         this->vao = vao;
         this->texture1Uniform = texture1Uniform;
         this-> shaderShadowProgram = shaderShadowProgram;
         this->scaleFactor = scaleFactor * size;
         this->sphereVAO = sphereVAO;
         this->sphereVertices = sphereVertices;
         this->yaw = yaw;

         bodyPosition = bodyPos;
         bodySize = vec3(2.0f, 0.5f, 1.0f);
         headPosition = vec3( bodySize.x/2,
                              bodySize.y/2,
                              0.0f);
         wingsSize = vec3( bodySize.x/3, 
                           bodySize.y/5, 
                           bodySize.x * 0.9f);
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
         modelMatrix = mat4(1.0f);
      }
      void drawShadow() {
         if(shapeSphere) {
            // BODY
            modelMatrix = 
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
            * scale(mat4(1.0f), bodySize);
            SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLES, sphereVertices, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // HEAD
            modelMatrix =
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
            * translate(mat4(1.0f), headPosition) 
            * scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f));
            SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLES, sphereVertices, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // WINGS L
            modelMatrix = 
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
            * rotate(mat4(1.0f), radians(-1.0f * wingsAngle), vec3(1.0f, 0.0f, 0.0f))
            * translate(mat4(1.0f), wingsPositionL) 
            * scale(mat4(1.0f), wingsSize);
            SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLES, sphereVertices, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // WINGS R
            modelMatrix = 
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
            * rotate(mat4(1.0f), radians(wingsAngle), vec3(1.0f, 0.0f, 0.0f))
            * translate(mat4(1.0f), wingsPositionR) 
            * scale(mat4(1.0f), wingsSize);
            SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLES, sphereVertices, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // LEGS L
            modelMatrix = 
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
            * translate(mat4(1.0f), legPositionL) 
            * scale(mat4(1.0f), vec3(0.5f, 0.2f, 0.2f));
            SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLES, sphereVertices, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // LEGS R
            modelMatrix = 
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
            * translate(mat4(1.0f), legPositionR) 
            * scale(mat4(1.0f), vec3(0.5f, 0.2f, 0.2f));
            SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLES, sphereVertices, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
         } else {
            // BODY
            modelMatrix = 
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
            * scale(mat4(1.0f), bodySize);
            SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // HEAD
            modelMatrix =
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
            * translate(mat4(1.0f), headPosition) 
            * scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f));
            SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // WINGS L
            modelMatrix = 
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
            * rotate(mat4(1.0f), radians(-1.0f * wingsAngle), vec3(1.0f, 0.0f, 0.0f))
            * translate(mat4(1.0f), wingsPositionL) 
            * scale(mat4(1.0f), wingsSize);
            SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // WINGS R
            modelMatrix = 
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
            * rotate(mat4(1.0f), radians(wingsAngle), vec3(1.0f, 0.0f, 0.0f))
            * translate(mat4(1.0f), wingsPositionR) 
            * scale(mat4(1.0f), wingsSize);
            SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // LEGS L
            modelMatrix = 
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
            * translate(mat4(1.0f), legPositionL) 
            * scale(mat4(1.0f), vec3(0.5f, 0.2f, 0.2f));
            SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // LEGS R
            modelMatrix = 
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
            * translate(mat4(1.0f), legPositionR) 
            * scale(mat4(1.0f), vec3(0.5f, 0.2f, 0.2f));
            SetUniformMat4(shaderShadowProgram, "worldMatrix", modelMatrix);

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);
         }
      }

      void draw() {
         glUseProgram(shaderProgram);
         if(shapeSphere) {
            // BODY
            modelMatrix = 
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor) 
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
            * scale(mat4(1.0f), bodySize * 0.6f);
            SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
            SetUniformVec3(shaderProgram, "customColor", vec3(1.0f, 0.0f, 0.0f));
            glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLES, sphereVertices, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // HEAD
            modelMatrix = 
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
            * translate(mat4(1.0f), headPosition) 
            * rotate(mat4(1.0f), radians(180.0f), vec3(0.0f, 1.0f, 0.0f)) 
            * rotate(mat4(1.0f), radians(-90.0f), vec3(1.0f, 0.0f, 0.0f)) 
            * scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f) * 0.7f);
            SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
            SetUniformVec3(shaderProgram, "customColor", vec3(1.0f, 1.0f, 1.0f));
            glUniform1i(texture1Uniform, 3); // Texture unit 2 is now bound to texture1

            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLES, sphereVertices, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // WINGS L
            modelMatrix =
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
            * rotate(mat4(1.0f), radians(-1.0f * wingsAngle), vec3(1.0f, 0.0f, 0.0f))
            * translate(mat4(1.0f), wingsPositionL) 
            * scale(mat4(1.0f), wingsSize * 0.5f);
            SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
            SetUniformVec3(shaderProgram, "customColor", vec3(0.0f, 0.0f, 1.0f));
            glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLES, sphereVertices, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // WINGS R
            modelMatrix =  
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
            * rotate(mat4(1.0f), radians(wingsAngle), vec3(1.0f, 0.0f, 0.0f))
            * translate(mat4(1.0f), wingsPositionR) 
            * scale(mat4(1.0f), wingsSize * 0.5f);
            SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
            SetUniformVec3(shaderProgram, "customColor", vec3(1.0f, 1.0f, 1.0f));
            glUniform1i(texture1Uniform, 2); // Texture unit 2 is now bound to texture1

            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLES, sphereVertices, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // LEGS L
            modelMatrix = 
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
            * translate(mat4(1.0f), legPositionL) 
            * scale(mat4(1.0f), vec3(0.5f, 0.2f, 0.2f) * 0.5f);
            SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
            SetUniformVec3(shaderProgram, "customColor", vec3(0.0f, 0.0f, 1.0f));
            glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLES, sphereVertices, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // LEGS R
            modelMatrix = 
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
            * translate(mat4(1.0f), legPositionR) 
            * scale(mat4(1.0f), vec3(0.5f, 0.2f, 0.2f) * 0.5f);
            SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
            SetUniformVec3(shaderProgram, "customColor", vec3(0.0f, 1.0f, 1.0f));
            glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

            glBindVertexArray(sphereVAO);
            glDrawElements(GL_TRIANGLES, sphereVertices, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
         } else {
            // BODY
            modelMatrix = 
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor) 
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
            * scale(mat4(1.0f), bodySize);
            SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
            SetUniformVec3(shaderProgram, "customColor", vec3(1.0f, 0.0f, 0.0f));
            glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // HEAD
            modelMatrix = 
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
            * translate(mat4(1.0f), headPosition) 
            // * rotate(mat4(1.0f), radians(-90.0f), vec3(0.0f, 1.0f, 0.0f)) 
            * scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f));
            SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
            SetUniformVec3(shaderProgram, "customColor", vec3(1.0f, 1.0f, 1.0f));
            glUniform1i(texture1Uniform, 3); // Texture unit 2 is now bound to texture1

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // WINGS L
            modelMatrix =
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
            * rotate(mat4(1.0f), radians(-1.0f * wingsAngle), vec3(1.0f, 0.0f, 0.0f))
            * translate(mat4(1.0f), wingsPositionL) 
            * scale(mat4(1.0f), wingsSize);
            SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
            SetUniformVec3(shaderProgram, "customColor", vec3(0.0f, 0.0f, 1.0f));
            glUniform1i(texture1Uniform, 1); // Texture unit 2 is now bound to texture1

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // WINGS R
            modelMatrix =  
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
            * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f)) 
            * rotate(mat4(1.0f), radians(wingsAngle), vec3(1.0f, 0.0f, 0.0f))
            * translate(mat4(1.0f), wingsPositionR) 
            * scale(mat4(1.0f), wingsSize);
            SetUniformMat4(shaderProgram, "worldMatrix", modelMatrix);
            SetUniformVec3(shaderProgram, "customColor", vec3(1.0f, 1.0f, 1.0f));
            glUniform1i(texture1Uniform, 2); // Texture unit 2 is now bound to texture1

            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
            glBindVertexArray(0);

            // LEGS L
            modelMatrix = 
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
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
            modelMatrix = 
            translate(mat4(1.0f), bodyPosition) 
            * scale(mat4(1.0f), scaleFactor)
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
      }
      void move() {
         bodyPosition += vec3(0.05f * cos(radians(yaw)), 0.0f, -0.05f * sin(radians(yaw)));
         yaw += 1.0f;
      }
      void moveWings() {
          if (wingsAngle == -10) {
              wingsUp = true;
          }
          else if (wingsAngle == 45) {
              wingsUp = false;
          }
          if (wingsUp) {
              wingsAngle++;
          }
          else {
              wingsAngle--;
          }
      }
      vec3 getPosition() {
         return bodyPosition;
      }
      void setShapeSphere() {
         shapeSphere = true;
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

      float wingsAngle = 45;
      bool wingsUp = false;
      float yaw = 0;
      float pitch = 0;
      float roll = 0;

      mat4 modelMatrix;
      int vao;
      int sphereVAO;
      int sphereVertices;

      bool shapeSphere = false;

      int shaderProgram;
      int shaderShadowProgram;
      GLint texture1Uniform;
};
