// glad
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


/*---------------------------------------------------------------------------------------
                    ============[ coordinate systems ]============
---------------------------------------------------------------------------------------*/

/*
  - opengl expects all the vertices to be in normalized device coordinates after each
    vertex shader run.
  - that is, the coordinates of each vertex should be between -1.0 and 1.0, coordinates
    outside this range will not be visible.
  - what we usually do, is specify the coordinates in a range (or space) we determine
    ourselves and in the vertex shader transform these coordinates to normalized device
    coordinates (NDC).
  - these NDC are then given to the rasterizer to transform them to 2d coordinates/pixel
    on your screen.

  - transforming coordinates to NDC is usually accompplished in a step-by-step fashion
    where we transform an object's vertices to several coordinate systems before finally
    transforming them to NDC.
  - the advantage of transforming them to several /intermediate/ coordinate systems is
    that some operations/calculations are easier in certain coordinate systems aas will
    soon become apparent.
  - there are a total of 5 different coordinate systems that are of importance to use:
        1. local space (or object space)
        2. world space
        3. view space (or eye space)
        4. clip space
        5. screen space
*/


    //------------------------------
    //    [ the global picture ]
    //------------------------------

    /*
      - to transform the coordinates from one space to the next coordinate space, we'll
        use several transformation matrices of which the most important are [model], [view],
        and, [projection] matrix.
      - our vertex coordinates first start in local space and are then further processed to
        world coodinates, view coordinates, clip coordinates and eventually end up as screen
        coordinates.
    */


    //-----------------------
    //    [ local space ]
    //-----------------------
    
    // local space is the coordinate space that is local to your object.


    //-----------------------
    //    [ world space ]
    //-----------------------
    
    /*
      - this is the coordinate space where you wannt your objects transformed to in such a
        way that they're all scattered around the place.
      - the coordinates of your object are transformed from loacal to world space; this is
        accomplished with the [model] matrix.
      - the model matrix is a transformation matrix that translates, scales and/or rotates
        your object to place it in the world at a location/orientation they belong to.
    */


    //----------------------
    //    [ view space ]
    //----------------------

    /*
      - the view space is what people usually refer to as the [camera] of OpenGL.
      - the view space is the result of transforming your world-space coordinates to
        coordinates that are in front of the user's view.
      - the view space is thus the space as seen from the camera's point of view.
    */


    //----------------------
    //    [ clip space ]
    //----------------------

    /*
      - at the end of each vertex shader run, OpenGL expects the coordinates to be within
        a specific range and any coordinate that falls outside this range is [clipped].
      - coordinates that are clipped are discarded, so the remaining coordinates will end
        up as fragments visible on your screen.

      - because specifying all the visible coordinates to be within the range -1.0 and 1.0
        isn't really intuitive, we specify our own coordinate set to work in and convert
        those back to NDC as OpenGL expects them.

      - to transform vertex coordinates from view to clip-space we define a [projection matrix]
        that specifies a range of coordinates e.g -1000 and 1000 in each dimension.
      - the projection matrix then tranforms coordinates within this specified range to NDC.

      - this viewing box projection matrix creates is called a [frustum] and each coordinate
        that ends up inside this frustum will end up on the user's screen.
      - the total process to convert coordinates within a specified range to NDC that can
        be easily be mapped to 2D view-space coordinates is called [projection] since the
        projection matrix [projects] 3D coordinates to the easy-to-map-to-2D NDC.

      - once all the vertices are transformed to clip space, a final operation called [perspective
        division] is performed where we divide the x, y, and x components of the position
        vectors by the vector's homogeneous w component; perspective division is what transforms
        the 4D clip space coordinates to 3D NDC.
      - this step is performed automatically at the ened of the vertex shader step.

      - it is after this stage where the resulting coordinates are mapped to screen coordinates
        (using the settings of glViewport) and turned into fragments.

      - the projection matrix to transform view coordinates to clip coordinates usually takes
        two different forms, where each form defined its own unique frustum.
      - we can either create an [orthographic] projection matrix or a [perspective] projection
        matrix.
    */


        // [[ orthographic projection ]]

        /*
          - an orthographic projection matrix defines a cube-like frustum box that defines
            the clipping space where each vertex outisde this box is clipped.
          - when creating an orthographic projection matrix we specify the width, height,
            and length of the visible frustum.
          - all the coordinates inside this frustum will end up within the NDC range after
            transformed by its matrix and this won't be clipped.

          - the frustum defines the visible coordinates and is specified by a width, height,
            and a [near] and [far] plane.
          - any coordinate in front of the near plane is clipped and the same applies to
            coordiantes behind the far plane.
          - the orthographic frustum directly maps all coordinates inside the frustum to
            normalized device coordinates without any special side effects since it won't
            touch the w component of the transformed vector; if the w componene remains
            equal to 1.0 perspective division won't change the coordinates.
        */

            // to create an orthographic projection matrix we make use of GLM's built in function:
            namespace creating_projection_matrix
            {
                glm::mat4 orthoProj { glm::ortho(0.0f, 800.0f, 0.0f, 600.0f, 0.1f, 100.0f) };
            }

        /*
          > 1st & 2nd param: specify the left and right coordinate of the frustum
          > 3rd & 4th param: specify the bottom and top part of the frustum\
          > 5th & 6th param: define the distance between the near and far plane.

          - this specific projection  matrix transformas all coordinates between these x,
            y, and z range values to NDC.
        */


        // [[ perspective projection ]]

        /*
          - the perspective projection mimics how we see things in real life.
          - the perspective projection matrix maps a given frustum range to clip space, but
            also manipulates the w value of each vertex coordinate in such a way that the
            further away a vertex coordiante is from the viewer, the higher this w component
            becomes.
          - once the coordinates are transformed to clip space, theay are in the range -w
            to w (anything outside this range is clipped).
          - OpenGL requires that the visible coordinates fall berween the range -1.0 and 1.0
            thus perspective division is applied at last:
                            ⎡x/w⎤
                    out  =  ⎢y/w⎥
                            ⎣z/w⎦
          - this is another reason why w component is important, since it helps us with the
            perspective projection.
          - the resulting coordinates are then in NDC.

          - a perspective frustum can be visualized as a non-uniformly shaped box where
            each coordinate inside this box will be mapped to a point in clip space.
        */

            // a perspective projection matrix can be created in GLM as follows:
            namespace creating_projection_matrix
            {
                int width { 1 };
                int height { 1 };
                glm::mat4 persProj { glm::perspective(glm::radians(45.0f), width/static_cast<float>(height), 0.1f, 100.0f) };
            }

        /*
          > 1st param        : defines the FOV (field of view) value.
          > 2nd param        : sets the aspect ratio value (view port width / height)
          > 3rd and 4th param: set the near and far plane of the frustum.
        */


    //-------------------------------
    //    putting it all together
    //-------------------------------

    /*
      - we create a transformation matrix for each of the aforementioned steps: model, view,
        and projection matrix.

            Vclip = Mproj ∙ Mview ∙ Mmodel ∙ Vlocal
    */




/*---------------------------------------------------------------------------------------
                      ============[ Going 3D ]============
---------------------------------------------------------------------------------------*/

/*
  - to start drawing in 3D we'll first create a model matrix.
  - the model matrix consists of translations, scaling and/or rotations we'de like to apply
    to transform all object's vertices to the global world space.

  - let's transform our plane a bit by rotating it on the x-axis so it looks like it's laying
    on the floor
*/

    namespace going_3d
    {
        glm::mat4 createModelMatrix()
        {
            glm::mat4 model { glm::mat4(1.0f) };
            model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            return model;
        }
    }

/*
  - next, we need to create a view matrix.

  - we want to move slightly backwards in the scene so the object becomes visible.
  - to move around the scene, think about the following:
        to move a camera backwards, is the same as moving the entire scen forward.
  - because we want to move backwards and since OpenGL is a right-handed system, we have
    to move in the positive z-axis.
  - this gives the impression that we are moving backwards.
*/

    namespace going_3d
    {
        glm::mat4 createViewMatrix()
        {
            glm::mat4 view { glm::mat4(1.0f) };
            view = glm::translate(view, glm::vec3(0.0f, 0.0f, 1.0f));
            return view;
        }
    }

/*
  - the last thing we need to define is the projection matrix.
  - we want to use perspective projection for our scene so we'll declate the projection
    matrix like this:
*/

    namespace going_3d
    {
        glm::mat4 createProjectionMatrix()
        {
            glm::mat4 projection { glm::perspective(glm::radians(45.0f), 800.0f / 600.0f , 0.1f, 100.0f) };
            return projection;
        }
    }

/*
  - now that we created the transformation matrices, we should pass them to our shaders.
  - first, declare the transformation matrices as unifroms in the vertex shader and multiply
    them with the vertex coordinates:
*/

#ifdef THIS_IS_GLSL_CODE
    #version 330 core
    
    layout (location = 0) in vec3 aPos
    ...
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;

    void main()
    {
        gl_Positoin = projection * view * model * vec4(aPos, 1.0f)
        ...
    }
#endif

/*
  - we should also send the matrices to the shader.
  - this is usually done each frame since the transformation matrices tend to change a lot.
*/
    unsigned int shader_ID {};  // placeholder, use your shader ID
    int modelLoc      { glGetUniformLocation(shader_ID, "model") };
    int viewLoc       { glGetUniformLocation(shader_ID, "view") };
    int projectionLoc { glGetUniformLocation(shader_ID, "projection") };




/*---------------------------------------------------------------------------------------
                        ============[ More 3D ]============
---------------------------------------------------------------------------------------*/

/*
  - so far, we've been working with 2D plane, even in 3D space, so let's extend the 2D plane
    to a 3D cube.
  - to render a cube, we need a total of 36 vertices (6 faces * 2 triangles * 3 vertices each).
*/

    namespace more_3d
    {
        float vertices[] = {
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
    }

/*
    for fun, we'll let the cube rotate over time:
*/

    namespace more_3d
    {   
        glm::mat4& getModelMatrix()
        {
            static glm::mat4 s_model(1.0f);
            s_model = glm::rotate(s_model, static_cast<float>(glfwGetTime()) * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
            return s_model;
        }
    }

/*
    and then we'll draw the cube using glDrawArrays.
*/

/*-----------------------------------------------------------
    try to run "coordinate sytems - 2. more 3d.cpp" file
-----------------------------------------------------------*/

/*
  - when you run it, you'll see the cube rotates... but something is off.
  - some sides of the cubes are drawn over other sides of the coba.
  - this happens because when OpenGL draws your cube triangle-by-triable, fragment-by-fragment,
    it will overwrite any pixel color that may have already been drawn there before

  - fortunately, OpenGL stores depth information in a buffer called the [z-buffer] that allows
    OpenGL to decide when to draw over a pixel and when not to.
*/


    //--------------------
    //    [ Z-buffer ]
    //--------------------

    /*
      - OpenGL stores all its depth information in a z-buffer, also known as [depth buffer].
      - GLFW automatically creates such a buffer for you.

      - The depth is stored within each fragment (as the fragment z value) and whenever the
        fragment wants to output its color, OpenGL compares its depth values with the z-buffer.
      - if the current fragment is behind the other fragment it is discarded, otherwise
        overwritten.
      - this process is called [depth testing] and is done automatically by OpenGL.

      - depth testing is disabled by default.
      - to enable it we can use glEnable() function:
    */
        void enableDepthTesting()
        {
            glEnable(GL_DEPTH_TEST);
        }
    /*
      - we also need to clear the depth buffer each frame like the color buffer
    */
        void clearDepthBuffer()     // and color buffer
        {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
