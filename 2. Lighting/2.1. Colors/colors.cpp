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

namespace lighting
{
    glm::vec3 lightPos{ 1.2f, 1.0f, 2.0f };
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

    // enable depth testing
    glEnable(GL_DEPTH_TEST);

    // build and compile shader
    //-------------------------
    // container
    Shader lightingShader("shader.vs", "shader.fs");

    // light source
    Shader lightSourceShader("light-source-shader.vs", "light-source-shader.fs");

    
    // vertex data
    float vertices[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };

    glm::vec3 cubePosition { glm::vec3( 0.0f,  0.0f,  0.0f) };

    // setting up buffer for container object
    //---------------------------------------
    unsigned int cubeVAO{};
    unsigned int cubeVBO{};

    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
    glEnableVertexAttribArray(0);


    // setting up buffer for light object
    //-----------------------------------
    unsigned int lightVAO{};
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);

    // we only need to bind to the VBO, the container's VBO's data already contains the data
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
    glEnableVertexAttribArray(0);


    //===========================================================================================================

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window, lightingShader);

        // render
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

        // clear color buffer and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // ------------------[ transformations ]------------------
        // container
        //----------

        // use shader
        lightingShader.use();

        // set color
        glm::vec3 objectColor{ 1.0f, 0.5f, 0.31f };
        lightingShader.setVec3("objectColor", objectColor);
        
        glm::vec3 lightColor { 1.0f, 1.0f, 1.0f };
        lightingShader.setVec3("lightColor", lightColor);

        // view is handled by camera class
        auto view { camera.getViewMatrix() };
        lightingShader.setMat4("view", view);
        
        // projection matrix changes a lot because of the aspect ratio, so we'll update it
        auto projection { glm::perspective(glm::radians(camera.fov), configuration::aspectRatio, 0.1f, 100.0f) };
        lightingShader.setMat4("projection", projection);
        
        // model matrix
        auto model_container{ glm::translate(glm::mat4(1.0f), cubePosition)};
        lightingShader.setMat4("model", model_container);

        // draw
        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, std::size(vertices));


        // light source (cube)
        //--------------------

        // use shader
        lightSourceShader.use();

        // view and projection is the same
        lightSourceShader.setMat4("view", view);
        lightSourceShader.setMat4("projection", projection);

        // model is different
        auto model_light{ glm::translate(glm::mat4(1.0f), lighting::lightPos) };
        model_light = glm::scale(model_light, glm::vec3(0.2f));
        lightSourceShader.setMat4("model", model_light);
        
        // draw
        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, std::size(vertices));
        //-------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
        updateDeltaTime();
    }

    // de-allocate all resources
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &cubeVBO);

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