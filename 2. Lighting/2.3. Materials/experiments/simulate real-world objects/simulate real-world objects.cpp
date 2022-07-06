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

// shapes
#include <shapes/sphere/sphere.h>
#include <shapes/cube/cube.h>

// camera
#include <camera_header/camera.h>

// materials
#include <material_collection/material_collection.h>

// STL
#include <iostream>

//===========================================================================================================


void framebuffer_size_callback(GLFWwindow*, int, int);
void cursor_position_callback(GLFWwindow*, double, double);
void scroll_callback(GLFWwindow*, double, double);
void key_callback(GLFWwindow*, int, int, int, int);

void processInput(GLFWwindow*);
void updateDeltaTime();
void changeAlpha(const Shader&, const std::string&);

void orbit(glm::vec3&, const glm::vec3, const glm::vec3, float);

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

namespace varyingMaterial
{
    int materialArrayIndex{ 0 };
    auto material{ materialCollection::materialArray[materialArrayIndex] };
}

// create camera object
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));


// material
// struct Material
// {
//     glm::vec3 ambient{ 1.0f, 1.0f, 1.0f };
//     glm::vec3 diffuse{ 1.0f, 1.0f, 1.0f };
//     glm::vec3 specular{ 1.0f, 1.0f, 1.0f };
//     float shininess{ 1.0f };
// };


// object 
template <class object_type>
class Object
{
    object_type object{};
    glm::vec3 position{};
    glm::vec3 scale{ 1.0f };
    Shader shader{};
    Material material{};
    glm::mat4 modelMatrix{ glm::mat4(1.0f) };

public:
    Object(object_type obj, glm::vec3 objPos, Shader objShader, Material material)
        : object{ obj }
        , position { objPos }
        , scale{ 1.0f }
        , shader{ objShader }
        , material{ material }
        , modelMatrix{ glm::mat4(1.0f) }
    {
    }

    void setPosition(glm::vec3 pos) { position = pos; }
    void setPosition(float x, float y, float z) { setPosition(glm::vec3{ x, y, z }); }
    void setScale(float scaling) { scale = scaling; }
    void setShader(Shader shdr) { shader = shdr; }
    void setMaterial(Material mat) { material = mat; }

    auto& getObject() { return object; }
    auto& getPosition() { return position; }
    auto& getShader() { return shader; }
    auto& getMaterial() { return material; }
    auto& getModelMatrix() { updateModelMatrix(); return modelMatrix; }

    // apply material through shader
    void applyShader()
    {
        shader.use();
        shader.setVec3("material.ambient",    material.ambient);
        shader.setVec3("material.diffuse",    material.diffuse);
        shader.setVec3("material.specular",   material.specular);
        shader.setFloat("material.shininess", material.shininess);
    }


private:
    void updateModelMatrix()
    {
        modelMatrix = glm::translate(glm::mat4(1.0f), position);
        modelMatrix = glm::scale(modelMatrix, scale);
    }
};


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
    // a cube container object
    Object<Cube> cube(
        Cube(0.5f),
        glm::vec3{ 0.0f,  0.0f,  0.0f},
        Shader("shader.vs", "shader.fs"),
        Material{
            { 1.0f, 0.5f, 0.31f },
            { 1.0f, 0.5f, 0.31f },
            { 0.5f, 0.5f, 0.5f  },
            32.0f
        }
    );

    // a cube light source object
    Object<Cube> light(
        Cube(0.1f),
        glm::vec3{ 1.2f, 1.0f, 2.0f },
        Shader("light-source-shader.vs", "light-source-shader.fs"),
        Material{}
    );
    

    // set material uniform in shader.fs;
    cube.applyShader();

    cube.getShader().setVec3("light.ambient",  glm::vec3{ 0.2f, 0.2f, 0.2f });
    cube.getShader().setVec3("light.diffuse",  glm::vec3{ 0.5f, 0.5f, 0.5f });
    cube.getShader().setVec3("light.specular", glm::vec3{ 1.0f, 1.0f, 1.0f });

    //=======================================================================================================

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

        // clear color buffer and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        // container
        //----------

        // use shader
        cube.getShader().use();

        // change material
        cube.setMaterial(varyingMaterial::material);
        cube.applyShader();

        // set uniform
        cube.getShader().setVec3("lightPos", light.getPosition());
        cube.getShader().setVec3("viewPos", camera.position);

        // view is handled by camera class
        auto view { camera.getViewMatrix() };
        cube.getShader().setMat4("view", view);
        
        // projection matrix changes a lot because of the aspect ratio, so we'll update it
        auto projection { glm::perspective(glm::radians(camera.fov), configuration::aspectRatio, 0.1f, 100.0f) };
        cube.getShader().setMat4("projection", projection);
        
        // model matrix
        cube.getShader().setMat4("model", glm::rotate(cube.getModelMatrix(), static_cast<float>(glfwGetTime()), glm::vec3{ 0.0f, 1.0f, 0.0f }));

        // draw
        cube.getObject().draw();


        // light source (cube)
        //--------------------

        // use shader
        light.getShader().use();

        // view and projection is the same
        light.getShader().setMat4("view", view);
        light.getShader().setMat4("projection", projection);

        // model is different
        orbit(light.getPosition(), glm::vec3{ 0.1f, 0.5f, 0.2f }, cube.getPosition(), 2.0f);
        light.getShader().setMat4("model", light.getModelMatrix());

        // light color
        light.getShader().setVec3("color", light.getMaterial().diffuse);
        
        // draw
        light.getObject().draw();
        //-------------------------------------------------------

        glfwSwapBuffers(window);
        glfwPollEvents();
        updateDeltaTime();
    }

    // clearing all previously allocated GLFW resources.
    // cube.getObject().~Cube();
    light.getObject().~Cube();
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
        // std::cout << "Before: " << camera.front.x << ' ' << camera.front.y << ' ' << camera.front.z << " | " << camera.yaw << ' ' << camera.pitch << '\n';
        camera.lookAtOrigin();      // look at (0,0,0)
        mouse::firstMouse = true;
        // std::cout << "After: " << camera.front.x << ' ' << camera.front.y << ' ' << camera.front.z << " | " << camera.yaw << ' ' << camera.pitch << "\n\n";
    }

    // varying material
    if (key == GLFW_KEY_L && action == GLFW_PRESS)
    {
        using namespace varyingMaterial;
        
        materialArrayIndex++;
        if (materialArrayIndex > 11)
            materialArrayIndex = 0;
            
        material = materialCollection::materialArray[materialArrayIndex];
    }

    if (key == GLFW_KEY_J && action == GLFW_PRESS)
    {
        using namespace varyingMaterial;
        
        materialArrayIndex--;
        if (materialArrayIndex < 0)
            materialArrayIndex = 11;
            
        material = materialCollection::materialArray[materialArrayIndex];
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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.moveCamera(CameraMovement::UPWARD, timing::deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.moveCamera(CameraMovement::DOWNWARD, timing::deltaTime);

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
void orbit(glm::vec3& orbitObjectPosition, const glm::vec3 orbitAxis, const glm::vec3 orbitCenter, float orbitSpeed = 1.0f)
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