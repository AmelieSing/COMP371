#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include "Quad.h"
//#include "Renderer.h"
//#include "shaderloader.h"

#include <iostream>

using namespace std;
using namespace glm;


    
    Quad::Quad(int vao, int vertexCount, int shaderProgram) : Model(vao, vertexCount, shaderProgram), age(0), opacity(1) {}

    // TODO 2 - Add an expiration condition
    bool Quad::expired() const { 
        if (age >= LIFETIME)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    void Quad::Update(float dt) {
        Model::Update(dt);
        age += dt;

        ////TODO 2 - Interpolation of scaling and opacity
        float interpolationParameter = age / LIFETIME;
        //
        float scalingParam = linear(interpolationParameter);
        float opacityParam = linear(interpolationParameter);

        opacity = OPACITY_I * (1 - opacityParam) + OPACITY_F * opacityParam;
        SetScaling(SCALE_I * (1 - scalingParam) + SCALE_F * scalingParam);
    }

    void Quad::Draw(int textureID, int shaderProgram) {
        // TODO 3.1 - Texture and opacity
        glActiveTexture(GL_TEXTURE1);
        GLuint textureLocation =
           glGetUniformLocation(shaderProgram, "textureSampler");
        glBindTexture(GL_TEXTURE_2D, textureID);
      glUniform1i(textureLocation, 1);
        GLuint opacityLocation =
            glGetUniformLocation(shaderProgram, "opacity");
        glUniform1f(opacityLocation, opacity);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_DEPTH_TEST);
        Model::Draw(textureID, shaderProgram);
        glDisable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
    }

    // Static stuff
    GLuint Quad::quadVAO = 0;

    GLuint Quad::makeQuadVAO(int& vertexCount) {
        if (quadVAO != 0) {
            return quadVAO;
        }

        vector<vec3> positions{vec3(-0.5, 0.5, 0), vec3(-0.5, -0.5, 0),
            vec3(0.5, 0.5, 0), vec3(0.5, 0.5, 0),
            vec3(-0.5, -0.5, 0), vec3(0.5, -0.5, 0)};

        vector<vec3> normals{6, vec3(0, 0, 1)};
        vector<vec3> colors{6, vec3(1, 0, 1)};

        vector<vec2> uvs{vec2(0, 1), vec2(0, 0), vec2(1, 1),
            vec2(1, 1), vec2(0, 0), vec2(1, 0)};

        return Model::createVAO(vertexCount, positions, normals, colors, uvs);
    }

    GLuint Quad::particleTexture = 0;
    int Quad::textureWidth = 0;

   

    float Quad::linear(float t) { return t; }
    float Quad::quadratic(float t) { return t * t; }
    float Quad::root(float t) { return sqrt(t); }
    float Quad::smooth(float t) {
        float theta = pi<float>() * (1 + t);
        return (glm::cos(theta) + 1) / 2;
    }
   
