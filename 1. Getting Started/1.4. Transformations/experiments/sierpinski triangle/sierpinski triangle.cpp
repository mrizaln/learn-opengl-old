/*---------------------------------------------------------------------------------------
Description: Try drawing a second container with another call to glDrawElements but place
             it at a different position using transformations only. Make sure this second
             container is placed at the top-left of the window and instead of rotating,
             scale it over time (using the sin function is useful here; note that using
             sin will cause the object to invert as soon as a negative scale is applied)
---------------------------------------------------------------------------------------*/


// glad
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// stb_image
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

// GLM
#include <glm/glm.hpp>                      // ⎫
#include <glm/gtc/matrix_transform.hpp>     // ⎬ for transformations
#include <glm/gtc/type_ptr.hpp>             // ⎭

// shader
#include <shader_header/shader.h>




//===========================================================================================================
//===========================================================================================================

// [ forward declarations ]

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void processInput(GLFWwindow *window, const Shader &shader);
void changeAlpha(const Shader &shader, const std::string &increment);
void drawTriangle(int depth, Shader& shader, glm::mat4 transformationMatrix, glm::vec3 translationVector);




//===========================================================================================================
//===========================================================================================================

// [ configurations ]

namespace globals
{
    constexpr int SCR_WIDTH { 800 };
    constexpr int SCR_HEIGHT { 600 };
    
    float ASPECT_RATIO { SCR_WIDTH / static_cast<float>(SCR_HEIGHT) };
    glm::vec3 TRANSLATION { glm::vec3(0.0f, 0.0f, 0.0f) };
    float ZOOM { 1.0f };
    int DEPTH { 1 };
}

namespace triangle
{
    constexpr float width { 1 };
    constexpr float height { 1 };
}



//===========================================================================================================
//===========================================================================================================

// [ main program ]

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

    // set glfw input mode
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);      // the key state will remain GLFW_PRESS until polled with glfwGetKey

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
        // positions            // colors               // texture coords
         0.57735f,  1/3.0f, 0.0f,     1.0f, 0.0f, 0.0f,       1.0f, 1.0f,         // top-right
        -0.57735f,  1/3.0f, 0.0f,     0.0f, 1.0f, 0.0f,       0.0f, 1.0f,         // top-left
         0.0f,     -2/3.0f, 0.0f,     0.0f, 0.0f, 1.0f,       0.5f, 0.0f,         // bottom
    };

    unsigned int indices[] = {
        0, 1, 2,        // first triangle
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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // load image (texture 0)
    int imageWidth, imageHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true);     // fix flipped image when loaded
    unsigned char* imageData { stbi_load("../../img/nakiri_2x.jpg", &imageWidth, &imageHeight, &nrChannels, 0) };
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

    // load image (texture 1)
    imageData = stbi_load("../../img/kemomimi.jpg", &imageWidth, &imageHeight, &nrChannels, 0);
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
//===========================================================================================================

/*-----------------------------------------------------------------------------------------------------------
                                ============[ Transformations ]============
-----------------------------------------------------------------------------------------------------------*/
    // create transformation matrix
    // glm::mat4 trans { glm::mat4(1.0f) };            // initialize transformation matrix
    // trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0f));     // rotate
    // trans = glm::scale(trans, glm::vec3(0.5f, 0.5f, 0.5f));                         // scale

    // int transformLoc { glGetUniformLocation(theShader.ID, "transform") };
    // glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
    // // or using
    // // theShader.setMat4("transform", trans);    
//===========================================================================================================

    // render loop
    while (!glfwWindowShouldClose(window))
    {
        // input
        processInput(window, theShader);

        // render
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind texture to corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureID[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textureID[1]);

        glBindVertexArray(VAO);
        
        // transform and draw
        //-------------------
        glm::mat4 trans { glm::mat4(1.0f) };

        // preserve aspect ratio
        trans = glm::scale(trans, glm::vec3(1/globals::ASPECT_RATIO, 1.0f, 1.0f));      // preserve the original aspect ration of the object

        // zoom
        trans = glm::scale(trans, glm::vec3(globals::ZOOM));
        
        // translation using WASD key
        trans = glm::translate(trans, globals::TRANSLATION);

        // draw the carpet
        drawTriangle(globals::DEPTH, theShader, trans, glm::vec3(0.0f));
        //-------------------


        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // de-allocate all resources
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;
}




//===========================================================================================================
//===========================================================================================================

// [ forwarded functions definition ]

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    globals::ASPECT_RATIO = width / static_cast<float>(height);
    // std::cout << "aspect ratio: " << globals::ASPECT_RATIO << '\n';
}


void processInput(GLFWwindow *window, const Shader &shader)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    // change alpha
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        changeAlpha(shader, "inc");
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        changeAlpha(shader, "dec");

    // change depth
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        {
            static int waitUP { 10 };
            if (--waitUP <= 0)
                {
                    waitUP = 10;
                    ++globals::DEPTH;
                }
        }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        {
            static int waitDOWN { 10 };
            if (--waitDOWN <= 0)
                {
                    waitDOWN = 10;
                    --globals::DEPTH;
                }
        }

    // zoom
    if (glfwGetKey(window, GLFW_KEY_PERIOD) == GLFW_PRESS)
        globals::ZOOM *= 1.1f;
    if (glfwGetKey(window, GLFW_KEY_COMMA) == GLFW_PRESS)
        globals::ZOOM /= 1.1f;

    // translation
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        globals::TRANSLATION += -glm::vec3(0.0f, 0.05f/globals::ZOOM, 0.0f);      // minus sign at front indicate that we want to translate the view not the object itself (basically view translation == -object translation)
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        globals::TRANSLATION += -glm::vec3(0.0f, -0.05f/globals::ZOOM, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        globals::TRANSLATION += -glm::vec3(0.05f/globals::ZOOM, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        globals::TRANSLATION += -glm::vec3(-0.05f/globals::ZOOM, 0.0f, 0.0f);
    if (glfwGetKey(window, GLFW_KEY_BACKSPACE) == GLFW_PRESS)
        globals::TRANSLATION = glm::vec3(0.0f, 0.0f, 0.0f);
}


// uniform alpha increment handler (increment: up/down)
void changeAlpha(const Shader &shader, const std::string &increment)
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


// recursive transform: draw sierpinski carpet
void drawTriangle(int depth, Shader& shader, glm::mat4 transformationMatrix, glm::vec3 translationVector)
{
    static int s_max_depth { 10 };
    static int s_count { 0 };

    if (depth > s_max_depth) 
        depth = s_max_depth;   // bisi beurat teuing

    if (depth <= 0) {
        s_count = 0;
        return;
    }

    glm::mat4 triangleTransform { transformationMatrix };
    glm::vec3 carpetTranslate { translationVector };

    triangleTransform = glm::translate(triangleTransform, carpetTranslate);
    triangleTransform = glm::rotate(triangleTransform, 0*static_cast<float>(glfwGetTime())/(10.0f*globals::ZOOM)+glm::radians(120.0f*s_count), glm::vec3(0.0f, 0.0f, 1.0f));
    triangleTransform = glm::scale(triangleTransform, glm::vec3(1/2.0f, 1/2.0f, 1.0f));
    shader.setMat4("transform", triangleTransform);

    // draw
    glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

    // decrease depth
    --depth;
    ++s_count;

    // top
    carpetTranslate = glm::vec3(0.0f, 2/3.0f, 0.0f);
    drawTriangle(depth, shader, triangleTransform, carpetTranslate);

    // bottom-right
    carpetTranslate = glm::vec3(0.57735f, -1/3.0f, 0.0f);
    drawTriangle(depth, shader, triangleTransform, carpetTranslate);

    // bottom-left
    carpetTranslate = glm::vec3(-0.57735f, -1/3.0f, 0.0f);
    drawTriangle(depth, shader, triangleTransform, carpetTranslate);
}
