#include "Ant.h"
#include <random>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // GLFW provides a cross-platform interface for creating a graphical context,
// initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/gtc/type_ptr.hpp>
//#include "functions.cpp"

using namespace glm;
using namespace std;

Ant::Ant() {
    this->bodyPosition = vec3(15.0f, 1.0f, 2.0f);
    this->bodySize = vec3(0.5f, 0.5f, 0.5f);
    this->headPosition = vec3(bodyPosition.x + bodySize.x / 2 + 0.5f,
        bodyPosition.y + bodySize.y / 2 + 0.5f,
        bodyPosition.z);
    this->legSize = vec3(bodySize.x,
        bodySize.y * 2,
        bodySize.z);
    this->tailPosition = vec3(bodyPosition.x - bodySize.x / 2 - 0.75f,
        bodyPosition.y,
        bodyPosition.z);
    this->eyePositionL = vec3(headPosition.x + 0.4f,
        headPosition.y + 0.25f,
        bodyPosition.z - 0.25f);
    this->eyePositionR = vec3(headPosition.x + 0.4f,
        headPosition.y + 0.25f,
        bodyPosition.z + 0.25f);

    this->leg1PositionL = vec3(bodyPosition.x,
        bodyPosition.y - 0.5f,
        bodyPosition.z - 1.0f);
    this->leg1PositionR = vec3(bodyPosition.x,
        bodyPosition.y - 0.5f,
        bodyPosition.z + 1.0f);

    this->leg2PositionL = vec3(tailPosition.x + 0.75f / 2.0f,
        bodyPosition.y - 0.5f,
        bodyPosition.z - 1.0f);
    this->leg2PositionR = vec3(tailPosition.x + 0.75f / 2.0f,
        bodyPosition.y - 0.5f,
        bodyPosition.z + 1.0f);

    this->leg3PositionL = vec3(tailPosition.x - 0.15f,
        bodyPosition.y - 0.5f,
        bodyPosition.z - 1.0f);
    this->leg3PositionR = vec3(tailPosition.x - 0.15f,
        bodyPosition.y - 0.5f,
        bodyPosition.z + 1.0f);

    this->antennaPositionL = vec3(headPosition.x + 0.35f,
        headPosition.y + 0.45f,
        bodyPosition.z - 0.25f);
    this->antennaPositionR = vec3(headPosition.x + 0.35f,
        headPosition.y + 0.45f,
        bodyPosition.z + 0.25f);

    this->antenna2PositionL = vec3(antennaPositionL.x + 0.6f,
        antennaPositionL.y + 0.4f,
        antennaPositionL.z);
    this->antenna2PositionR = vec3(antennaPositionR.x + 0.6f,
        antennaPositionR.y + 0.4f,
        antennaPositionR.z);

    this->rotationPointAntennaL = vec3(antennaPositionL.x + 0.3f,
        antennaPositionL.y + 0.4f,
        antennaPositionL.z);
    this->rotationPointAntennaR = vec3(antennaPositionR.x + 0.3f,
        antennaPositionR.y + 0.4f,
        antennaPositionR.z);

    this->antennaAngle = -90.0f;
    this->antennaUp = false;
    this->antennaDown = false;
    this->goingDown = false;
    this->goingUp = true;

    this->textureID = textureID;
    this->defaultID = defaultID;

    this->modelMatrix = mat4(1.0f);
}
Ant::Ant(int shaderProgram, int shaderShadowProgram, int sphereVao, int cubeVao, GLint texture1Uniform, vec3 bodyPos, int vertex, GLuint textureID, GLuint defaultID) {
    this->shaderProgram = shaderProgram;
    this->sphereVao = sphereVao;
    this->cubeVao = cubeVao;
    this->texture1Uniform = texture1Uniform;
    this->shaderShadowProgram = shaderShadowProgram;
    this->vertex = vertex;
    
    this->bodyPosition = bodyPos;
    this->bodySize = vec3(0.25f, 0.25f, 0.25f);
    this->headPosition = vec3(bodyPosition.x + bodySize.x / 2 + 0.5f,
        bodyPosition.y + bodySize.y / 2 + 0.5f,
        bodyPosition.z);
    this->legSize = vec3(bodySize.x/2,
        bodySize.y * 5,
        bodySize.z/2);
    this->tailPosition = vec3(bodyPosition.x - bodySize.x / 2 - 0.75f,
        bodyPosition.y,
        bodyPosition.z);
    
    this->eyePositionL = vec3(headPosition.x + 0.4f,
        headPosition.y + 0.25f,
        bodyPosition.z - 0.25f);
    this->eyePositionR = vec3(headPosition.x + 0.4f,
        headPosition.y + 0.25f,
        bodyPosition.z + 0.25f);
    
    this->leg1PositionL = vec3(bodyPosition.x,
        bodyPosition.y - 0.5f,
        bodyPosition.z - 1.0f);
    this->leg1PositionR = vec3(bodyPosition.x,
        bodyPosition.y - 0.5f,
        bodyPosition.z + 1.0f);

    this->leg2PositionL = vec3(tailPosition.x + 0.75f/2.0f,
        bodyPosition.y - 0.5f,
        bodyPosition.z - 1.0f);
    this->leg2PositionR = vec3(tailPosition.x + 0.75f / 2.0f,
        bodyPosition.y - 0.5f,
        bodyPosition.z + 1.0f);

    this->leg3PositionL = vec3(tailPosition.x - 0.15f,
        bodyPosition.y - 0.5f,
        bodyPosition.z - 1.0f);
    this->leg3PositionR = vec3(tailPosition.x - 0.15f,
        bodyPosition.y - 0.5f,
        bodyPosition.z + 1.0f);

    this->antennaSize = vec3(bodySize.x / 2,
        bodySize.y * 2,
        bodySize.z / 2);

    this->antennaPositionL = vec3(headPosition.x + 0.35f,
        headPosition.y + 0.45f,
        bodyPosition.z - 0.25f);
    this->antennaPositionR = vec3(headPosition.x + 0.35f,
        headPosition.y + 0.45f,
        bodyPosition.z + 0.25f);

    this->antenna2PositionL = vec3(antennaPositionL.x + 0.6f,
        antennaPositionL.y +0.4f,
        antennaPositionL.z);
    this->antenna2PositionR = vec3(antennaPositionR.x + 0.6f,
        antennaPositionR.y + 0.4f,
        antennaPositionR.z);

    this->rotationPointAntennaL = vec3(antenna2PositionL.x -0.25f,
        antennaPositionL.y + 0.4f,
        antennaPositionL.z);
    this->rotationPointAntennaR = vec3(antenna2PositionR.x - 0.25f,
        antennaPositionR.y + 0.4f,
        antennaPositionR.z);

    this->antennaAngle = -90.0f;
    this->antennaUp = false;
    this->antennaDown = false;
    this->goingDown = false;
    this->goingUp = true;

    this->textureID = textureID;
    this->defaultID = defaultID;

    this->modelMatrix = mat4(1.0f);
}
void Ant::drawShadow() {
    glUseProgram(shaderShadowProgram);
    glBindVertexArray(sphereVao);
    // BODY
    modelMatrix = scale(mat4(1.0f), scaleFactor)
        * translate(mat4(1.0f), bodyPosition)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * scale(mat4(1.0f), bodySize);
    SetUniformMat4Ant(shaderShadowProgram, "worldMatrix", modelMatrix);

    glDrawElements(GL_TRIANGLES, vertex, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // HEAD
    glBindVertexArray(sphereVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
    //    * translate(mat4(1.0f), bodyPosition)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * translate(mat4(1.0f), headPosition)
        * scale(mat4(1.0f), vec3(0.2f, 0.2f, 0.2f));
    SetUniformMat4Ant(shaderShadowProgram, "worldMatrix", modelMatrix);

    glDrawElements(GL_TRIANGLES, vertex, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Tail
    glBindVertexArray(sphereVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
   //     * translate(mat4(1.0f), bodyPosition)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * translate(mat4(1.0f), tailPosition)
        * scale(mat4(1.0f), vec3(0.5f, 0.5f, 0.5f));
    SetUniformMat4Ant(shaderShadowProgram, "worldMatrix", modelMatrix);

    glDrawElements(GL_TRIANGLES, vertex, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Eye L
    glBindVertexArray(sphereVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
        //     * translate(mat4(1.0f), bodyPosition)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * translate(mat4(1.0f), eyePositionL)
        * scale(mat4(1.0f), vec3(0.05f, 0.05f, 0.05f));
    SetUniformMat4Ant(shaderShadowProgram, "worldMatrix", modelMatrix);

    glDrawElements(GL_TRIANGLES, vertex, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Eye R
    glBindVertexArray(sphereVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
        //     * translate(mat4(1.0f), bodyPosition)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * translate(mat4(1.0f), eyePositionR)
        * scale(mat4(1.0f), vec3(0.05f, 0.05f, 0.05f));
    SetUniformMat4Ant(shaderShadowProgram, "worldMatrix", modelMatrix);

    glDrawElements(GL_TRIANGLES, vertex, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // LEG1 L
    glBindVertexArray(cubeVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
        //      * translate(mat4(1.0f), bodyPosition) 
        * translate(mat4(1.0f), leg1PositionL)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * rotate(mat4(1.0f), radians(45.0f), vec3(1.0f, 0.0f, 0.0f))

        * scale(mat4(1.0f), vec3(legSize));
    SetUniformMat4Ant(shaderShadowProgram, "worldMatrix", modelMatrix);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);

    // LEG1 R
    glBindVertexArray(cubeVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
        //       * translate(mat4(1.0f), bodyPosition)
        * translate(mat4(1.0f), leg1PositionR)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * rotate(mat4(1.0f), radians(-45.0f), vec3(1.0f, 0.0f, 0.0f))

        * scale(mat4(1.0f), vec3(legSize));
    SetUniformMat4Ant(shaderShadowProgram, "worldMatrix", modelMatrix);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);


    // LEG2 L
    glBindVertexArray(cubeVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
        // * translate(mat4(1.0f), bodyPosition)
        * translate(mat4(1.0f), leg2PositionL)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * rotate(mat4(1.0f), radians(45.0f), vec3(1.0f, 0.0f, 0.0f))

        * scale(mat4(1.0f), vec3(legSize));
    SetUniformMat4Ant(shaderShadowProgram, "worldMatrix", modelMatrix);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);

    // LEG2 R
    glBindVertexArray(cubeVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
        //   * translate(mat4(1.0f), bodyPosition)
        * translate(mat4(1.0f), leg2PositionR)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * rotate(mat4(1.0f), radians(-45.0f), vec3(1.0f, 0.0f, 0.0f))

        * scale(mat4(1.0f), vec3(legSize));
    SetUniformMat4Ant(shaderShadowProgram, "worldMatrix", modelMatrix);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);

    // LEG3 L
    glBindVertexArray(cubeVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
        //  * translate(mat4(1.0f), bodyPosition)
        * translate(mat4(1.0f), leg3PositionL)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * rotate(mat4(1.0f), radians(45.0f), vec3(1.0f, 0.0f, 0.0f))

        * scale(mat4(1.0f), vec3(legSize));
    SetUniformMat4Ant(shaderShadowProgram, "worldMatrix", modelMatrix);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);

    // Antenna L
    glBindVertexArray(cubeVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
        //  * translate(mat4(1.0f), bodyPosition)
        * translate(mat4(1.0f), antennaPositionL)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * rotate(mat4(1.0f), radians(-45.0f), vec3(0.0f, 0.0f, 1.0f))

        * scale(mat4(1.0f), vec3(legSize));
    SetUniformMat4Ant(shaderShadowProgram, "worldMatrix", modelMatrix);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);

    // Antenna R
    glBindVertexArray(cubeVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
        //  * translate(mat4(1.0f), bodyPosition)
        * translate(mat4(1.0f), antennaPositionR)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * rotate(mat4(1.0f), radians(-45.0f), vec3(0.0f, 0.0f, 1.0f))

        * scale(mat4(1.0f), vec3(legSize));
    SetUniformMat4Ant(shaderShadowProgram, "worldMatrix", modelMatrix);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);

    // Antenna 2 L
    glBindVertexArray(cubeVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
        //  * translate(mat4(1.0f), bodyPosition)
        * translate(mat4(1.0f), antenna2PositionL)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * rotate(mat4(1.0f), radians(antennaAngle), vec3(0.0f, 0.0f, 1.0f))

        * scale(mat4(1.0f), vec3(antennaSize));
    SetUniformMat4Ant(shaderShadowProgram, "worldMatrix", modelMatrix);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);

    // Antenna 2 R
    glBindVertexArray(cubeVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
        //  * translate(mat4(1.0f), bodyPosition)
        * translate(mat4(1.0f), antenna2PositionR)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * rotate(mat4(1.0f), radians(antennaAngle), vec3(0.0f, 0.0f, 1.0f))

        * scale(mat4(1.0f), vec3(antennaSize));
    SetUniformMat4Ant(shaderShadowProgram, "worldMatrix", modelMatrix);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);
}

void Ant::draw() {
    glUseProgram(shaderProgram);
    glBindVertexArray(sphereVao);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(texture1Uniform, 6);
    // BODY

    modelMatrix = scale(mat4(1.0f), scaleFactor)
        * translate(mat4(1.0f), bodyPosition)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * scale(mat4(1.0f), bodySize);
    SetUniformMat4Ant(shaderProgram, "worldMatrix", modelMatrix);
    SetUniformVec3Ant(shaderProgram, "customColor", color);

    glDrawElements(GL_TRIANGLES, vertex, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // HEAD
     glBindVertexArray(sphereVao);
     modelMatrix = scale(mat4(1.0f), scaleFactor)
  //      * translate(mat4(1.0f), bodyPosition)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * translate(mat4(1.0f), headPosition)
        * scale(mat4(1.0f), vec3(0.2f, 0.2f, 0.2f));
    SetUniformMat4Ant(shaderProgram, "worldMatrix", modelMatrix);
    SetUniformVec3Ant(shaderProgram, "customColor", color);

    glDrawElements(GL_TRIANGLES, vertex, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Tail
    glBindVertexArray(sphereVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
  //      * translate(mat4(1.0f), bodyPosition)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * translate(mat4(1.0f), tailPosition)
        * scale(mat4(1.0f), vec3(bodySize));
    SetUniformMat4Ant(shaderProgram, "worldMatrix", modelMatrix);
    SetUniformVec3Ant(shaderProgram, "customColor", color);

    glDrawElements(GL_TRIANGLES, vertex, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, defaultID);
    glUniform1i(texture1Uniform, 1);
    // Eye L
    glBindVertexArray(sphereVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
        //     * translate(mat4(1.0f), bodyPosition)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * translate(mat4(1.0f), eyePositionL)
        * scale(mat4(1.0f), vec3(0.05f, 0.05f, 0.05f));
    SetUniformMat4Ant(shaderProgram, "worldMatrix", modelMatrix);
    SetUniformVec3Ant(shaderProgram, "customColor", vec3(0.0f, 0.0f, 0.0f));

    glDrawElements(GL_TRIANGLES, vertex, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // Eye R
    glBindVertexArray(sphereVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
        //     * translate(mat4(1.0f), bodyPosition)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * translate(mat4(1.0f), eyePositionR)
        * scale(mat4(1.0f), vec3(0.05f, 0.05f, 0.05f));
    SetUniformMat4Ant(shaderProgram, "worldMatrix", modelMatrix);
    SetUniformVec3Ant(shaderProgram, "customColor", vec3(0.0f, 0.0f, 0.0f));

    glDrawElements(GL_TRIANGLES, vertex, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(texture1Uniform, 6);
    // LEG1 L
    glBindVertexArray(cubeVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
  //      * translate(mat4(1.0f), bodyPosition) 
        * translate(mat4(1.0f), leg1PositionL)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * rotate(mat4(1.0f), radians(45.0f), vec3(1.0f, 0.0f, 0.0f))
       
        * scale(mat4(1.0f), vec3(legSize));
    SetUniformMat4Ant(shaderProgram, "worldMatrix", modelMatrix);
    SetUniformVec3Ant(shaderProgram, "customColor", color);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);

    // LEG1 R
    glBindVertexArray(cubeVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
 //       * translate(mat4(1.0f), bodyPosition)
        * translate(mat4(1.0f), leg1PositionR)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * rotate(mat4(1.0f), radians(-45.0f), vec3(1.0f, 0.0f, 0.0f))
        
        * scale(mat4(1.0f), vec3(legSize));
    SetUniformMat4Ant(shaderProgram, "worldMatrix", modelMatrix);
    SetUniformVec3Ant(shaderProgram, "customColor", color);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);

    
    // LEG2 L
    glBindVertexArray(cubeVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
       // * translate(mat4(1.0f), bodyPosition)
        * translate(mat4(1.0f), leg2PositionL)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * rotate(mat4(1.0f), radians(45.0f), vec3(1.0f, 0.0f, 0.0f))

        * scale(mat4(1.0f), vec3(legSize));
    SetUniformMat4Ant(shaderProgram, "worldMatrix", modelMatrix);
    SetUniformVec3Ant(shaderProgram, "customColor", color);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);

    // LEG2 R
    glBindVertexArray(cubeVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
     //   * translate(mat4(1.0f), bodyPosition)
        * translate(mat4(1.0f), leg2PositionR)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * rotate(mat4(1.0f), radians(-45.0f), vec3(1.0f, 0.0f, 0.0f))

        * scale(mat4(1.0f), vec3(legSize));
    SetUniformMat4Ant(shaderProgram, "worldMatrix", modelMatrix);
    SetUniformVec3Ant(shaderProgram, "customColor", color);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);

    // LEG3 L
    glBindVertexArray(cubeVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
      //  * translate(mat4(1.0f), bodyPosition)
        * translate(mat4(1.0f), leg3PositionL)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * rotate(mat4(1.0f), radians(45.0f), vec3(1.0f, 0.0f, 0.0f))

        * scale(mat4(1.0f), vec3(legSize));
    SetUniformMat4Ant(shaderProgram, "worldMatrix", modelMatrix);
    SetUniformVec3Ant(shaderProgram, "customColor", color);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);

    // LEG3 R
    glBindVertexArray(cubeVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
      //  * translate(mat4(1.0f), bodyPosition)
        * translate(mat4(1.0f), leg3PositionR)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * rotate(mat4(1.0f), radians(-45.0f), vec3(1.0f, 0.0f, 0.0f))

        * scale(mat4(1.0f), vec3(legSize));
    SetUniformMat4Ant(shaderProgram, "worldMatrix", modelMatrix);
    SetUniformVec3Ant(shaderProgram, "customColor", color);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);

    // Antenna L
    glBindVertexArray(cubeVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
        //  * translate(mat4(1.0f), bodyPosition)
        * translate(mat4(1.0f), antennaPositionL)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * rotate(mat4(1.0f), radians(-45.0f), vec3(0.0f, 0.0f, 1.0f))

        * scale(mat4(1.0f), vec3(legSize));
    SetUniformMat4Ant(shaderProgram, "worldMatrix", modelMatrix);
    SetUniformVec3Ant(shaderProgram, "customColor", color);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);

    // Antenna R
    glBindVertexArray(cubeVao);
    modelMatrix = scale(mat4(1.0f), scaleFactor)
        //  * translate(mat4(1.0f), bodyPosition)
        * translate(mat4(1.0f), antennaPositionR)
        * rotate(mat4(1.0f), radians(yaw), vec3(0.0f, 1.0f, 0.0f))
        * rotate(mat4(1.0f), radians(-45.0f), vec3(0.0f, 0.0f, 1.0f))

        * scale(mat4(1.0f), vec3(legSize));
    SetUniformMat4Ant(shaderProgram, "worldMatrix", modelMatrix);
    SetUniformVec3Ant(shaderProgram, "customColor", color);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);

    // Antenna 2 L
    glBindVertexArray(cubeVao);
    modelMatrix =
        scale(mat4(1.0f), scaleFactor)
         * translate(mat4(1.0f), antenna2PositionL) 
        * rotate(mat4(1.0f), radians(antennaAngle), vec3(0.0f, 0.0f, 1.0f))
 
        * scale(mat4(1.0f), vec3(antennaSize));
    SetUniformMat4Ant(shaderProgram, "worldMatrix", modelMatrix);
    SetUniformVec3Ant(shaderProgram, "customColor", color);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);

    // Antenna 2 R
    glBindVertexArray(cubeVao);
    modelMatrix =
        scale(mat4(1.0f), scaleFactor)
        * translate(mat4(1.0f), antenna2PositionR)
        * rotate(mat4(1.0f), radians(antennaAngle), vec3(0.0f, 0.0f, 1.0f))

        * scale(mat4(1.0f), vec3(antennaSize));
    SetUniformMat4Ant(shaderProgram, "worldMatrix", modelMatrix);
    SetUniformVec3Ant(shaderProgram, "customColor", color);

    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);
    
}

void Ant::moveAntennas() {
    const float angleThreshold = 0.1f;
    if (abs(antennaAngle + 90.0f) < angleThreshold) {
     //   antennaUp = true;
        goingUp = false;
        goingDown = true;
    }
    else if (abs(antennaAngle + 135.0f) < angleThreshold) {
      //  antennaUp = false;
        goingDown = false;
        goingUp = true;
    }
    if (goingUp) {
        antennaAngle += 0.1f;
    }
    else if(goingDown) {
        antennaAngle -= 0.1f;
    }
        
}

vec3 Ant::getPosition() {
    return bodyPosition;
}


// Functions

// shader variable setters
void SetUniformMat4Ant(GLuint shader_id, const char* uniform_name, mat4 uniform_value) {
    glUseProgram(shader_id);
    glUniformMatrix4fv(glGetUniformLocation(shader_id, uniform_name), 1, GL_FALSE, &uniform_value[0][0]);

}
void SetUniformVec3Ant(GLuint shader_id, const char* uniform_name, vec3 uniform_value) {
    glUseProgram(shader_id);
    glUniform3fv(glGetUniformLocation(shader_id, uniform_name), 1, value_ptr(uniform_value));

}
template <class T>
void SetUniform1ValueAnt(GLuint shader_id, const char* uniform_name, T uniform_value) {
    glUseProgram(shader_id);
    glUniform1i(glGetUniformLocation(shader_id, uniform_name), uniform_value);
    glUseProgram(0);
}

void createFloorShadowAnt(int shadowShaderProgram, int vao) {
    mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f))
        * scale(mat4(1.0f), vec3(100.0f, 0.5f, 100.0f));

    SetUniformMat4Ant(shadowShaderProgram, "worldMatrix", groundWorldMatrix);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);
}

void createFloorAnt(int shaderProgram, int vao, GLint texture1Uniform) {
    mat4 groundWorldMatrix = translate(mat4(1.0f), vec3(0.0f, 0.0f, 0.0f))
        * scale(mat4(1.0f), vec3(100.0f, 0.5f, 100.0f));
    SetUniformMat4Ant(shaderProgram, "worldMatrix", groundWorldMatrix);
    SetUniformVec3Ant(shaderProgram, "customColor", vec3(1.0f, 1.0f, 1.0f));

    glUniform1i(texture1Uniform, 2); // Texture unit 2 is now bound to texture1

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
    glBindVertexArray(0);
}

float randomInRangeAnt(float lowerBound, float upperBound) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dist(lowerBound, upperBound);
    return dist(gen);
}