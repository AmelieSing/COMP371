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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "gameObject.h"

using namespace glm;
using namespace std;

bool initContext();

int loadSHADER(string vertex_file_path, string fragment_file_path);

GLuint loadTexture(const char* filename);

int createTexturedCubeVertexArrayObject();

int createVertexArrayCube();

void SetUniformMat4(GLuint shader_id, const char* uniform_name, mat4 uniform_value);

GLFWwindow* window = NULL;

int main(int argc, char* argv[])
{
    // Initialize GLFW and OpenGL version
    if (!initContext()) return -1;

    //Loading Shaders
    GLuint colorShader = loadSHADER("./Assets/Shaders/color_vertex.glsl", "./Assets/Shaders/color_fragment.glsl"); //Color shader is the basic shader used in lab03 and lab04
    GLuint textureShader = loadSHADER("./Assets/Shaders/texture_vertex.glsl", "./Assets/Shaders/texture_fragment.glsl"); 
    GLuint lightShader = loadSHADER("./Assets/Shaders/light_vertex.glsl", "./Assets/Shaders/light_fragment.glsl");
    GLuint shaderShadow = loadSHADER("./Assets/Shaders/shadow_vertex.glsl", "./Assets/Shaders/shadow_fragment.glsl");
   
    //Loading Textures
#if defined(PLATFORM_OSX)
    GLuint brickTextureID = loadTexture("Assets/Textures/brick.jpg");
#else
    GLuint brickTextureID = loadTexture("./Assets/Textures/brick.jpg");
#endif

    //Vector Array Objects
    int vaColorCube = createVertexArrayCube();
    int vaTexturedCube = createTexturedCubeVertexArrayObject();

    // Background color
    float red = 56.0f/255.0f;
    float blue =  89.0f/255.0f;
    float green = 76.0f/255.0f;
    glClearColor(red, blue, green, 1.0f);

    // Camera parameters for view transform
    vec3 cameraPosition(0.6f, 1.0f, 10.0f);
    vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
    vec3 cameraUp(0.0f, 1.0f, 0.0f);

    // Other camera parameters
    float cameraSpeed = 1.0f;
    float cameraFastSpeed = 2 * cameraSpeed;
    float cameraHorizontalAngle = 90.0f;
    float cameraVerticalAngle = 0.0f;

    // Set projection matrix for shader
    float fov = 70.0f;
    mat4 projectionMatrix = perspective(fov,            // field of view in degrees
        1024.0f / 768.0f,  // aspect ratio
        0.01f, 100.0f);   // near and far (near > 0)

    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition,  // eye
        cameraPosition + cameraLookAt,  // center
        cameraUp); // up

    // Set View and Projection matrices on all shaders
    // Color Shader
    SetUniformMat4(colorShader, "projectionMatrix", projectionMatrix);
    SetUniformMat4(colorShader, "viewMatrix", viewMatrix);

    SetUniformMat4(textureShader, "projectionMatrix", projectionMatrix);
    SetUniformMat4(textureShader, "viewMatrix", viewMatrix);

    mat4 worldMatrix = mat4(1.0f);

    //Color matrix
    GLuint colorLocation = glGetUniformLocation(colorShader, "objectColor");

    //Setting up world matrix location for geometry
    GLuint colorWorldMatrixLocation = glGetUniformLocation(colorShader, "worldMatrix");
    GLuint textureWorldMatrixLocation = glGetUniformLocation(textureShader, "worldMatrix");

    // For frame time
    float lastFrameTime = glfwGetTime();
    int lastMouseLeftState = GLFW_RELEASE;
    double lastMousePosX, lastMousePosY;
    glfwGetCursorPos(window, &lastMousePosX, &lastMousePosY);

    // Other OpenGL states to set once
    // Enable Backface culling
    glEnable(GL_CULL_FACE);
    // Enable Depth Test
    glEnable(GL_DEPTH_TEST);

    //World rotation angles
    float worldRy = 0;
    float worldRx = 0;

    //Camera rotation angles
    float phi = 0; //vertical
    float theta = radians(90.0f); //horizontal

    //-----MODELING USING gameObject class------

    //Create a parent gameObject instance. 
    //This object contains a refrence to all it's children object. 
    //When transforming parent object position/rotation/scale, their children object will transformed relative to it
    gameObject arm; 
    //set arm position
    arm.setTransformPosition(glm::vec3(0.0f, 1.0f, 0.0f));
    //create a parent gameObject
    //This object (although still a parent object) will be a child of 'arm'
    //This means that when arm moves, elbow will move accoridngly  
    gameObject elbow;
    //Make elbow a child of arm
    arm.addChildObject(&elbow);
    //This will transform the elbows position relative to it's parent object
    //In this case, 'arm' is at (0,1,0)
    //We will set 'elbow' at (0,2,0). Because elbow's position is relative to arm, it's world position is elbow_position + arm_position 
    //elbow world position: (0,2,0) + (0,1,0) = (0,3,0)
    elbow.setTransformPosition(glm::vec3(0.0f, 2.0f, 0.0f));

    //Create a gameObject instance
    //This object will be attached to the arm and be visible
    gameObject upperArm;
    //set which VAO this part of the model will use
    upperArm.setVAO(vaTexturedCube);
    //set the number of vertices that the assigned VAO has
    upperArm.setVertCount(36);
    //set texture for this part of the model
    upperArm.setTexture(brickTextureID);
    //set the scale for 'aesthetic' reasons 
    upperArm.setTransformScale(glm::vec3(0.3f,2.0f,0.3f));
    //Make 'upperArm' a child or 'arm'
    arm.addChildObject(&upperArm);
    //set the postion (relative to 'arm')
    upperArm.setTransformPosition(glm::vec3(0.0f, 1.0f, 0.0f));

    //Create a gameObject instance
    //This object will be attached to the 'elbow' and be visible
    gameObject lowerArm;
    //Make 'lowerArm' a child or 'elbow'
    elbow.addChildObject(&lowerArm);
    //set which VAO this part of the model will use
    lowerArm.setVAO(vaTexturedCube);
    //set the number of vertices that the assigned VAO has
    lowerArm.setVertCount(36);
    //set texture for this part of the model
    lowerArm.setTexture(brickTextureID);
    //set the scale for "aesthetic" reasons 
    lowerArm.setTransformScale(glm::vec3(0.3f, 1.0f, 0.3f));
    //set the postion (relative to 'elbow')
    lowerArm.setTransformPosition(glm::vec3(0.0f, 0.5f, 0.0f));

    //MODEL HIERARCHY
    /*
        arm--|
             |--> upperArm
             |
             |--> elbow--|
                         |--> lowerArm
    */
 

    float angle = 0.0f;
    // Entering Main Loop
    while (!glfwWindowShouldClose(window))
    {



        // Frame time calculation
        float dt = glfwGetTime() - lastFrameTime;
        lastFrameTime += dt;

        //World rotation
        glm::mat4 worldMatrix = rotate(mat4(1.0f), radians(worldRy), glm::vec3(0.0f, 1.0f, 0.0f))
            * rotate(mat4(1.0f), radians(worldRx), glm::vec3(1.0f, 0.0f, 0.0f));

        /*Test Cubes*/

        // Each frame, reset color of each pixel to glClearColor
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //Texture Cube
        //Setting program shader
        glUseProgram(textureShader);

        //Set up texture
        glActiveTexture(GL_TEXTURE0);
        GLuint textureLocation = glGetUniformLocation(textureShader, "textureSampler");
        glBindTexture(GL_TEXTURE_2D, brickTextureID);
        glUniform1i(textureLocation, 0);                // Set our Texture sampler to user Texture Unit 0

        
        //Setting up vaTexturedCube
        glBindVertexArray(vaTexturedCube);
        glBindBuffer(GL_ARRAY_BUFFER, vaTexturedCube);

        //Drawing Lower arm
        mat4 textureCubeMatrix = worldMatrix * translate(mat4(1.0f), vec3(3.0f, 0.0f, 0.0f))
            * scale(mat4(1.0f), vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(textureWorldMatrixLocation, 1, GL_FALSE, &textureCubeMatrix[0][0]);
       // glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(0.9f, 0.8f, 0.6f)));
        SetUniformMat4(textureShader, "worldMatrix", textureCubeMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0
      
        //------DRAWING USING gameObject class------
        //calling the draw function on parent object calls draw on all it's children
        arm.drawModel(GL_TRIANGLES, textureShader, textureWorldMatrixLocation, colorWorldMatrixLocation, textureLocation);
        //add some rotation to the entire arm
        angle += 0.01;
        arm.setTransformRotation(glm::vec3(0.0f,0.0f,80*std::sin(angle)));
        //add some rotation to the elbow
        elbow.setTransformRotation(glm::vec3(0.0f,0.0f, 80 * std::sin(angle)));

        //Color Cube
        //Setting program shader
        glUseProgram(colorShader);

        //Setting up vaColorCube 
        glBindVertexArray(vaColorCube);
        glBindBuffer(GL_ARRAY_BUFFER, vaColorCube);

        //Drawing Color Cube
        mat4 colorCubeMatrix = worldMatrix * translate(mat4(1.0f), vec3(-3.0f, 0.0f, 0.0f)) 
            * scale(mat4(1.0f), vec3(1.0f,1.0f,1.0f));
        glUniformMatrix4fv(colorWorldMatrixLocation, 1, GL_FALSE, &colorCubeMatrix[0][0]);
        glUniform3fv(colorLocation, 1, glm::value_ptr(glm::vec3(1.0f, 1.0f, 0.0f)));
        SetUniformMat4(colorShader, "worldMatrix", colorCubeMatrix);
        glDrawArrays(GL_TRIANGLES, 0, 36); // 36 vertices, starting at index 0

        
        
 
        /*World functionalities*/
        
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

            //Setting the new projection
            SetUniformMat4(colorShader, "projectionMatrix", projectionMatrix);
            SetUniformMat4(textureShader, "projectionMatrix", projectionMatrix);
            SetUniformMat4(lightShader, "projection_matrix", projectionMatrix);
            lastMousePosY = mousePosY;
        }

        viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);
        SetUniformMat4(colorShader, "viewMatrix", viewMatrix);
        SetUniformMat4(textureShader, "viewMatrix", viewMatrix);
        SetUniformMat4(lightShader, "view_matrix", viewMatrix);
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

//Added functions to check the functionalities - Need to be properly stored in another file at some point
void SetUniformMat4(GLuint shader_id, const char* uniform_name, mat4 uniform_value)
{
    glUseProgram(shader_id);
    glUniformMatrix4fv(glGetUniformLocation(shader_id, uniform_name), 1, GL_FALSE, &uniform_value[0][0]);
}

int loadSHADER(string vertex_file_path, string fragment_file_path) {

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    // Read the Vertex Shader code from the file
    std::string VertexShaderCode;
    std::ifstream VertexShaderStream(vertex_file_path, std::ios::in);
    if (VertexShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << VertexShaderStream.rdbuf();
        VertexShaderCode = sstr.str();
        VertexShaderStream.close();
    }
    else {
        printf("Impossible to open %s. Are you in the right directory ? Don't forget to read the FAQ !\n", vertex_file_path);
        getchar();
        return 0;
    }

    // Read the Fragment Shader code from the file
    std::string FragmentShaderCode;
    std::ifstream FragmentShaderStream(fragment_file_path, std::ios::in);
    if (FragmentShaderStream.is_open()) {
        std::stringstream sstr;
        sstr << FragmentShaderStream.rdbuf();
        FragmentShaderCode = sstr.str();
        FragmentShaderStream.close();
    }

    GLint Result = GL_FALSE;
    int InfoLogLength;

    // Compile Vertex Shader
    cout << "Compiling shader : " << vertex_file_path << endl;
    char const* VertexSourcePointer = VertexShaderCode.c_str();
    glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
    glCompileShader(VertexShaderID);

    // Check Vertex Shader
    glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
        printf("%s\n", &VertexShaderErrorMessage[0]);
    }

    // Compile Fragment Shader
    cout << "Compiling shader : " << fragment_file_path << endl;
    char const* FragmentSourcePointer = FragmentShaderCode.c_str();
    glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
    glCompileShader(FragmentShaderID);

    // Check Fragment Shader
    glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
    glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
        glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
        printf("%s\n", &FragmentShaderErrorMessage[0]);
    }

    // Link the program
    printf("Linking program\n");
    GLuint ProgramID = glCreateProgram();
    glAttachShader(ProgramID, VertexShaderID);
    glAttachShader(ProgramID, FragmentShaderID);
    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
    }

    glDetachShader(ProgramID, VertexShaderID);
    glDetachShader(ProgramID, FragmentShaderID);

    glDeleteShader(VertexShaderID);
    glDeleteShader(FragmentShaderID);

    return ProgramID;
}

GLuint loadTexture(const char* filename)
{
    // Step1 Create and bind textures
    GLuint textureId = 0;
    glGenTextures(1, &textureId);
    assert(textureId != 0);


    glBindTexture(GL_TEXTURE_2D, textureId);

    // Step2 Set filter parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Step3 Load Textures with dimension data
    int width, height, nrChannels;
    unsigned char* data = stbi_load(filename, &width, &height, &nrChannels, 0);
    if (!data)
    {
        std::cerr << "Error::Texture could not load texture file:" << filename << std::endl;
        return 0;
    }

    // Step4 Upload the texture to the PU
    GLenum format = 0;
    if (nrChannels == 1)
        format = GL_RED;
    else if (nrChannels == 3)
        format = GL_RGB;
    else if (nrChannels == 4)
        format = GL_RGBA;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height,
        0, format, GL_UNSIGNED_BYTE, data);

    // Step5 Free resources
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureId;
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

