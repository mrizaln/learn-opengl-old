// glad
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// shader
#include <shader_header/shader.h>

/*---------------------------------------------------------------------------------------
                          ============[ Colors ]============
---------------------------------------------------------------------------------------*/

/*
  - in the real world, colors can take any known color value with each object having its
    own color(s).
  - in the digital world we need to map the (infinite) real colors to (limited) digital
    values and therefore not all real-world colors can be represented digitally.
  - colors are digitall represented using a red, green, and blue component (RGB).
  - using just those 3 values, within range of [0, 1], we can represent almost any color
    there is.

  - the color of an object we see in real life is not the color it actually has, but is
    the color reflected from the object.
  - the color from a light source is all absorbed except the color that we perceive.

  - these rules of color reflection apply directly in graphics-land.
  - when we define a light source in OpenGL we want to give this light source a color.
  - we'll give the light source a white color.
  - if we would then multiply the light source's color with an object's color value, the
    resulting color would be the reflected color of the object.
*/

    namespace colors
    {
        glm::vec3 lightColor{ 1.0f, 1.0f, 1.0f };
        glm::vec3 toyColor{ 1.0f, 0.5f, 0.31f };
        auto result{ lightColor * toyColor };           // == (1.0f, 0.5f, 0.31f)
    }




/*---------------------------------------------------------------------------------------
                       ============[ A Lighting Scene ]============
---------------------------------------------------------------------------------------*/

/*
  - the first thing we need is an object to cast the light on and we'll use the container
    cube from previous chapters.
  - we'll also be needing a light object to show where the light source is located in the
    3D scene (for simplicity sake, we'll use a cube as well).
*/

/*
  - so the first thing we'll need is a vertex shader to draw the container.
  - the vertex positions of the container remain the same (although we won't be needing
    texture coordinates this time).

        [ open shader.vs on the same directory as this file ]

  - because we're also going to render a light source cuve, we want to generate a new VAO
    specifically for the light source.
  - we could render the light source with the same VAO and then do a few light position
    transformations on the model matrix, but in the upcoming chapters we'll be changing
    the vertex data and attribute pointers of the container object quite often and we do
    not want these changes to propagate to the light source object, so we'll create new VAO.
*/

    namespace light
    {
        // assume that the VBO is already defined
        unsigned int VBO{};

        // light VAO
        unsigned int lightVAO{};

        void configure_attrib()
        {
            glGenVertexArrays(1, &lightVAO);
            glBindVertexArray(lightVAO);

            // we only need to bind to the VBO, the container's VBO's data already contains the data
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), static_cast<void*>(0));
            glEnableVertexAttribArray(0);

            // enable depth testing
            glEnable(GL_DEPTH_TEST);
        }
    }

/*
  - there is one thing left to define and that is the fragment shader for both the container
    and the light source.

        [ open shader.fs on the same directory as this file ]

  - the fragment shader accepts both an object color and light color from a uniform variable.
  - here, we multiply the light's color with the object's (reflected) color like we discussed.
  - let's set the object's color and the light color
*/

    namespace light
    {
        Shader lightingShader("shader.vs", "shader.fs");

        void set_shader()
        {
            lightingShader.use();
            lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
            lightingShader.setVec3("lightColor",  1.0f, 1.0f, 1.0f);
        }
    }

/*
  - one thing left to note is that when we start to update these /lighting shaders/ in the
    next chapters, the light source cube would also be affected and this is not what we want.
  - we want the light source to have a constant bright color, unaffected by other color
    changes.

  - to accomplish this, we need to create a second set of shaders that we'll use to draw
    the light source cube, thus beng safe from any changes from the lighting shaders.
  - the vertex shader is the same as the lighting shader so you can simply copy the source
    code over.
  - the fragment shader of the light source cube ensures the cube's color remains bright
    by defining a constant white color on the lamp.

        [ open light-source-shader.fs on the same directory as this file ]
*/

/*
  - when we want to render, we want to render the container object using the lighting shader
    we just defined, and when we want to draw the light source we use the light source's
    shaders.
  - the main purpose of the light source cube is to show where the light comes from.
*/

    namespace light
    {
        glm::vec3 lightPos{ 1.2f, 2.0f, 2.0f };

        glm::mat4 model_light(1.0f);
        void set_light_source_model_matrix()
        {
            model_light = glm::translate(model_light, lightPos);
            model_light = glm::scale(model_light, glm::vec3(0.2f));
        }

        Shader lightCubeShader("light-source-shader.vs", "light-source-shader.fs");

        void set_lightSourceShader()
        {
            lightCubeShader.use();
        }
    }

// after that we draw the objects