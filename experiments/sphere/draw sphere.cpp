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

// sphere
#include <shapes/sphere/sphere.h>

// STL
#include <iostream>

//===========================================================================================================


void framebuffer_size_callback(GLFWwindow*, int, int);
void cursor_position_callback(GLFWwindow*, double, double);
void scroll_callback(GLFWwindow*, double, double);
void key_callback(GLFWwindow*, int, int, int, int);

void processInput(GLFWwindow*, Shader&);
void updateDeltaTime();
void changeAlpha(const Shader&, const std::string&);

void orbit(glm::vec3&, const glm::vec3, glm::vec3, float);
void rotate(glm::vec3&, const glm::vec3, float);

//===========================================================================================================


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
    bool captureMouse{ true };
}

namespace lighting
{
    glm::vec3 lightPos{ 100.0f, 0.0f, 100.0f };
    glm::vec3 lightColor{ 0.42f, 0.39f, 0.19f };    // sun color
    float lightStrength{ 1.0f };
}

// create camera object
Camera camera(glm::vec3(0.0f, 0.0f, 20.0f));

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

    // set callbacks
    //--------------
    // set framebuffer callback
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // set glfw to capture cursor and set the callback
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    // set scroll callback
    glfwSetScrollCallback(window, scroll_callback);
    // set key callback
    glfwSetKeyCallback(window, key_callback);

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
    // sphere object
    Shader lightingShader("./shaders/shader.vs", "./shaders/shader.fs");
    // light source
    Shader lightSourceShader("./shaders/light-source-shader.vs", "./shaders/light-source-shader.fs");
    // sky
    Shader skyShader("./shaders/sky.vs", "./shaders/sky.fs");


    // create object
    //---------------
    // opaque sphere
    Sphere sphere(3.0f, 64, 32, true);
    glm::vec3 spherePosition{ 0.0f, 0.0f, 0.0f };
    glm::vec3 sphereColor{ 0.8f, 0.8f, 1.0f };
    
    // light source sphere
    Sphere lightSphere(3.0f, 32, 16, true);
    // light position and color is in lighting namespace

    // sky
    Sphere skySphere(1000.0f, 18, 6, true);
    const auto& skySpherePosition{ camera.position };       // the sky sphere position will follow the camera; thus, the camera will never be outside the sky


    // texture for opaque sphere
    //--------
    unsigned int textureID{};
    glGenTextures(1, &textureID);

    // bind
    glBindTexture(GL_TEXTURE_2D, textureID);

    // set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image (texture 0)
    int imageWidth, imageHeight, nrChannels;
    // stbi_set_flip_vertically_on_load(true);     // fix flipped image when loaded
    unsigned char* imageData = stbi_load("./textures/8k_mars.jpg", &imageWidth, &imageHeight, &nrChannels, 0);
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

    lightingShader.use();
    lightingShader.setInt("texture0", 0);


    // sky texture
    // //------------
    unsigned int skyTextureID{};
    glGenTextures(1, &skyTextureID);

    // bind
    glBindTexture(GL_TEXTURE_2D, skyTextureID);

    // set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image (texture 0)
    // stbi_set_flip_vertically_on_load(true);     // fix flipped image when loaded
    imageData = stbi_load("./textures/8k_stars_milky_way.jpg", &imageWidth, &imageHeight, &nrChannels, 0);
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

    skyShader.use();
    skyShader.setInt("texture0", 1);

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

        // activate texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, skyTextureID);


        // object
        //-------
        lightingShader.use();

        // rotate lightPos
        // orbit(lighting::lightPos, glm::vec3(0.0f, 1.0f, 1.0f), spherePosition, 1.0f);            // orbit

        // position uniform
        lightingShader.setVec3("lightPos", lighting::lightPos);
        lightingShader.setVec3("viewPos", camera.position);

        // color uniform
        lightingShader.setVec3("objectColor", sphereColor);        
        lightingShader.setVec3("lightColor", lighting::lightColor);

        // light strength uniform
        lightingShader.setFloat("lightStrength", lighting::lightStrength);
        
        // projection matrix changes a lot because of the aspect ratio, so we'll update it
        auto projection { glm::perspective(glm::radians(camera.fov), configuration::aspectRatio, 0.01f, 1000.0f) };
        lightingShader.setMat4("projection", projection);

        // view is handled by camera class
        auto view { camera.getViewMatrix() };
        lightingShader.setMat4("view", view);
        
        // model matrix
        auto model_object{ glm::translate(glm::mat4(1.0f), spherePosition)};

        // tilt object 23.5 deg
        model_object = glm::rotate(model_object, glm::radians<float>(23.5f), glm::vec3(0.0f, 0.0f, 1.0f));

        // rotate object
        model_object = glm::rotate(model_object, 0.1f*static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 1.0f, 0.0f));
        lightingShader.setMat4("model", model_object);

        // draw
        sphere.draw();
        //------


        // sky
        //----
        skyShader.use();

        skyShader.setMat4("projection", projection);
        skyShader.setMat4("view", view);
        auto model_skyObject{ glm::translate(glm::mat4(1.0f), skySpherePosition) };
        skyShader.setMat4("model", model_skyObject);

        skySphere.draw();
        //----


        // light source (cube)
        //--------------------

        // use shader
        lightSourceShader.use();

        lightSourceShader.setVec3("lightColor", lighting::lightColor);

        // view and projection is the same
        lightSourceShader.setMat4("projection", projection);
        lightSourceShader.setMat4("view", view);

        // model is different
        auto model_light{ glm::translate(glm::mat4(1.0f), lighting::lightPos) };
        // model_light = glm::scale(model_light, glm::vec3(0.2f));
        lightSourceShader.setMat4("model", model_light);
        
        // draw
        lightSphere.draw();
        //-------------------


        glfwSwapBuffers(window);
        glfwPollEvents();
        updateDeltaTime();
    }

    // de-allocate all resources
    sphere.deleteBuffers();
    lightSphere.deleteBuffers();
    skySphere.deleteBuffers();

    // clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}

//===========================================================================================================


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
        camera.lookAtOrigin();      // look at (0,0,0)
        mouse::firstMouse = !mouse::firstMouse;
    }
}

// for continuous input
void processInput(GLFWwindow* window, Shader& shader)
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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.moveCamera(CameraMovement::UPWARD, timing::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.moveCamera(CameraMovement::DOWNWARD, timing::deltaTime);

    // change camera speed
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
        camera.speed *= 1.01f;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
        camera.speed /= 1.01f;

    // increase/decrease lightStrength
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
        lighting::lightStrength /= 1.05;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
        lighting::lightStrength *= 1.05;

    // print fps
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        std::cout << "fps: " << static_cast<int>(1/timing::deltaTime) << '\n';
}

// record frame draw time
void updateDeltaTime()
{
    float currentFrame{ static_cast<float>(glfwGetTime()) };
    timing::deltaTime = currentFrame - timing::lastFrame;
    timing::lastFrame = currentFrame;
}

// orbit
void orbit(glm::vec3& orbitObjectPosition, const glm::vec3 orbitAxis, glm::vec3 orbitCenter, float orbitSpeed = 1.0f)
{
    float radius{ glm::length(orbitObjectPosition - orbitCenter) };

    // orbit coordinate
    glm::vec4 orbitPosition(1.0f);
    orbitPosition.x = radius * sin(static_cast<float>(glfwGetTime()) * orbitSpeed);
    orbitPosition.z = radius * cos(static_cast<float>(glfwGetTime()) * orbitSpeed);
    orbitPosition.y = 0.0f;

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

// rotate
void rotate(glm::vec3& rotatingObjectPosition, const glm::vec3 rotateAxis, float rotateSpeed = 1.0f)
{
    // TODO: create a rotating matrix then apply it to rotatingObjectPosition
}

// make texture then bind it to the shader