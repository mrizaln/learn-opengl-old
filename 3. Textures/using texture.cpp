#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shader_header/shader.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window);

namespace globals
{
    const int SCR_WIDTH { 800 };
    const int SCR_HEIGHT { 600 };
    float ASPECT_RATIO { SCR_WIDTH / static_cast<float>(SCR_HEIGHT) };
}

int main()
{
    // initialize glfw
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(globals::SCR_WIDTH, globals::SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to Create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))        // bool == 0 if success
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Shader theShader("default-vertex-shader.glsl", "default-fragment-shader.glsl");         // using default shader
    Shader theShader;

    // vertex data
    float vertices[] {
        // position                                     // colors
         0.0f,             0.6f*globals::ASPECT_RATIO, 0.0f,     1.0f, 0.0f, 0.0f,   // top
        -3*sqrt(3)/10.0f, -0.3f*globals::ASPECT_RATIO, 0.0f,     0.0f, 1.0f, 0.0f,   // bottom left
         3*sqrt(3)/10.0f, -0.3f*globals::ASPECT_RATIO, 0.0f,     0.0f, 0.0f, 1.0f,   // bottom right
    };

    unsigned int VAO;
    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2, 0.3, 0.4, 1);
        glClear(GL_COLOR_BUFFER_BIT);

        theShader.use();
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    globals::ASPECT_RATIO = width / static_cast<float>(height);
    // std::cout << "aspect ratio: " << globals::ASPECT_RATIO << '\n';
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}