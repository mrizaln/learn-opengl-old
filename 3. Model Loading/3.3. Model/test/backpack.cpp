#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <model_header/model.h>
#include <shader_header/shader.h>
#include <camera_header/camera.h>
#include <light_header/light.h>
#include <shapes/sphere/sphere.h>


//=======================================================================================


void framebuffer_size_callback(GLFWwindow*, int, int);
void cursor_position_callback(GLFWwindow*, double, double);
void scroll_callback(GLFWwindow*, double, double);
void key_callback(GLFWwindow*, int, int, int, int);

void processInput(GLFWwindow*);
void updateDeltaTime();

void orbit(glm::vec3&, const glm::vec3, const glm::vec3, float);

//=======================================================================================


namespace configuration
{
    constexpr int screenWidth{ 800 };
    constexpr int screenHeight{ 600 };
    float aspectRatio{ static_cast<float>(screenWidth)/screenHeight };
}

namespace timing
{
    float lastFrame{};
    float deltaTime{};
}

namespace mouse
{
    float lastX{};
    float lastY{};
    bool firstMouse { true };
    bool captureMouse{ false };
}

namespace orbitParam
{
    bool doOrbit{ false };
    float orbitSpeed{ 1.0f };
    glm::vec3 orbitAxis { 0.0f, 1.0f, 0.0f };

    bool lockViewToOrigin{ false };
}

Camera camera{};

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

    // set callbacks
    //--------------
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))        // bool == 0 if success
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // disable cursor
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    mouse::captureMouse = true;

    // enable depth testing
    glEnable(GL_DEPTH_TEST);

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    // backpack model
    //---------------
    Model model{ "../../../resources/model/backpack/backpack.obj" };
    Shader modelShader{ "./shader.vs", "./shader.fs" };
    glm::vec3 modelPos{ 0.0f, 0.0f, 0.0f };
    glm::vec3 modelScale{ 1.0f, 1.0f, 1.0f };
    //---------------

    // light source
    //-------------
    Sphere light{ 0.2f, 16, 32 };       // light model
    PointLight lightSource{
        // { 5.0f,  0.0f,  2.0f },         // pos
        camera.position,
        { 0.1f,  0.1f,  0.1f },         // amb
        { 0.7f,  0.7f,  0.7f },         // diff
        { 1.0f,  1.0f,  1.0f },         // spec
        1.0f,
        0.09f,
        0.032f
    };
    Shader lightShader{ "./light-source-shader.vs", "./light-source-shader.fs" };
    // glm::vec3& lightPos{ lightSource.position };
    glm::vec3& lightPos{ camera.position };
    glm::vec3 lightScale{ 1.0f, 1.0f, 1.0f };
    //-------------

    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // render
        //-------
        glClearColor(0.1f, 0.1f, 0.11f, 1.0f);

        // clear color buffer and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // view is handled by camera class
        auto viewMatrix { camera.getViewMatrix() };

        // projection matrix changes because of the aspect ratio, so we'll update it
        auto projectionMatrix { glm::perspective(glm::radians(camera.fov), configuration::aspectRatio, 0.001f, 100.0f) };

        // backpack model
        {
            modelShader.use();

            // view and projection matrix
            modelShader.setMat4("view", viewMatrix);
            modelShader.setMat4("projection", projectionMatrix);

            // model matrix
            glm::mat4 modelMatrix{ 1.0f };
            modelMatrix = glm::translate( modelMatrix , modelPos);
            modelMatrix = glm::scale(modelMatrix, modelScale);
            modelShader.setMat4("model", modelMatrix);

            // uniforms
            // modelShader.setVec3("pointLights[0].position",   lightSource.position);
            modelShader.setVec3("pointLights[0].position",   lightPos);
            modelShader.setVec3("pointLights[0].ambient",    lightSource.ambient);
            modelShader.setVec3("pointLights[0].diffuse",    lightSource.diffuse);
            modelShader.setVec3("pointLights[0].specular",   lightSource.specular);
            modelShader.setFloat("pointLights[0].constant",  lightSource.constant);
            modelShader.setFloat("pointLights[0].linear",    lightSource.linear);
            modelShader.setFloat("pointLights[0].quadratic", lightSource.quadratic);

            model.draw(modelShader);

        }

        // light
        {
            lightShader.use();

            // view and projection matrix
            lightShader.setMat4("view", viewMatrix);
            lightShader.setMat4("projection", projectionMatrix);

            // model matrix
            glm::mat4 modelMatrix{ 1.0f };
            modelMatrix = glm::translate( modelMatrix , lightPos);
            modelMatrix = glm::scale(modelMatrix, lightScale);
            lightShader.setMat4("model", modelMatrix );

            // uniform
            lightShader.setVec3("color",    lightSource.specular);

            // draw
            // light.draw();
        }
        //-------------
        
        if (orbitParam::doOrbit)
        {
            orbit(lightPos, {0.0f, 1.0f, 0.0f}, modelPos, 2.0f);
            if (orbitParam::lockViewToOrigin)
                camera.lookAtOrigin();
        }
    

        glfwSwapBuffers(window);
        glfwPollEvents();
        updateDeltaTime();
    }

    // clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

//=======================================================================================


// window resize callback
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    configuration::aspectRatio = width / static_cast<float>(height);
    // std::cout << "aspect ratio: " << configuration::aspectRatio << '\n';
}

// cursor position callback
void cursor_position_callback(GLFWwindow* window, double xPos, double yPos)
{
    if (!mouse::captureMouse)
        return;

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

// scroll callback
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    camera.processMouseScroll(static_cast<float>(yOffset));
}

// key press callback (for 1 press)
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // close window
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // toggle capture mouse
    if (key == GLFW_KEY_C && action == GLFW_PRESS)
    {
        // toggle
        mouse::captureMouse = !mouse::captureMouse;

        if (mouse::captureMouse)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            mouse::firstMouse = true;
        }
    }

    // set camera target to (0,0,0)
    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS)
    {
        // std::cout << "Before: " << camera.front.x << ' ' << camera.front.y << ' ' << camera.front.z << " | " << camera.yaw << ' ' << camera.pitch << '\n';
        camera.lookAtOrigin();      // look at (0,0,0)
        mouse::firstMouse = true;
        // std::cout << "After: " << camera.front.x << ' ' << camera.front.y << ' ' << camera.front.z << " | " << camera.yaw << ' ' << camera.pitch << "\n\n";
    }

    // toggle orbit
    if (key == GLFW_KEY_O && action == GLFW_PRESS)
    {
        orbitParam::doOrbit = !orbitParam::doOrbit;
    }

    // lock camera to origin
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        orbitParam::lockViewToOrigin = !orbitParam::lockViewToOrigin;
    }
}

// for continuous input
void processInput(GLFWwindow* window)
{
    // camera movement
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.moveCamera(CameraMovement::FORWARD, timing::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.moveCamera(CameraMovement::BACKWARD, timing::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.moveCamera(CameraMovement::RIGHT, timing::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.moveCamera(CameraMovement::LEFT, timing::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        camera.speed *= 1.01;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        camera.speed /= 1.01;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.moveCamera(CameraMovement::UPWARD, timing::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.moveCamera(CameraMovement::DOWNWARD, timing::deltaTime);
}

// record frame draw time
void updateDeltaTime()
{
    float currentFrame{ static_cast<float>(glfwGetTime()) };
    timing::deltaTime = currentFrame - timing::lastFrame;
    timing::lastFrame = currentFrame;
}

void orbit(glm::vec3& orbitObjectPosition, const glm::vec3 orbitAxis, const glm::vec3 orbitCenter, float orbitSpeed = 1.0f)
{
    static int timing{ 0 };

    float radius{ glm::length(orbitObjectPosition - orbitCenter) };

    // orbit coordinate
    glm::vec4 orbitPosition(1.0f);
    // orbitPosition.x = radius * sin(static_cast<float>(glfwGetTime()) * orbitSpeed);
    orbitPosition.x = radius * sin((timing * M_PIf32)/300.0f * orbitSpeed);
    // orbitPosition.z = radius * cos(static_cast<float>(glfwGetTime()) * orbitSpeed);
    orbitPosition.z = radius * cos((timing * M_PIf32)/300.0f * orbitSpeed);
    orbitPosition.y = 0.0f;
    ++timing;

    const auto orbitUp{ glm::vec3(0.0f, 1.0f, 0.0f) };

    // angle between orbit axis and worldUp (in radians)
    float axisAngle{ acos(glm::dot(orbitAxis, orbitUp)) };

    // transformation axis
    auto transformAxis{ glm::normalize(glm::cross(orbitAxis, orbitUp)) };       // this won't work if orbitAxis == orbitUp

    glm::mat4 fromOrbitToWorldCoordinate(1.0f);
    fromOrbitToWorldCoordinate = glm::translate(fromOrbitToWorldCoordinate, orbitCenter);
    
    // don't do rotation if orbitAxis == orbitUp
    if (orbitAxis != orbitUp)
        fromOrbitToWorldCoordinate = glm::rotate(fromOrbitToWorldCoordinate, axisAngle, transformAxis);

    orbitPosition = fromOrbitToWorldCoordinate * orbitPosition;
    orbitObjectPosition = glm::vec3(orbitPosition.x, orbitPosition.y ,orbitPosition.z);
}