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
#include <math.h>

#include <stdlib.h>

#define GLEW_STATIC 1   // This allows linking with Static Library on Windows, without DLL
#include <GL/glew.h>    // Include GLEW - OpenGL Extension Wrangler

#include <GLFW/glfw3.h> // cross-platform interface for creating a graphical context,
                        // initializing OpenGL and binding inputs

#include <glm/glm.hpp>  // GLM is an optimized math library with syntax to similar to OpenGL Shading Language
#include <glm/gtc/matrix_transform.hpp> // include this to create transformation matrices
#include <glm/gtc/type_ptr.hpp>
#include <glm/common.hpp>

#include<random>       // RANDOM NUMBER GENERATOR - https://cplusplus.com/reference/random/

#include "animalBird.cpp"
#include "characterObject.cpp"
#include "Ant.h"
//#include "functions.cpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "shaderloader.h"

#include "gameObject.h"
#include "animalGenerators.h"
#include "animalFrog.h"
#include "World.h"

#include "OBJloader.h" 
#include "OBJloaderV2.h"

using namespace glm;
using namespace std;

bool initContext();
int createCubeVAO();
int createCubeVAO2();

// int loadSHADER(string vertex_file_path, string fragment_file_path);

GLuint loadTexture(const char* filename);

//int createTexturedCubeVertexArrayObject();
int createVertexArrayCube(); 
void handleInputs();

GLFWwindow* window = NULL;

void generateBird(vec3 cameraPosition, vector<Bird *>& birdList, float cameraHorizontalAngle, int shaderProgram, int shaderShadowProgram, int vao, int sphere2VAO, int sphere2Vertices, GLint texture1Uniform);

const char* vertexShaderSource = R"(
    #version 330 core
    layout(location = 0) in vec2 aPos;
    layout(location = 1) in vec2 aTexCoord;

    out vec2 TexCoord;
    uniform mat4 model;

    void main()
    {
        gl_Position =model * vec4(aPos.x, aPos.y, 0.0, 1.0);
        TexCoord = vec2(aTexCoord.x, 1.0 - aTexCoord.y); // Flip the texture vertically
    }
)";

const char* fragmentShaderSource = R"(
    #version 330 core
    in vec2 TexCoord;
    out vec4 FragColor;

    uniform sampler2D ourTexture;

    void main()
    {
        vec4 texColor = texture(ourTexture, TexCoord);
        
        if (texColor.a < 1.0)
        {
            FragColor = mix(FragColor, vec4(0.0, 0.0, 0.0, 0.0), 1.0 - texColor.a);
        }
        FragColor = texColor; 
    }
)";

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

void createSPhere(vector<glm::vec3>& vertices, vector<glm::vec3>& normals, vector<glm::vec2>& UV, vector<int>& indices, float radius, int slices, int stacks) {
    int k1, k2;
    for (int i = 0; i <= slices; i++) {
        k1 = i * (stacks + 1);
        k2 = k1 + stacks + 1;
        for (int j = 0; j <= stacks; j++, k1++, k2++) {
            glm::vec3 v;
            float theta = 2.0f * 3.14f * j / slices;
            float phi = 3.14f * i / stacks;
            v.x = radius * cos(theta) * sin(phi);
            v.y = radius * sin(theta) * sin(phi);
            v.z = radius * cos(phi);
            vertices.push_back(v);
            glm::vec3 n(v.x / radius, v.y / radius, v.z / radius);
            normals.push_back(n);
            vec2 m;
            m.x = (float)j / (float)slices;
            m.y = (float)i / (float)stacks;
            UV.push_back(m);

            if (i != 0) {
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);
            }

            // k1+1 => k2 => k2+1
            if (i != (slices - 1)) {
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }
    }
}

GLuint setupModelVBO(string path, int& vertexCount) {
    std::vector<glm::vec3> vertices;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> UVs;

    // read the vertex data from the model's OBJ file
    loadOBJ(path.c_str(), vertices, normals, UVs);

    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);  // Becomes active VAO
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and
    // attribute pointer(s).

    // Vertex VBO setup
    GLuint vertices_VBO;
    glGenBuffers(1, &vertices_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
        &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
        (GLvoid*)0);
    glEnableVertexAttribArray(0);

    // Normals VBO setup
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3),
        &normals.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
        (GLvoid*)0);
    glEnableVertexAttribArray(1);

    // UVs VBO setup
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(),
        GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat),
        (GLvoid*)0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent
    // strange bugs, as we are using multiple VAOs)
    vertexCount = vertices.size();
    return VAO;
}

GLuint setupModelEBO(int& vertexCount, vector<glm::vec3> vertices, vector<glm::vec3> normals, vector<glm::vec2> UVs, vector<int> vertexIndices) {
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO); //Becomes active VAO
    // Bind the Vertex Array Object first, then bind and set vertex buffer(s) and attribute pointer(s).

    //Vertex VBO setup
    GLuint vertices_VBO;
    glGenBuffers(1, &vertices_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertices_VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);

    //Normals VBO setup
    GLuint normals_VBO;
    glGenBuffers(1, &normals_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normals_VBO);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(1);
    //UVs VBO setupdra
    GLuint uvs_VBO;
    glGenBuffers(1, &uvs_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, uvs_VBO);
    glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs.front(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(2);

    //EBO setup
    GLuint EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, vertexIndices.size() * sizeof(int), &vertexIndices.front(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    // Unbind VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs), remember: do NOT unbind the EBO, keep it bound to this VAO
    vertexCount = vertexIndices.size();
    return VAO;
}

int main(int argc, char* argv[])
{
    // Initialize GLFW and OpenGL version
    if (!initContext()) return -1;

    srand(time(0));

    //Loading Shaders
    string shaderPathPrefix = "./Assets/mycoal_shaders/";
    int shaderProgram   = loadSHADER(shaderPathPrefix + "texture_vertex.glsl", shaderPathPrefix + "texture_fragment.glsl");
    int shadowShaderProgram     = loadSHADER(shaderPathPrefix + "shadow_vertex.glsl", shaderPathPrefix + "shadow_fragment.glsl");
    int particleShaderProgram = loadSHADER(shaderPathPrefix + "Texture.vertexshader", shaderPathPrefix + "Texture.fragmentshader");

    GLuint brickTextureID = loadTexture("./Assets/Textures/brick.jpg");
    GLuint defaultTextureID = loadTexture("./Assets/Textures/white.png");
    GLuint furTextureID = loadTexture("./Assets/Textures/HairFur.png");
    GLuint grassTextureID = loadTexture("./Assets/Textures/grass.png");
    GLuint scalesTextureID = loadTexture("./Assets/Textures/scales.png");
    GLuint barkTextureID = loadTexture("./Assets/Textures/bark.jpg");
    GLuint smokeTextureID = loadTexture("./Assets/Textures/smoke.png");
    GLuint treeTextureID = loadTexture("./Assets/Textures/tree.png");
    GLuint bushTextureID = loadTexture("./Assets/Textures/busg.png");
    GLuint birdFaceTextureID = loadTexture("./Assets/Textures/bird.jpg");
    GLuint birdFurTextureID = loadTexture("./Assets/Textures/birdFur.png");
    GLuint leavesTextureID = loadTexture("./Assets/Textures/leaves.png");
    GLuint insectTextureID = loadTexture("./Assets/Textures/insect.jpg");
    GLuint frogSkinTextureID = loadTexture("./Assets/Textures/frog.jpg");


    std::vector<GLuint>* generatorTextures = new std::vector<GLuint>;
    generatorTextures->push_back(furTextureID);
    generatorTextures->push_back(grassTextureID);
    generatorTextures->push_back(scalesTextureID);
    generatorTextures->push_back(barkTextureID);

    std::vector<GLuint>* frogTextures = new std::vector<GLuint>;
    frogTextures->push_back(defaultTextureID);
    frogTextures->push_back(frogSkinTextureID);

    //skybox shader
    GLuint skyboxShader = loadSHADER("./Assets/skybox/skyboxvs.glsl",
        "./Assets/skybox/skyboxfs.glsl");

    GLuint titleTextureID = loadTexture("./Assets/Textures/titled.png");


    //string spherePath = "./Assets/Models/sphere.obj";

    //int sphereVertices;
    //GLuint sphereVAO = setupModelVBO(spherePath, sphereVertices);

    

    //change skybox image here
    vector<std::string> faces
    {
        "./Assets/skybox/aright.jpg", //ok
            "./Assets/skybox/afront.jpg",//ok
            "./Assets/skybox/atop.jpg",
            "./Assets/skybox/abottom.jpg",
            "./Assets/skybox/aback.jpg",
            "./Assets/skybox/aleft.jpg"
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
    glm::vec3 cameraPosition(0.6f, 30.0f, 50.0f);
    glm::vec3 cameraLookAt(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp(0.0f, 1.0f, 0.0f);

    // Other camera parameters
    float cameraSpeed = 10.0f;
    float cameraFastSpeed = 2 * cameraSpeed;
    float cameraHorizontalAngle = 90.0f;
    float cameraVerticalAngle = 0.0f;

    bool spacePressed = false;

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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

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
        0.01f, 1000.0f);   // near and far (near > 0)

    // Set initial view matrix
    mat4 viewMatrix = lookAt(cameraPosition,  // eye
        cameraPosition + cameraLookAt,  // center
        cameraUp); // up

    SetUniformMat4(shaderProgram, "projectionMatrix", projectionMatrix);
    SetUniformMat4(shaderProgram, "viewMatrix", viewMatrix);
    mat4 ViewProjectionTransformMatrix = projectionMatrix * viewMatrix;
    SetUniformMat4(particleShaderProgram, "ViewProjectionTransform", ViewProjectionTransformMatrix);


    int vao2 = createCubeVAO();
    int vao = createCubeVAO2();

    GLint shadowMapUniform = glGetUniformLocation(shaderProgram, "shadow_map");
    glUniform1i(shadowMapUniform, 0); // Texture unit 1 is now bound to texture1

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, defaultTextureID);
    GLint texture1Uniform = glGetUniformLocation(shaderProgram, "textureSampler");
    glUniform1i(texture1Uniform, 1); // Texture unit 1 is now bound to texture1

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, birdFurTextureID);
    glUniform1i(texture1Uniform, 2); // Texture unit 2 is now bound to texture1

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, birdFaceTextureID);
    glUniform1i(texture1Uniform, 3); // Texture unit 3 is now bound to texture1

    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glUniform1i(glGetUniformLocation(skyboxShader, "skybox"), 4);

    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, smokeTextureID);
    glUniform1i(texture1Uniform, 5); // Texture unit 5 is now bound to texture1
    
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, leavesTextureID);
    glUniform1i(texture1Uniform, 6); // Texture unit 6 is now bound to texture1

    glActiveTexture(GL_TEXTURE7);
    // glBindTexture(GL_TEXTURE_2D, birdFurTextureID);
    // glUniform1i(texture1Uniform, 7); // Texture unit 7 is now bound to texture1


    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, insectTextureID);
    glUniform1i(texture1Uniform, 6);

    std::cout << cubemapTexture;


    vector<int> vertexIndices;
    //The contiguous sets of three indices of vertices, normals and UVs, used to make a triangle
    vector<glm::vec3> vertices;
    vector<glm::vec3> normals;
    vector<glm::vec2> UVs;
    createSPhere(vertices, normals, UVs, vertexIndices, 3.0f, 40, 40);
    int sphere2Vertices;
    GLuint sphere2VAO = setupModelEBO(sphere2Vertices, vertices, normals, UVs, vertexIndices);

    vector<Bird *> birdList;

    Bird* bird1 = new Bird(shaderProgram, shadowShaderProgram, vao, sphere2VAO, sphere2Vertices, texture1Uniform, vec3(-5.0f, 5.0f, 15.0f), 1, 0, 0.05f, 1.0f);
    Bird* bird2 = new Bird(shaderProgram, shadowShaderProgram, vao, sphere2VAO, sphere2Vertices, texture1Uniform, vec3(-15.0f, 15.0f, -3.0f), 1, 0, 0.05f, 1.0f);
    bird1->setShapeSphere();
    // birdList.push_back(bird1);
    // birdList.push_back(bird2);
    characterObject cameraMan(shaderProgram, shadowShaderProgram, vao, texture1Uniform, cameraPosition);
    characterObject NPC1(shaderProgram, shadowShaderProgram, vao, texture1Uniform, 10.0f, 0.0f, -5.0f);
    characterObject NPC2(shaderProgram, shadowShaderProgram, vao, texture1Uniform, 5.0f, 4.0f, 3.0f);

    Ant ant1(shaderProgram, shadowShaderProgram, sphere2VAO, vao, texture1Uniform, vec3(20.0f, 1.25f, 2.0f), vertexIndices.size(), insectTextureID, defaultTextureID);
    
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
    float wokidooAnimalRotate = 0.0f;
    generateAnimal(sphere2VAO, vertexIndices.size(), generatorTextures,wokidooAnimal, neck_joint, leftHip_joint, rightHip_joint, leftShoulder_joint, rightShoulder_joint);
    /*
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
    */
    gameObject wokidooAnimalPivot;
    wokidooAnimalPivot.addChildObject(&wokidooAnimal);
    wokidooAnimal.setTransformPosition(0.0f, 4.0f, 8.0f);
    wokidooAnimal.setTransformRotation(0.0f, -90.0f, 0.0f);

    //Frog parameters
    gameObject frogAnimal;
    gameObject frogLeftHip_joint; 
    gameObject frogRightHip_joint;
    gameObject frogLeftKnee_joint;
    gameObject frogRightKnee_joint;
    gameObject frogLeftAnkle_joint;
    gameObject frogRightAnkle_joint;
    gameObject frogLeftShoulder_joint;
    gameObject frogRightShoulder_joint;
    gameObject frogLeftElbow_joint;
    gameObject frogRightElbow_joint;
    gameObject frogLeftWrist_joint;
    gameObject frogRightWrist_joint;
    gameObject frogLeftCheek_joint;
    gameObject frogRightCheek_joint;

    float frogAnimalRotate = 0.0f;
    generateFrog(sphere2VAO, vertexIndices.size(), frogTextures, frogAnimal, 
        frogLeftHip_joint, frogRightHip_joint, frogLeftKnee_joint, frogRightKnee_joint, frogLeftAnkle_joint, frogRightAnkle_joint,
        frogLeftShoulder_joint, frogRightShoulder_joint, frogLeftElbow_joint, frogRightElbow_joint, frogLeftWrist_joint, frogRightWrist_joint,
        frogLeftCheek_joint, frogRightCheek_joint);

    gameObject frogPivot;
    frogPivot.addChildObject(&frogAnimal);
    frogAnimal.setTransformPosition(-12.0f, 3.5f, 20.0f);
    frogAnimal.setTransformRotation(0.0f, 45.0f, 0.0f);
    frogAnimal.setTransformScale(0.6f,0.6f,0.6f);

    float frogCheekMax = 0.7;
    float frogCheekMin = 0.1;
    float frogCheekScale = frogCheekMin;
    bool frogCheekIncrease = true;

    
    // Entering Main Loop

    //plane
    float planevertices[] = {
        // Positions       // Texture Coords
        -0.5f, -0.5f,     0.0f, 0.0f,
         0.5f, -0.5f,     1.0f, 0.0f,
         0.5f,  0.5f,     1.0f, 1.0f,
        -0.5f,  0.5f,     0.0f, 1.0f,
    };
    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };
    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planevertices), planevertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Specify vertex attributes
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);


    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    int width, height, nrChannels;
    unsigned char* data = stbi_load("./Assets/Textures/brick.jpg", &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format = nrChannels == 3 ? GL_RGB : GL_RGBA;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture!" << std::endl;
    }
    stbi_image_free(data);

    GLuint vertexShader, fragmentShader, planeshaderProgram;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);

    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);

    planeshaderProgram = glCreateProgram();
    glAttachShader(planeshaderProgram, vertexShader);
    glAttachShader(planeshaderProgram, fragmentShader);
    glLinkProgram(planeshaderProgram);

    bool showTitle = true;
    World world;
    GLuint hRelease = GLFW_RELEASE;   
     
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

        int n = birdList.size();
        // ------------------------- SHADOW PASS -------------------------------
        {
            glUseProgram(shadowShaderProgram);
            glViewport(0, 0, DEPTH_MAP_TEXTURE_SIZE, DEPTH_MAP_TEXTURE_SIZE);
            // Bind depth map texture as output framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, depth_map_fbo);
            // Clear depth data on the framebuffer
            glClear(GL_DEPTH_BUFFER_BIT);

            createFloorShadow(shadowShaderProgram, vao2);
            for(int i = 0; i < n; i++) {
                birdList[i]->drawShadow();
                birdList[i]->moveWings();
                birdList[i]->move();
            }
            cameraMan.drawShadow();
            NPC1.drawShadow();
            NPC2.drawShadow();

            bird1->drawShadow();
            bird2->drawShadow();
            wokidooAnimalPivot.drawModelShadows(GL_TRIANGLES, shadowShaderProgram, glGetUniformLocation(shadowShaderProgram, "worldMatrix"));
            frogPivot.drawModelShadows(GL_TRIANGLES, shadowShaderProgram, glGetUniformLocation(shadowShaderProgram, "worldMatrix"));

            ant1.drawShadow();
        }


        // ----------------------------------------------------------------------------
        // ---------------------- RENDER SCENE AFTER SHADOW ---------------------------
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
            createFloor(shaderProgram, vao2, texture1Uniform);
        
        // ------------------- 
            
            for(int i = 0; i < n; i++) {
                birdList[i]->draw();
                birdList[i]->moveWings();
                birdList[i]->move();
            }
            bird1->draw();
            bird1->move();
            bird1->moveWings();
            bird2->draw();
            bird2->moveWings();
            cameraMan.draw();
            cameraMan.setBodyAngle(cameraHorizontalAngle);
            cameraMan.moveAnimation();
            NPC1.draw();
            NPC1.moveAnimation();
            NPC1.move();
            NPC2.draw();
            NPC2.move();
            NPC2.rotateSelf();
            // cameraMan.updatePos(dt);
            wokidooAnimalPivot.drawModel(GL_TRIANGLES, shaderProgram, glGetUniformLocation(shaderProgram, "worldMatrix"), glGetUniformLocation(shaderProgram, "customColor"), glGetUniformLocation(shaderProgram, "textureSampler"));
            frogPivot.drawModel(GL_TRIANGLES, shaderProgram, glGetUniformLocation(shaderProgram, "worldMatrix"), glGetUniformLocation(shaderProgram, "customColor"), glGetUniformLocation(shaderProgram, "textureSampler"));

            ant1.draw();
            ant1.moveAntennas();

        }

        //BALL SAMPLE DISPLAY
        glBindVertexArray(sphere2VAO);
        glUseProgram(shaderProgram);
        /*glDrawArrays(GL_TRIANGLES, 0, sphere2Vertices);*/
        //glDrawElements(GL_TRIANGLES, sphere2Vertices, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        

        //skybox
        glDepthFunc(GL_LEQUAL);
        glUseProgram(skyboxShader);
        SetUniformMat4(skyboxShader, "projection", projectionMatrix);
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE4);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        glDepthFunc(GL_LESS);

        //plane sample display
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(planeshaderProgram);
        glBindVertexArray(VAO);
        glActiveTexture(GL_TEXTURE5);
        glBindTexture(GL_TEXTURE_2D, titleTextureID);
        GLuint textureLocation = glGetUniformLocation(planeshaderProgram, "ourTexture");
        glUniform1i(textureLocation, 5); // Use texture unit 5
        glm::mat4 model = glm::mat4(1.0f);
        model = translate(mat4(1.0f), glm::vec3(0.5f, 0.75f, 0.0f));
        GLint modelLoc = glGetUniformLocation(planeshaderProgram, "model");
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        if(showTitle)
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


        //trees!!
        glActiveTexture(GL_TEXTURE5); //
        glBindTexture(GL_TEXTURE_2D, treeTextureID);
        glUniform1i(textureLocation, 5); 
        model = projectionMatrix*viewMatrix*translate(mat4(1.0f), glm::vec3(-15.5f, 10.0f, -17.0f))*scale(mat4(1),vec3(20,20,20));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        model = projectionMatrix * viewMatrix * translate(mat4(1.0f), glm::vec3(17.5f, 10.0f, -40.0f)) * scale(mat4(1), vec3(20, 30, 20));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glActiveTexture(GL_TEXTURE5); //
        glBindTexture(GL_TEXTURE_2D, bushTextureID);
        glUniform1i(textureLocation, 5);
        model = projectionMatrix * viewMatrix * translate(mat4(1.0f), glm::vec3(-20.5f, 5.0f, -10.0f)) * scale(mat4(1), vec3(30, 10, 10));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        
        glBindVertexArray(0);
        glUseProgram(0);
        


        //World rotation
        // glm::mat4 worldMatrix = rotate(mat4(1.0f), radians(worldRy), glm::vec3(0.0f, 1.0f, 0.0f))
            // * rotate(mat4(1.0f), radians(worldRx), glm::vec3(1.0f, 0.0f, 0.0f));

        wokidooAnimalRotate += 0.01;
        
        wokidooAnimalPivot.setTransformRotation(0.0f,-100*wokidooAnimalRotate,0.0f);
        //wokidooAnimal.setTransformRotation(0, 80 * std::sin(wokidooAnimalRotate), 0);
        leftHip_joint.setTransformRotation(glm::vec3(45 * std::sin(wokidooAnimalRotate * 10), 0.0f, 0.0f));
        rightHip_joint.setTransformRotation(glm::vec3(-45 * std::sin(wokidooAnimalRotate * 10), 0.0f, 0.0f));
        leftShoulder_joint.setTransformRotation(glm::vec3(0.0f, 0.0f, -30 * std::sin(wokidooAnimalRotate * 15)));
        rightShoulder_joint.setTransformRotation(glm::vec3(0.0f, 0.0f, 30 * std::sin(wokidooAnimalRotate * 15)));
        neck_joint.setTransformPosition(glm::vec3(neck_joint.getTransformPosition()[0], neck_joint.getTransformPosition()[1], neck_joint.getTransformPosition()[2] + (std::sin(wokidooAnimalRotate * 25) / 12)));
        
        world.Update(dt);
        world.Draw(smokeTextureID, particleShaderProgram);
        glBindTexture(GL_TEXTURE_2D, defaultTextureID);


        //Frog animations


        frogLeftCheek_joint.setTransformScale(frogCheekScale, frogCheekScale, frogCheekScale);
        frogRightCheek_joint.setTransformScale(frogCheekScale, frogCheekScale, frogCheekScale);

        if (frogCheekIncrease == true && frogCheekScale <= 0.7) {
            frogCheekScale += 0.01;
	        }
	    else if (frogCheekIncrease == false && frogCheekScale >= 0.1) {
            frogCheekScale -= 0.01;
	    }
	    else if (frogCheekScale > 0.7 && frogCheekIncrease == true) {
		    frogCheekIncrease = false;
	    }
	    else if (frogCheekScale < 0.1 && frogCheekIncrease == false) {
            frogCheekIncrease = true;
	    }

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
        if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) // generate animal
        {
            generateAnimal(sphere2VAO, vertexIndices.size(), generatorTextures, wokidooAnimal, neck_joint, leftHip_joint, rightHip_joint, leftShoulder_joint, rightShoulder_joint);
        }
        if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS && hRelease == GLFW_RELEASE) //toggle title 
        {
            if (showTitle)
                showTitle = false;
            else
                showTitle = true;
        }
        glBindVertexArray(0);
        hRelease = glfwGetKey(window, GLFW_KEY_H);

        // End Frame
        glfwSwapBuffers(window);
        glfwPollEvents();

        // ------------------------------------------------------------------------------------
        // ------------------------------ Handle inputs ---------------------------------------
        // ------------------------------------------------------------------------------------
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
           glfwSetWindowShouldClose(window, true);
        }
        // ------------------------------------------------------------------------------------
        
        // -------------------------- MOUSE MOVEMENTS ------------------------------------------
        double mousePosX, mousePosY;
        glfwGetCursorPos(window, &mousePosX, &mousePosY);

        double dx = mousePosX - lastMousePosX;
        double dy = mousePosY - lastMousePosY;

        lastMousePosX = mousePosX;
        lastMousePosY = mousePosY;

        // Convert to spherical coordinates
        const float cameraAngularSpeed = 20.0f;
        cameraHorizontalAngle -= dx * cameraAngularSpeed * dt;
        cameraVerticalAngle -= dy * cameraAngularSpeed * dt;

        // Clamp vertical angle to [-85, 85] degrees
        cameraVerticalAngle = std::max(-65.0f, std::min(85.0f, cameraVerticalAngle));

        float theta = radians(cameraHorizontalAngle);
        float phi = radians(cameraVerticalAngle);

        cameraLookAt = vec3(cosf(phi) * cosf(theta), sinf(phi), -cosf(phi) * sinf(theta));
        vec3 frontVector = vec3(cosf(theta), 0.0f, -1.0f * sinf(theta));
        vec3 cameraSideVector = glm::cross(cameraLookAt, vec3(0.0f, 1.0f, 0.0f));
        vec3 cameraSideVector2 = glm::cross(frontVector, vec3(0.0f, 1.0f, 0.0f));

        glm::normalize(cameraSideVector);
        // ------------------------------------------------------------------------------------
        //Camera Speed
        bool fastCam = glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        float currentCameraSpeed = (fastCam) ? cameraFastSpeed : cameraSpeed;
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
            ViewProjectionTransformMatrix = projectionMatrix * viewMatrix;
            SetUniformMat4(particleShaderProgram, "ViewProjectionTransform", ViewProjectionTransformMatrix);

            
            lastMousePosY = mousePosY;
        }
        // -------------------------- CAMERA MOVEMENTS ------------------------------------------
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {  // move camera to the left
            cameraMan.controlMove(-1.0f * cameraSideVector2 * currentCameraSpeed, dt);
        }
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) { // move camera to the right
            cameraMan.controlMove(1.0f * cameraSideVector2 * currentCameraSpeed, dt);
        }
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {  // move camera backward
            cameraMan.controlMove(-1.0f * frontVector * currentCameraSpeed, dt);
        }
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {    // move camera forward
            cameraMan.controlMove(1.0f * frontVector * currentCameraSpeed, dt);
        }

        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) { // move camera up
            if (!spacePressed && cameraMan.getOnGround()) {
                cameraMan.charJump();
                spacePressed = true;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE) { // move camera up
            spacePressed = false;
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) { // move camera down
        }

        cameraMan.updatePos(dt);

        cameraPosition = cameraMan.getHeadPosition();
        generateBird(cameraPosition, birdList, cameraHorizontalAngle, shaderProgram, shadowShaderProgram, vao, sphere2VAO, sphere2Vertices, texture1Uniform);

        viewMatrix = lookAt(cameraPosition, cameraPosition + cameraLookAt, cameraUp);

        SetUniformMat4(skyboxShader, "view", glm::mat4(glm::mat3(viewMatrix)));
        SetUniformMat4(shaderProgram, "viewMatrix", viewMatrix);
        ViewProjectionTransformMatrix = projectionMatrix * viewMatrix;
        SetUniformMat4(particleShaderProgram, "ViewProjectionTransform", ViewProjectionTransformMatrix);


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
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 15.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(15.0f, 15.0f)),

    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(15.0f, 15.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(15.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),  vec3(0.0f, 0.0f, -1.0f), vec2(15.0f, 15.0f)), // far - blue
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 15.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),  vec3(0.0f, 0.0f, -1.0f), vec2(15.0f, 15.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f),  vec3(0.0f, 0.0f, -1.0f), vec2(15.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(15.0f, 15.0f)), // bottom - turquoise
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f),vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(15.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),  vec3(0.0f, -1.0f, 0.0f), vec2(15.0f, 15.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 15.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 15.0f)), // near - green
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),  vec3(0.0f, 0.0f, 1.0f), vec2(15.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),  vec3(0.0f, 0.0f, 1.0f), vec2(15.0f, 15.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 15.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),  vec3(0.0f, 0.0f, 1.0f), vec2(15.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(15.0f, 15.0f)), // right - purple
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(15.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(15.0f, 15.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 15.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),  vec3(0.0f, 1.0f, 0.0f), vec2(15.0f, 15.0f)), // top - yellow
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),  vec3(0.0f, 1.0f, 0.0f), vec2(15.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),  vec3(0.0f, 1.0f, 0.0f), vec2(15.0f, 15.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 15.0f))
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
int createCubeVAO2() {
    // Cube model
    const TexturedColoredVertex vertexArray[] = {  // position,                            normals
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)), //left - red
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),

    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(-1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),  vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f)), // far - blue
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 1.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),  vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f),  vec3(0.0f, 0.0f, -1.0f), vec2(1.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, 0.0f, -1.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 1.0f)), // bottom - turquoise
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f),vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),  vec3(0.0f, -1.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f,-0.5f,-0.5f), vec3(0.0f, -1.0f, 0.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)), // near - green
    TexturedColoredVertex(vec3(-0.5f,-0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),  vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),  vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 0.0f, 1.0f), vec2(0.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f),  vec3(0.0f, 0.0f, 1.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)), // right - purple
    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f,-0.5f,-0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(0.5f,-0.5f, 0.5f), vec3(1.0f, 0.0f, 0.0f), vec2(0.0f, 1.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),  vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)), // top - yellow
    TexturedColoredVertex(vec3(0.5f, 0.5f,-0.5f),  vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),

    TexturedColoredVertex(vec3(0.5f, 0.5f, 0.5f),  vec3(0.0f, 1.0f, 0.0f), vec2(1.0f, 1.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f,-0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 0.0f)),
    TexturedColoredVertex(vec3(-0.5f, 0.5f, 0.5f), vec3(0.0f, 1.0f, 0.0f), vec2(0.0f, 1.0f))
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



void generateBird(vec3 cameraPosition, vector<Bird *>& birdList, float cameraHorizontalAngle, int shaderProgram, int shaderShadowProgram, int vao, int sphere2VAO, int sphere2Vertices, GLint texture1Uniform) {
    int n = birdList.size();
    int numBirdInRange = 0;
    if(n > 25) {
        return;
    }
    for(int i = 0; i < n; i++) {
        vec3 distance = abs(cameraPosition - birdList[i]->getPosition());
        float length = glm::length(distance);  // distance.x
        if(length < 250.0f) {
            numBirdInRange++;
        }
    }

    if(numBirdInRange < 4) {
        int diff = 5 - numBirdInRange;
        for(int i = 0; i < diff; i++) {
            float yPos = randomInRange(50.0f, 70.0f);
            float rPos = randomInRange(100.0f, 200.0f);

            float angle = randomInRange(cameraHorizontalAngle + 90, cameraHorizontalAngle + 150);
            float yaw = randomInRange(0, 360);
            float size = randomInRange(0.5f, 4.0f);
            float circleDistance = randomInRange(0.0f, 10.0f);
            float circleSpeed = randomInRange(0.0f, 0.75f);
            // std::cout << "angle - "<< angle << " cameraAngle " << cameraHorizontalAngle << std::endl;
            
            vec3 newPos(rPos * cos(radians(angle)) + cameraPosition.x, yPos, rPos * sin(radians(angle)) + cameraPosition.z);
            // std::cout << "cam - "<< cameraPosition.x << " x - z " << cameraPosition.z << std::endl;
            // std::cout << newPos.x << " x - z " << newPos.z << std::endl;

            Bird* pointer = new Bird(shaderProgram, shaderShadowProgram, vao, sphere2VAO, sphere2Vertices, texture1Uniform, newPos, size, yaw, circleDistance, circleSpeed);
            if(i % 2 == 0) {
                pointer->setShapeSphere();
            }
            birdList.push_back(pointer);
            
        }
    }

}
