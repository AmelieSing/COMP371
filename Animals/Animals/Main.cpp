//
// COMP 371 Summer 2023 - Final Project
//
// Created by Pipelines Pirates.
//

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <streambuf>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

#include "animalBird.cpp"
// #include "functions.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shaderloader.h"

#include "gameObject.h"

using namespace glm;
using namespace std;

bool initContext();
int createCubeVAO();
// int loadSHADER(string vertex_file_path, string fragment_file_path);

GLuint loadTexture(const char* filename);

int createTexturedCubeVertexArrayObject();

int createVertexArrayCube();

GLFWwindow* window = NULL;


unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}


int main(int argc, char* argv[])
{
    // Initialize GLFW and OpenGL version
    if (!initContext()) return -1;

    //Loading Shaders
    string shaderPathPrefix = "./Assets/mycoal_shaders/";
    int shaderProgram   = loadSHADER(shaderPathPrefix + "texture_vertex.glsl", shaderPathPrefix + "texture_fragment.glsl");
    int shadowShaderProgram     = loadSHADER(shaderPathPrefix + "shadow_vertex.glsl", shaderPathPrefix + "shadow_fragment.glsl");

    GLuint brickTextureID = loadTexture("./Assets/Textures/brick.jpg");
    GLuint defaultTextureID = loadTexture("./Assets/Textures/white.png");

    //skybox shader
    GLuint skyboxShader = loadSHADER("./Assets/skybox/skyboxvs.glsl",
        "./Assets/skybox/skyboxfs.glsl");

    //change skybox image here
    vector<std::string> faces
    {
        "./Assets/skybox/aback.jpg",
            "./Assets/skybox/abottom.jpg",
            "./Assets/skybox/atop.jpg",
            "./Assets/skybox/aleft.jpg",
            "./Assets/skybox/aright.jpg",
            "./Assets/skybox/afront.jpg"
    };

    GLuint cubemapTexture = loadCubemap(faces);

    float skyboxVertices[] = {
    // positions
    -50.0f,  50.0f, -50.0f,
    -50.0f, -50.0f, -50.0f,
     50.0f, -50.0f, -50.0f,
     50.0f, -50.0f, -50.0f,
     50.0f,  50.0f, -50.0f,
    -50.0f,  50.0f, -50.0f,

    -50.0f, -50.0f,  50.0f,
    -50.0f, -50.0f, -50.0f,
    -50.0f,  50.0f, -50.0f,
    -50.0f,  50.0f, -50.0f,
    -50.0f,  50.0f,  50.0f,
    -50.0f, -50.0f,  50.0f,

     50.0f, -50.0f, -50.0f,
     50.0f, -50.0f,  50.0f,
     50.0f,  50.0f,  50.0f,
     50.0f,  50.0f,  50.0f,
     50.0f,  50.0f, -50.0f,
     50.0f, -50.0f, -50.0f,

    -50.0f, -50.0f,  50.0f,
    -50.0f,  50.0f,  50.0f,
     50.0f,  50.0f,  50.0f,
     50.0f,  50.0f,  50.0f,
     50.0f, -50.0f,  50.0f,
    -50.0f, -50.0f,  50.0f,

    -50.0f,  50.0f, -50.0f,
     50.0f,  50.0f, -50.0f,
     50.0f,  50.0f,  50.0f,
     50.0f,  50.0f,  50.0f,
    -50.0f,  50.0f,  50.0f,
    -50.0f,  50.0f, -50.0f,

    -50.0f, -50.0f, -50.0f,
    -50.0f, -50.0f,  50.0f,
     50.0f, -50.0f, -50.0f,
     50.0f, -50.0f, -50.0f,
    -50.0f, -50.0f,  50.0f,
     50.0f, -50.0f,  50.0f
};

    // skybox VAO
    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    
    
    //Vector Array Objects
    // int vaColorCube = createVertexArrayCube();
    // int vaTexturedCube = createTexturedCubeVertexArrayObject();

    // Background color
    float red = 56.0f/255.0f;
    float blue =  89.0f/255.0f;
    float green = 76.0f/255.0f;
    glClearColor(red, blue, green, 1.0f);

    // Camera parameters for view transform
    vec3 cameraPosition(0.6f, 10.0f, 50.0f);
    vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
    vec3 cameraUp(0.0f, 1.0f, 0.0f);

    // Other camera parameters
    float cameraSpeed = 1.0f;
    float cameraFastSpeed = 2 * cameraSpeed;
    float cameraHorizontalAngle = 90.0f;
    float cameraVerticalAngle = 0.0f;

    const unsigned int DEPTH_MAP_TEXTURE_SIZE = 1024;
        
    // Variable storing index to texture used for shadow mapping
    GLuint depth_map_texture;
    // Get the texture
    glGenTextures(1, &depth_map_texture);
    // Bind the texture so the next glTex calls affect it
    glBindTexture(GL_TEXTURE_2D, depth_map_texture);
    // Create the texture and specify it's attributes, including widthn height, components (only depth is stored, no color information)
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT,
                NULL);
    // Set texture sampler parameters.
    // The two calls below tell the texture sampler inside the shader how to upsample and downsample the texture. Here we choose the nearest filtering option, which means we just use the value of the closest pixel to the chosen image coordinate.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // The two calls below tell the texture sampler inside the shader how it should deal with texture coordinates outside of the [0, 1] range. Here we decide to just tile the image.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Variable storing index to framebuffer used for shadow mapping
    GLuint depth_map_fbo;  // fbo: framebuffer object
    // Get the framebuffer
    glGenFramebuffers(1, &depth_map_fbo);
    // Bind the framebuffer so the next glFramebuffer calls affect it
    glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
    // Attach the depth map texture to the depth map framebuffer
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_texture, 0);
	//glDrawBuffer(GL_NONE); //disable rendering colors, only write depth values

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Set projection matrix for shader
    float fov = 70.0f;
    mat4 projectionMatrix = perspective(fov,            // field of view in degrees
        1024.0f / 768.0f,  // aspect ratio
        0.01f, 100.0f);   // near and far (near > 0)

    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition,  // eye
        cameraPosition + cameraLookAt,  // center
        cameraUp); // up

    SetUniformMat4(shaderProgram, "projectionMatrix", projectionMatrix);
    SetUniformMat4(shaderProgram, "viewMatrix", viewMatrix);

    // mat4 worldMatrix = mat4(1.0f);
    int vao = createCubeVAO();
    GLint shadowMapUniform = glGetUniformLocation(shaderProgram, "shadow_map");
    glUniform1i(shadowMapUniform, 0); // Texture unit 1 is now bound to texture1

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, defaultTextureID);
    GLint texture1Uniform = glGetUniformLocation(shaderProgram, "textureSampler");
    glUniform1i(texture1Uniform, 1); // Texture unit 1 is now bound to texture1

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, brickTextureID);
    glUniform1i(texture1Uniform, 2); // Texture unit 2 is now bound to texture1

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glUniform1i(glGetUniformLocation(skyboxShader, "skybox"), 4);

    std::cout << cubemapTexture;


    // glActiveTexture(GL_TEXTURE3);
    // glBindTexture(GL_TEXTURE_2D, tennistTextureID);
    // glUniform1i(texture1Uniform, 3); // Texture unit 3 is now bound to texture1

    // glActiveTexture(GL_TEXTURE4);
    // glBindTexture(GL_TEXTURE_2D, steelTextureID);
    // glUniform1i(texture1Uniform, 4); // Texture unit 4 is now bound to texture1

    Bird bird1(shaderProgram, shadowShaderProgram, vao, texture1Uniform, vec3(5.0f, 5.0f, 0.0f));

    // For frame time
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    //World rotation angles
    float worldRy = 0;
    float worldRx = 0;

    //Camera rotation angles
    float phi = 0; //vertical
    float theta = radians(90.0f); //horizontal

    //Creating parent & child 'joint' objects
    gameObject wokidooAnimal;
    gameObject leftHip_joint;
    gameObject rightHip_joint;
    gameObject neck_joint;
    gameObject leftShoulder_joint;
    gameObject rightShoulder_joint;
    wokidooAnimal.setTransformPosition(0.0f, 6.0f, 0.0f);
    float wokidooAnimalRotate = 0.0f;
    {
        wokidooAnimal.addChildObject(&leftHip_joint);
        wokidooAnimal.addChildObject(&rightHip_joint);
        wokidooAnimal.addChildObject(&neck_joint);
        wokidooAnimal.addChildObject(&leftShoulder_joint);
        wokidooAnimal.addChildObject(&rightShoulder_joint);
        leftShoulder_joint.setTransformPosition(glm::vec3(2.0f,1.0f,1.5f));
        rightShoulder_joint.setTransformPosition(glm::vec3(-2.0f, 1.0f, 1.5f));
        leftHip_joint.setTransformPosition(glm::vec3(1.5f, -2.0f, 1.0f));
        rightHip_joint.setTransformPosition(glm::vec3(-1.5f, -2.0f, 1.0f));
        neck_joint.setTransformPosition(glm::vec3(0.0f,1.0f,3.0f));


        //MAIN BODY
        gameObject mainBody;
        wokidooAnimal.addChildObject(&mainBody);
        mainBody.setVAO(vao);
        mainBody.setTexture(defaultTextureID);
        mainBody.setVertCount(36);
        mainBody.setTransformScale(glm::vec3(4.0f,4.0f,5.0f));
        mainBody.setTransformPosition(0.0f, 0.0f, 1.0f);
        mainBody.setColourVector(glm::vec3(1.0f, 0.0f, 0.0f));
        
        gameObject tail;
        wokidooAnimal.addChildObject(&tail);
        tail.setVAO(vao);
        tail.setTexture(defaultTextureID);
        tail.setVertCount(36);
        tail.setTransformScale(0.6f, 2.0f, 4.0f);
        tail.setTransformPosition(0.0f, 1.0f, -2.2f);
        tail.setTransformRotation(45.0f, 0.0f, 0.0f);

        gameObject leftWing;
        leftShoulder_joint.addChildObject(&leftWing);
        leftWing.setVAO(vao);
        leftWing.setTexture(defaultTextureID);
        leftWing.setVertCount(36);
        leftWing.setTransformScale(glm::vec3(4.0f,1.0f,2.0f));
        leftWing.setTransformPosition(glm::vec3(2.0f, 0.0f, 0.0f));

        gameObject rightWing;
        rightShoulder_joint.addChildObject(&rightWing);
        rightWing.setVAO(vao);
        rightWing.setTexture(defaultTextureID);
        rightWing.setVertCount(36);
        rightWing.setTransformScale(glm::vec3(4.0f, 1.0f, 2.0f));
        rightWing.setTransformPosition(glm::vec3(-2.0f, 0.0f, 0.0f));

        gameObject leftLeg;
        leftHip_joint.addChildObject(&leftLeg);
        leftLeg.setVAO(vao);
        leftLeg.setTexture(defaultTextureID);
        leftLeg.setVertCount(36);
        leftLeg.setTransformScale(glm::vec3(1.0f, 3.0f, 1.0f));
        leftLeg.setTransformPosition(glm::vec3(0.0f, -1.0f, 0.0f));

        gameObject rightLeg;
        rightHip_joint.addChildObject(&rightLeg);
        rightLeg.setVAO(vao);
        rightLeg.setTexture(defaultTextureID);
        rightLeg.setVertCount(36);
        rightLeg.setTransformScale(glm::vec3(1.0f, 3.0f, 1.0f));
        rightLeg.setTransformPosition(glm::vec3(0.0f, -1.0f, 0.0f));

        gameObject head;
        neck_joint.addChildObject(&head);
        head.setVAO(vao);
        head.setTexture(defaultTextureID);
        head.setVertCount(36);
        head.setTransformScale(glm::vec3(3.0f, 3.0f, 3.0f));
        head.setTransformPosition(glm::vec3(0.0f, 1.5f, 0.0f));

    }
    gameObject wokidooAnimalPivot;
    wokidooAnimalPivot.addChildObject(&wokidooAnimal);
    wokidooAnimal.setTransformPosition(0.0f, 4.0f, 8.0f);
    wokidooAnimal.setTransformRotation(0.0f, -90.0f, 0.0f);
    // Entering Main Loop
    while (!glfwWindowShouldClose(window))
    {
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;


        // -------------- LIGHTING -----------------------------------
        SetUniformVec3(shaderProgram, "light_color", vec3(1.0, 1.0, 1.0));
        vec3 lightPosition = vec3(0.0f,60.0f, 0.0f); // the location of the light in 3D space
        // vec3(30.0f * sinf(glfwGetTime()), 30.0f, 30.0f * cosf(glfwGetTime()));

        vec3 lightFocus = vec3(0.0, 0.0, 1.0);      // the point in 3D space the light "looks" at
        vec3 lightDirection = normalize(lightFocus - lightPosition);

        float lightNearPlane = 1.0f;
        float lightFarPlane = 180.0f;


        float lightAngleOuter = 30.0;
        float lightAngleInner = 20.0;
        // Set light cutoff angles on scene shader
        SetUniform1Value(shaderProgram, "light_cutoff_inner", cos(radians(lightAngleInner)));
        SetUniform1Value(shaderProgram, "light_cutoff_outer", cos(radians(lightAngleOuter)));
        
        
        mat4 lightProjectionMatrix = frustum(-1.0f, 1.0f, -1.0f, 1.0f, lightNearPlane, lightFarPlane);
        //perspective(20.0f, (float)DEPTH_MAP_TEXTURE_SIZE / (float)DEPTH_MAP_TEXTURE_SIZE, lightNearPlane, lightFarPlane);
        mat4 lightViewMatrix = lookAt(lightPosition, lightFocus, vec3(0.0f, 1.0f, 0.0f));
        mat4 lightSpaceMatrix = lightProjectionMatrix * lightViewMatrix;
        SetUniformMat4(shadowShaderProgram, "light_view_proj_matrix", lightSpaceMatrix);
        SetUniformMat4(shaderProgram, "light_view_proj_matrix", lightSpaceMatrix);

        SetUniform1Value(shaderProgram, "light_near_plane", lightNearPlane);
        SetUniform1Value(shaderProgram, "light_far_plane", lightFarPlane);
        
        SetUniformVec3(shaderProgram, "light_position", lightPosition);
        SetUniformVec3(shaderProgram, "light_direction", lightDirection);

        float tempColor[3] = {0.5f, 0.5f, 0.5f};    // Change Color to Grey
        GLuint texColorLocation = glGetUniformLocation(shaderProgram, "customColor");



        // ------------------------- SHADOW PASS -------------------------------
        {
            glUseProgram(shadowShaderProgram);
            glViewport(0, 0, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE);
            // Bind depth map texture as output framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
            // Clear depth data on the framebuffer
            glClear(GL_DEPTH_BUFFER_BIT);

            createFloorShadow(shadowShaderProgram, vao);

            bird1.drawShadow();
            wokidooAnimalPivot.drawModelShadows(GL_TRIANGLES, shadowShaderProgram, glGetUniformLocation(shadowShaderProgram, "worldMatrix"));
        }



        // ----------------------------------------------------------------------------
        // ---------------------- RENDER SCENE AFTER    SHADOW ---------------------------
        // ----------------------------------------------------------------------------
        {   
            glUseProgram(shaderProgram);
            glUniform1i(shadowMapUniform, 0); 

            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            glViewport(0, 0, width, height);
            // Bind screen as output framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            // Clear color and depth data on framebuffer
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // -------------------------- CLAY GROUND ------------------------------
            createFloor(shaderProgram, vao, texture1Uniform);
        
        // ------------------- 
            bird1.draw();
            bird1.move();
            wokidooAnimalPivot.drawModel(GL_TRIANGLES, shaderProgram, glGetUniformLocation(shaderProgram, "worldMatrix"), glGetUniformLocation(shaderProgram, "objectColor"), glGetUniformLocation(shaderProgram, "textureSampler"));
           

        }


        //skybox
        glDepthFunc(GL_LEQUAL);
        glUseProgram(skyboxShader);
        SetUniformMat4(skyboxShader, "view", lookAt(cameraPosition, cameraLookAt, cameraUp));
        SetUniformMat4(skyboxShader, "projection", projectionMatrix);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        glDepthFunc(GL_LESS);
        


        //World rotation
        // glm::mat4 worldMatrix = rotate(mat4(1.0f), radians(worldRy), glm::vec3(0.0f, 1.0f, 0.0f))
            // * rotate(mat4(1.0f), radians(worldRx), glm::vec3(1.0f, 0.0f, 0.0f));

        wokidooAnimalRotate += 0.01;
        
        wokidooAnimalPivot.setTransformRotation(0.0f,-100*wokidooAnimalRotate,0.0f);
        leftHip_joint.setTransformRotation(glm::vec3(45 * std::sin(wokidooAnimalRotate * 10), 0.0f, 0.0f));
        rightHip_joint.setTransformRotation(glm::vec3(-45 * std::sin(wokidooAnimalRotate * 10), 0.0f, 0.0f));
        leftShoulder_joint.setTransformRotation(glm::vec3(0.0f, 0.0f, -30 * std::sin(wokidooAnimalRotate * 15)));
        rightShoulder_joint.setTransformRotation(glm::vec3(0.0f, 0.0f, 30 * std::sin(wokidooAnimalRotate * 15)));
        neck_joint.setTransformPosition(glm::vec3(neck_joint.getTransformPosition()[0], neck_joint.getTransformPosition()[1], neck_joint.getTransformPosition()[2] + (std::sin(wokidooAnimalRotate * 25) / 12)));
        
        //Rotate the world
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) // rotate left
        {
            worldRy -= 5.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) //  rotate right
        {
            worldRy += 5.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) //  rotate down
        {
            worldRx -= 5.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) //  rotate up
        {
            worldRx += 5.0f;
        }

        if (glfwGetKey(window, GLFW_KEY_HOME) == GLFW_PRESS) //  remove rotation
        {
            worldRy = worldRx = 0.0f;
        }
        glBindVertexArray(0);

        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();

        // Handle inputs
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        //Camera Speed
        bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;

        //Mouse movements
        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        double dx = mousePosX - lastMousePosX;
        double dy = mousePosY - lastMousePosY;

        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;

        // Convert to spherical coordinates
        const float cameraAngularSpeed = 60.0f;


        //X tilt of camera
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
        {
            cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;

            if (cameraHorizontalAngle > 360)
            {
                cameraHorizontalAngle -= 360;
            }
            else if (cameraHorizontalAngle < -360)
            {
                cameraHorizontalAngle += 360;
            }

            theta = radians(cameraHorizontalAngle);


        }
        //Y tilt of camera
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS)
        {

            cameraVerticalAngle -= dy * cameraAngularSpeed * dt;

            // Clamp vertical angle to [-85, 85] degrees
            cameraVerticalAngle = std::max(-85.0f, std::min(85.0f, cameraVerticalAngle));

            phi = radians(cameraVerticalAngle);

        }

        cameraLookAt = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
        vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));

        glm::normalize(cameraSideVector);

        //Zoom in and out
        float zoomSpeed = 0.01f;
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        {
            //Follow mouse movement in y
            if (dy < 0) {
                fov -= zoomSpeed;
            }
            else if (dy > 0) {
                fov += zoomSpeed;
            }

            //Limition on field of view (1 degree - 90 degree)
            if (fov < 1.0f) {
                fov = 1.0f;
            }
            if (fov > 90.0) {
                fov = 90.0f;
            }

            projectionMatrix = perspective(fov, // field of view in degrees
                1024.0f / 768.0f,  // aspect ratio
                0.01f, 100.0f);   // near and far (near > 0)

            SetUniformMat4(shaderProgram, "projectionMatrix", projectionMatrix);
            
            lastMousePosY = mousePosY;
        }

        viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
      
        SetUniformMat4(shaderProgram, "viewMatrix", viewMatrix);

    }

    // Shutdown GLFW
    glfwTerminate();

    return 0;
}


bool initContext() {     // Initialize GLFW and OpenGL version
    glfwInit();

#if defined(PLATFORM_OSX)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#else
    // On windows, we set OpenGL version to 4.1, to support more hardware
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
#endif

    // Create Window and rendering context using GLFW, resolution is 1024X768
    window = glfwCreateWindow(1024, 768, "Comp371 - Summer 2023 Final Project", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLEW
    glewExperimental = true; // Needed for core profile
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to create GLEW" << std::endl;
        glfwTerminate();
        return false;
    }
    return true;
}

//To test that the file works - Need to be properly stored in another file at some point if they are to be re-used instead of 3D files
// Cube model
vec3 vertexArrayCube[] = {  // position,                            color
    vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), //left - red
    vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
    vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),

    vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),
    vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f),
    vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f),

    vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), // far - blue
    vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
    vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),

    vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
    vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),
    vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f),

    vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), // bottom - turquoise
    vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),
    vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),

    vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
    vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f),
    vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f),

    vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), // near - green
    vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
    vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),

    vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
    vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),
    vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f),

    vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), // right - purple
    vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
    vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),

    vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f),
    vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),
    vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f),

    vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), // top - yellow
    vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),
    vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),

    vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f),
    vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f),
    vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f)
};

int createVertexArrayCube() {
    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArrayCube), vertexArrayCube, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
        3,                   // size
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        2 * sizeof(glm::vec3), // stride - each vertex contain 2 vec3 (position, color)
        (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
        3,
        GL_FLOAT,
        GL_FALSE,
        2 * sizeof(glm::vec3),
        (void*)sizeof(glm::vec3)      // color is offseted a vec3 (comes after position)
    );
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return vertexArrayObject;
}

// Textured Cube model
struct TexturedColoredVertex
{
    TexturedColoredVertex(vec3 _position, vec3 _color, vec2 _uv)
        : position(_position), color(_color), uv(_uv) {}

    vec3 position;
    vec3 color;
    vec2 uv;
};

const TexturedColoredVertex texturedCubeVertexArray[] = {  // position,                            color
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)), //left - red
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),

    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f)), // far - blue
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f)), // bottom - turquoise
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 1.0f, 1.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f)), // near - green
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f)), // right - purple
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)), // top - yellow
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(1.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f))
};


int createTexturedCubeVertexArrayObject()
{
    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);

    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(texturedCubeVertexArray), texturedCubeVertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
        3,                   // size
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        sizeof(TexturedColoredVertex), // stride - each vertex contain 2 vec3 (position, color)
        (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aColor in Vertex Shader
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TexturedColoredVertex),
        (void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
    );
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,                            // attribute 2 matches aUV in Vertex Shader
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TexturedColoredVertex),
        (void*)(2 * sizeof(vec3))      // uv is offseted by 2 vec3 (comes after position and color)
    );
    glEnableVertexAttribArray(2);

    return vertexArrayObject;
}

int createCubeVAO() {
    // Cube model
    const TexturedColoredVertex vertexArray[] = {  // position,                            normals
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)), //left - red
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 5.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(5.0f, 1.0f)),

    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(5.0f, 5.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(5.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),  vec3(0.0f, 0.0f, -1.0f), vec2(5.0f, 5.0f)), // far - blue
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 5.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),  vec3(0.0f, 0.0f, -1.0f), vec2(5.0f, 5.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f),  vec3(0.0f, 0.0f, -1.0f), vec2(5.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(5.0f, 5.0f)), // bottom - turquoise
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f),vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(5.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),  vec3(0.0f, -1.0f, 0.0f), vec2(5.0f, 5.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 5.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 5.0f)), // near - green
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),  vec3(0.0f, 0.0f, 1.0f), vec2(5.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),  vec3(0.0f, 0.0f, 1.0f), vec2(5.0f, 5.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 5.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),  vec3(0.0f, 0.0f, 1.0f), vec2(5.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(5.0f, 5.0f)), // right - purple
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(5.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(5.0f, 5.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 5.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),  vec3(0.0f, 1.0f, 0.0f), vec2(5.0f, 5.0f)), // top - yellow
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),  vec3(0.0f, 1.0f, 0.0f), vec2(5.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),  vec3(0.0f, 1.0f, 0.0f), vec2(5.0f, 5.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 5.0f))
    };
        
    // Create a vertex array
    GLuint vertexArrayObject;
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
     
    // Upload Vertex Buffer to the GPU, keep a reference to it (vertexBufferObject)
    GLuint vertexBufferObject;
    glGenBuffers(1, &vertexBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexArray), vertexArray, GL_STATIC_DRAW);

    glVertexAttribPointer(0,                   // attribute 0 matches aPos in Vertex Shader
        3,                   // size
        GL_FLOAT,            // type
        GL_FALSE,            // normalized?
        sizeof(TexturedColoredVertex), // stride - each vertex contain 2 vec3 (position, color)
        (void*)0             // array buffer offset
    );
    glEnableVertexAttribArray(0);


    glVertexAttribPointer(1,                            // attribute 1 matches aNormals in Vertex Shader
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TexturedColoredVertex),
        (void*)sizeof(vec3)      // color is offseted a vec3 (comes after position)
    );
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,                            // attribute 2 matches aUV in Vertex Shader
        2,
        GL_FLOAT,
        GL_FALSE,
        sizeof(TexturedColoredVertex),
        (void*)(2 * sizeof(vec3))      // uv is offseted by 2 vec3 (comes after position and color)
    );
    glEnableVertexAttribArray(2);

    
    return vertexBufferObject;
}