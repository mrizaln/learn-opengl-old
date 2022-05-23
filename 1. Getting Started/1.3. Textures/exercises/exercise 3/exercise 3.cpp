/*---------------------------------------------------------------------------------------
Description: Try to display only the center pixels of the texture image on the rectangle
             in such a way that the individual pixels are getting visible by changing the
             texture coordinates.
---------------------------------------------------------------------------------------*/

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

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

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(globals::SCR_WIDTH, globals::SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to Create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))        // bool == 0 if success
    {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // build and compile shader
    Shader theShader("exercise 3.vs", "exercise 3.fs");

    // vertex data
    float vertices[] {
        // positions            // colors               // texture coords
         0.5f,  0.5f, 0.0f,     1.0f, 0.0f, 0.0f,       0.55f, 0.55f,         // top-right
         0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,       0.55f, 0.45f,         // bottom-right
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,       0.45f, 0.45f,         // bottom-left
        -0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 0.0f,       0.45f, 0.55f          // top-left
    };

    unsigned int indices[] = {
        0, 1, 3,        // first triangle
        1, 2, 3         // second triangle
    };

    // setting up buffer(s)
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


/*-----------------------------------------------------------------------------------------------------------
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // load image (texture 0)
    int imageWidth, imageHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true);     // fix flipped image when loaded
    unsigned char* imageData { stbi_load("../../img/container.jpg", &imageWidth, &imageHeight, &nrChannels, 0) };
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // load image (texture 1)
    imageData = stbi_load("../../img/awesomeface.png", &imageWidth, &imageHeight, &nrChannels, 0);
    if (imageData)
    {
        // now generate texture from image
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, imageWidth, imageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);
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
//===========================================================================================================


    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind texture to corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureID[1]);

        theShader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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