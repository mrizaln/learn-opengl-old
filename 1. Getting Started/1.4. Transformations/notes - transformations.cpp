#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

/*---------------------------------------------------------------------------------------
                        ============[ linear alegbra ]============
---------------------------------------------------------------------------------------*/

// basically math: vectors, matrices, vector operations & matrix operations.

/*
        ⎡1   0   0   0⎤   ⎡x⎤   ⎡x⎤
        ⎢0   1   0   0⎥ ∙ ⎢y⎥ = ⎢y⎥
        ⎢0   0   1   0⎥   ⎢z⎥   ⎢z⎥
        ⎣0   0   0   1⎦   ⎣w⎦   ⎣w⎦

        ⎡ ⎤      ⎡⎣⎢⎥⎤⎦
        ⎢ ⎥
        ⎣ ⎦


    in OpenGL we usually dealing with 4x4 matrix (and 4D vector) or in 4D space
    the 4th component is called homogenous coordinates.

    [ 3D + homogenious coordinate ]

        > identity matrix

            ⎡1   0   0   0⎤   ⎡x⎤   ⎡x⎤
            ⎢0   1   0   0⎥ ∙ ⎢y⎥ = ⎢y⎥
            ⎢0   0   1   0⎥   ⎢z⎥   ⎢z⎥
            ⎣0   0   0   1⎦   ⎣1⎦   ⎣w⎦


        > 3D scaling

            ⎡S₁  0   0   0⎤   ⎡x⎤   ⎡S₁⋅x⎤
            ⎢0   S₂  0   0⎥ ∙ ⎢y⎥ = ⎢S₂⋅y⎥
            ⎢0   0   S₃  0⎥   ⎢z⎥   ⎢S₃⋅z⎥
            ⎣0   0   0   1⎦   ⎣1⎦   ⎣ w  ⎦

            *) Sₙ = scaling variables


        > 3D translation

            ⎡1   0   0   T₁⎤   ⎡x⎤   ⎡x + T₁⎤
            ⎢0   1   0   T₂⎥ ∙ ⎢y⎥ = ⎢y + T₂⎥
            ⎢0   0   1   T₃⎥   ⎢z⎥   ⎢z + T₃⎥
            ⎣0   0   0   1 ⎦   ⎣1⎦   ⎣  w   ⎦

            *) Tₙ           = translation variable
               (T₁, T₂, T₃) = translation vector
            
            this is the advantage of using homogenious

        > 3D rotation:

            panjang, males ngetik
    
*/

// combining matrices

/*
    it is advised to first do scaling operations then rotatoins and lastly translations
    when combining matrices otherwise they may (negatively) affect each other.
*/




/*---------------------------------------------------------------------------------------
                ============[ linear alegbra in practice ]============
---------------------------------------------------------------------------------------*/

// we can define our matrix and vector operation and all by ourself to use it on OpenGL, or
// we can use a library: GLM

    /*---------|
    |   GLM    |
    |----------|

    GLM is a header only mathematics lirary that is tailored fot OpenGL

    most of GLM's functionality that we need can be found in 3 headers files:
    */
        #include <glm/glm.hpp>
        #include <glm/gtc/matrix_transform.hpp>
        #include <glm/gtc/type_ptr.hpp>
    /*

    lets test if we can put our transformation knowledge to good use by translating a vector
    of (1, 0, 0) by (1, 1, 0)
    */
        namespace test
        {
            glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);          // homogeneous coordiate = 1
            glm::mat4 identity { glm::mat4(1.0f) };        // initialize identity matrix
            
            void translating_a_vector()
            {
                glm::mat4 trans { glm::translate(identity, glm::vec3(1.0f, 1.0f, 0.0f)) };     // create tnraslation matrix
                vec = trans * vec;      // translate vector using newly made translation matrix

                std::cout << "x = " << vec.x << '\n'
                          << "y = " << vec.y << '\n'
                          << "z = " << vec.z << '\n';
            }
        }
    /*
    let's do something more interesting and scale and rotate the container object from the
    previous chapter
    */
        namespace test
        {
            void scale_and_rotate()
            {
                glm::mat4 trans { glm::mat4(1.0f) };
                trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0, 0.0, 1.0));  // 90° rotation matrix around z axis
                trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));                        // scale by half
            }
        }
    /*
    the question is: how do we get the trasnformation matric to the shaders?

    GLSL has a mat4 type, so we'll adapt the vertex shader to accept a mat4 uniform variable
    and multiply the position vector by the matrix uniform:
    */
    #ifdef THIS_IS_GLSL_CODE
        #version 330 core
        layout (location = 0) in vec3 aPos;
        layout (location = 1) in vec2 aTexCoord;

        out vec2 TexCoord;

        uniform mat4 transform;

        void main()
        {
            gl_Position = transform * vec4(aPos, 1.0f);
            TexCoord = vec2(aTexCoord.x, aTexCoord.y)
        }
    #endif
    /*
        GLSL also has mat2 and mat3 types that allow for swizzling-like operations just like
        vectors.

    we still need to pass the transformation matrix to the shader though:
    */

        unsigned int transformLoc { glGetUniformLocation(ourShader.ID, "transform") };
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(trans));
        
    /*
    - We first query the location of the uniform variable and then send the matrix data to
      the shaders using glUniform with Matrix4fv as its postfix.
    - The first argument should be familiar by now which is the uniform's location.
    - The second argument tells OpenGL how many matrices we'd like to send, which is 1.
    - The third argument asks us if we want to transpose our matrix, that is to swap the
      columns and rows.
    - OpenGL developers often use an internal matrix layout called column-major ordering
      which is the default matrix layout in GLM so there is no need to transpose the matrices;
      we can keep it at GL_FALSE.
    - The last parameter is the actual matrix data, but GLM stores their matrices' data in
      a way that doesn't always match OpenGL's expectations so we first convert the data
      with GLM's built-in function value_ptr. 
    */

//=======================================================================================

int main()
{
    test::translating_a_vector();

    return 0;
}