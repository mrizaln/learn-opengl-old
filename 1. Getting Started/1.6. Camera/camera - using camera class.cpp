// glad
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// stb_image
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// shader
#include <shader_header/shader.h>

// camera
#include <camera_header/camera.h>

// STL
#include <iostream>

//===========================================================================================================


void framebuffer_size_callback(GLFWwindow*, int, int);
void cursor_position_callback(GLFWwindow*, double, double);
void scroll_callback(GLFWwindow*, double, double);
void processInput(GLFWwindow*, Shader&);
void updateDeltaTime();
void changeAlpha(const Shader&, const std::string&);

//===========================================================================================================


namespace configuration
{
    constexpr int screenWidth { 800 };
    constexpr int screenHeight { 600 };
    float aspectRatio { static_cast<float>(screenWidth)/screenHeight };
}

namespace timing
{
    float lastFrame {};
    float deltaTime {};
}

namespace mouse
{
    float lastX {};
    float lastY {};
    bool firstMouse { true };
}

// create camera object
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

//===========================================================================================================


int main()
{
    // initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // window creation
    GLFWwindow* window { glfwCreateWindow(configuration::screenWidth, configuration::screenHeight, "LearnOpenGL", NULL, NULL) };
    if (!window)
    {
        std::cerr << "Failed to create GLFW window";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // set framebuffer callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // set glfw to capture cursor and set the callback
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    // set scroll callback
    glfwSetScrollCallback(window, scroll_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))        // bool == 0 if success
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // build and compile shader
    Shader theShader("shader.vs", "shader.fs");


    // vertex data
    float vertices[] {
        // positions                // texture coords
        -0.5f, -0.5f, -0.5f,        0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,        1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,        1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,        1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,        0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,        0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,        0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,        1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,        1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,        1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,        0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,        0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,        1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,        1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,        0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,        0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,        0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,        1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,        1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,        1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,        0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,        0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,        0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,        1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,        0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,        1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,        1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,        1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,        0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,        0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,        0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,        1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,        1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,        1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,        0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,        0.0f, 1.0f
    };

    glm::vec3 cubePositions[] {
        glm::vec3( 0.0f,  0.0f,  0.0f), 
        glm::vec3( 2.0f,  5.0f, -15.0f), 
        glm::vec3(-1.5f, -2.2f, -2.5f),  
        glm::vec3(-3.8f, -2.0f, -12.3f),  
        glm::vec3( 2.4f, -0.4f, -3.5f),  
        glm::vec3(-1.7f,  3.0f, -7.5f),  
        glm::vec3( 1.3f, -2.0f, -2.5f),  
        glm::vec3( 1.5f,  2.0f, -2.5f), 
        glm::vec3( 1.5f,  0.2f, -1.5f), 
        glm::vec3(-1.3f,  1.0f, -1.5f)  
    };

    // setting up buffer(s)
    unsigned int VAO;
    unsigned int VBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);


/*===========================================================================================================
                                    ============[ Texture ]============    
-----------------------------------------------------------------------------------------------------------*/
    
    // generate texture (ID)
    unsigned int textureID[2];
    glGenTextures(2, textureID);

    // texture 0
    //----------

    // bind texture
    glBindTexture(GL_TEXTURE_2D, textureID[0]);

    // set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image (texture 0)
    int imageWidth, imageHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true);     // fix flipped image when loaded
    unsigned char* imageData { stbi_load("img/container.jpg", &imageWidth, &imageHeight, &nrChannels, 0) };
    if (imageData)
    {
        // now generate texture from image
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        // fail
        std::cerr << "Failed to load texture 0\n" ;
    }
    stbi_image_free(imageData);

    // texture 1
    //----------
    
    // bind texture
    glBindTexture(GL_TEXTURE_2D, textureID[1]);

    // set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image (texture 0)
    imageData = stbi_load("img/nakiri.jpg", &imageWidth, &imageHeight, &nrChannels, 0);
    if (imageData)
    {
        // now generate texture from image
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        // fail
        std::cerr << "Failed to load texture 1\n" ;
    }
    stbi_image_free(imageData);


    // tell opengl for each sampler to which texture unit it belongs to
    theShader.use();
    theShader.setInt("texture0", 0);
    theShader.setInt("texture1", 1);
    theShader.setFloat("alpha", 0.2f);     // third uniform

    // enable depth testing
    glEnable(GL_DEPTH_TEST);


    //===========================================================================================================

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window, theShader);

        // render
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

        // clear color buffer and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // bind texture to corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureID[1]);

        glBindVertexArray(VAO);

        // ------------------[ transformations ]------------------
        // camera is handled in namespace camera
        auto view { camera.getViewMatrix() };
        theShader.setMat4("view", view);
        
        // projection matrix changes a lot because of the aspect ratio, so we'll update it
        auto projection { glm::perspective(glm::radians(camera.fov), configuration::aspectRatio, 0.1f, 100.0f) };
        theShader.setMat4("projection", projection);
        
        // model matrix is applied 10 times to create 10 cubes
        for (int i { 1 }; const auto& vec : cubePositions)
        {
            auto model { glm::translate(glm::mat4(1.0f), vec) };
            float angle { glm::radians(i*20.0f) };
            // glm::vec3 axis { glm::vec3( sin(glfwGetTime()+1.0f), sin(glfwGetTime()+0.3f), sin(glfwGetTime()+0.5f) ) };
            glm::vec3 axis { glm::vec3(1.0f, 0.3f, 0.5f) };
            model = glm::rotate(model, angle, axis);
            theShader.setMat4("model", model);
            ++i;
            glDrawArrays(GL_TRIANGLES, 0, std::size(vertices));
        }
        //-------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
        updateDeltaTime();
    }

    // de-allocate all resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

//===========================================================================================================


void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    configuration::aspectRatio = width / static_cast<float>(height);
    // std::cout << "aspect ratio: " << configuration::aspectRatio << '\n';
}

void cursor_position_callback(GLFWwindow* window, double xPos, double yPos)
{
    if (mouse::firstMouse)
    {
        mouse::lastX = xPos;
        mouse::lastY = yPos;
        mouse::firstMouse = false;
    }

    float xOffset { static_cast<float>(xPos) - mouse::lastX };
    float yOffset { mouse::lastY - static_cast<float>(yPos) };

    camera.processMouseMovement(xOffset, yOffset);

    mouse::lastX = xPos;
    mouse::lastY = yPos;
}

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    camera.processMouseScroll(static_cast<float>(yOffset));
}

void updateDeltaTime()
{
    float currentFrame { static_cast<float>(glfwGetTime()) };
    timing::deltaTime = currentFrame - timing::lastFrame;
    timing::lastFrame = currentFrame;
}

void processInput(GLFWwindow* window, Shader& shader)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

        // update cameraSpeed by multipying it with configuration::deltaTime
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.moveCamera(CameraMovement::FORWARD, timing::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.moveCamera(CameraMovement::BACKWARD, timing::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.moveCamera(CameraMovement::RIGHT, timing::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.moveCamera(CameraMovement::LEFT, timing::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.moveCamera(CameraMovement::UPWARD, timing::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.moveCamera(CameraMovement::DOWNWARD, timing::deltaTime);
    
    // alpha
    if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS)
        changeAlpha(shader, "inc");
    if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS)
        changeAlpha(shader, "dec");
}

// uniform alpha increment handler (increment: up/down)
void changeAlpha(const Shader& shader, const std::string& increment)
{
    float alphaValue {};
    glGetUniformfv(shader.ID, glGetUniformLocation(shader.ID, "alpha"), &alphaValue);

    if (increment == "inc")  alphaValue += 0.01f;
    if (increment == "dec") alphaValue -= 0.01f;

    if (alphaValue > 1.0) alphaValue = 1;
    if (alphaValue < 0.0) alphaValue = 0;

    // std::cout << alphaValue << '\n';

    // glUniform1f(glGetUniformLocation(shader.ID, "alpha"), alphaValue);
    shader.setFloat("alpha", alphaValue);
}