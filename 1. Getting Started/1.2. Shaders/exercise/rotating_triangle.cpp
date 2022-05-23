#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shader_header/shader.h>

#include <iostream>
#include <cmath>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow* window);
float variableColor(float period, float phase);
void rotateTriangle(double *arr, int size, int stride, int offset, double theta);

//settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
float ASPECT_RATIO = SCR_WIDTH/(float)SCR_HEIGHT;

int main()
{
    // initialize and configure GLFW
    //-----------------------------------------------------------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    //-----------------------------------------------------------------------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    //-----------------------------------------------------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // build and compile our shader program
    //-----------------------------------------------------------------------------------
    Shader theShader("rotating.vs", "shader.fs");

    // set up vertex data (and buffers(s)) and configure vertex attributes
    //-----------------------------------------------------------------------------------
    float vertices[] ={
        // position                                     // colors
         0.0f,             0.6f*ASPECT_RATIO, 0.0f,     1.0f, 0.0f, 0.0f,   // top
        -3*sqrt(3)/10.0f, -0.3f*ASPECT_RATIO, 0.0f,     0.0f, 1.0f, 0.0f,   // bottom left
         3*sqrt(3)/10.0f, -0.3f*ASPECT_RATIO, 0.0f,     0.0f, 0.0f, 1.0f,   // bottom right
    };

    unsigned int VAO;
    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    double bases[] = {1, 1, 1};
    // render loop
    //-----------------------------------------------------------------------------------
    while (!glfwWindowShouldClose(window))
    {
        // input
        //-------------------------------------------------------------------------------
        processInput(window);

        // render
        //-------------------------------------------------------------------------------
        float RED_COLOR = variableColor(4.4, 0.1);
        float GREEN_COLOR = variableColor(3.2, 0.3);
        float BLUE_COLOR = variableColor(6.1, 0.4);
        glClearColor(RED_COLOR, GREEN_COLOR, BLUE_COLOR, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // update the uniform position
        // double theta = M_PI*1e-8;
        // rotateTriangle( bases, 3, 3, 0, theta);
        // theShader.setVec3("aPosRotate", glm::vec3(bases[0], bases[1], bases[2]));
        
        // update xOffset
        theShader.setFloat("xOffset", (1 - 3*sqrt(3)/10.0f) * sin(glfwGetTime()/3.0f));
        theShader.setFloat("yOffset", (0.8 / 2) * cos(glfwGetTime()/3.0f) - 0.2);

        // render the triangle
        //-------------------------------------------------------------------------------
        theShader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // swap buffer and poll io events
        //-------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // deallocate all resources
    //-----------------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    // terminate glfw then exit
    //-----------------------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// use sin function to get a value between 0 and 1
//---------------------------------------------------------------------------------------
float variableColor(float period = 1.0f, float phase = 0)
{
    float time = glfwGetTime();
    period *= 5;
    return (15/33.0f) * (sin((2*M_PI / period) * time + phase)/2.0f + 0.5 + sin(3.5f* (2*M_PI / period) * time - phase)/4.0f + 0.25);
}

// rotate triangle
//---------------------------------------------------------------------------------------
void rotateTriangle(double *arr, int size, int stride, int offset, double theta)
{
    int count = offset;
    while (count <= size)
    {
        arr[count]   = arr[count]*(cos(theta)-1) - arr[count+1]*sin(theta);
        arr[count+1] = arr[count]*sin(theta) + arr[count+1]*(cos(theta)-1);
        count += stride;
    }
}

// process input
//---------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: resize viewport every time window resized
//---------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}
