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
                      ============[ Basic Lighting ]============
---------------------------------------------------------------------------------------*/

/*
  - lighting in OpenGL is based on approximations of reality using simplified models that
    are muh easier to process and look relatively similar.
  - the lighting models are based on the physics of light as we understand it.
  - one of those models is called the [Phong lighting model].

  - the major building blocks of the Phong lighting model consists of 3 components: ambient,
    diffuse, and specular lighting.
*/




/*---------------------------------------------------------------------------------------
                    ============[ Ambient Lighting ]============
---------------------------------------------------------------------------------------*/

/*
  - light bounce and scatter in many directions, reaching spots that aren't immediately
    visible; light can thus reflect on other surfaces and have an indirect impact on the
    lighting of an object.
  - algorithms that take this into account are called [global illumination algorithm].
  
  - a very simplistic model of global illumintion is named [ambient lighting].

  - adding ambient light to the scene is really easy.
  - we take the light's color, multiply it with a small constant ambient factor, multiply
    this with the object's color, and use that as the fragment's color in the cube object's
    shader

        [ file: shader.fs; section: 2.2.ambient ]
*/




/*---------------------------------------------------------------------------------------
                    ============[ Diffuse Lighting ]============
---------------------------------------------------------------------------------------*/

/*
  - diffuse lighting gives the object more brightness the closer its fragments are aligned
    to the rays from a light source.

        light
            \    ^
             \   | surface normal (n⃗)
              \  |
               \θ|
                \|
    --------------------------
    |                        |
    |                        |

  - we need to measure at what angle the light ray touches the fragment.
  - if the light ray is perpendicular to the object's surface, the light has the greatest
    impact.
  - to measure the angle between the light ray and the fragment, we use something called
    a [normal vector], that is a vector perpendicular to the fragment's surface.
  - the angle between the two vectors can then easily be calculated with the dot product.

  - so what do we need to calculate diffuse lighting:
      > normal vector
      > the directed light ray
*/



    //----------------------
    //    normal vectors
    //----------------------

    /*
      - a normal vector is a unit vector that is perpendicular to the surface of a vertex
      - since a vertex by itself has no surface we retrieve a normal vector by using its
        surrounding vertices to figure out the surface of the vertex.
      - we can use a little trick to calculate the normal vectors for all the cube;s vertices
        by using the cross product, but since a 3D cube is not complicated, we can simply
        manually add then to the vertex data.

            [ file: "basic lighting.cpp"; section: 2.2.vertex_normal ]

      - since we added extra data to the vertex array, we should update the cube's vertex
        shader

            [ file: shader.vs; section: 2.2.vertex_normal ]

      - also update the vertex attribute pointer

            [ file: "basic lighting.cpp"; section: 2.2.vertex_normal_attrib ]
    
      - all the lighting calculatios are done in the fragment shader so we need to forward
        the normal vectors from the vertex shader to the fragment shader.

            [ file: shader.vs and shader.fs; section: 2.2.forward_normal ]
    */
    


    //-------------------------------------
    //    calculating the diffuse color
    //-------------------------------------

    /*
      - we now have the normal vector for each vertex, but we still need the light's position
        vector and the fragment's position vector.
      - since the light's position is a single static variable, we can declare it as a uniform
        in the fragment shader

            [ file: shader.fs; section: 2.2.light_pos ]

      - and then update the uniform in the render loop (or outside if it is not changed).

            [ file: "basic lighting.cpp"; section: 2.2.light_pos_uniform ]

      - then the last thing we need is the actual fragment's position.
      - we're going to do all the lighting calculations in world space so we want a vertex
        position that is in world space first.
      - we can accomplist this by multiplying the vertex position attribute with the model
        matrix only to transform it to world space coordinates.
      - and lastly we can add the corresponsding input variable to the fragment shader.

            [ file: shader.vs and shader.fs; section: 2.2.fragment_pos ]

      - now the calculation part.
      - the first thing we need to calculate is the direction vector between the light source
        and the fragment's position.
      - next, we need to calculate the diffuse impact of the light on the current fragment
        by taking the dot product between the [norm] and [lightDir] vectors.
      - the resulting value is the multiplied with the light's color to get the diffuse
        component.
      - last add both the ambient and diffuse color to the resultant fragment color.

            [ file: shader.fs; section: 2.2.calculate_diffuse ]
    */



    //----------------------
    //    one last thing
    //----------------------

    /*
      - in the previous section we passed the normal vector directly from the vertex shader
        to the fragment shader.
      - however calculations in the fragment shader are all done in world space, so shouldn't
        we transform the normal vectors to world space coordinates as well?
      - basically yes, but it's not as simple as simply multiplying it with a model matrix.

      - first of all, normal vectors are only direction vectors and do not represent a specific
        position in space
      - so it means, translation shouldn't have any effect on the normal vectors.

      - second, if the model matrix would perform a non-uniform scale, the vertices would
        be changed in such a way that the normal vector is not perpendicular to the surface
        anymore.

      - the trick of fixing this behavior is to use a different model matrix specifically
        tailored for normal vectors.
      - this matrix is called the normal matrix and uses a few linear algebraic operations
        to remove the effect of wronglyscaling the normal vectors.

            [ for more information, read: http://www.lighthouse3d.com/tutorials/glsl-12-tutorial/the-normal-matrix/ ]

      - the normal matrix is defined as: "the transpose of the inverse of the upper 3x3 part
        of the model matrix" (upper 3x3: scaling part only).
      - note that most resources define the normal matrix derived from the model-view matrix
        but since we're working in world space, we will derived it from the model matrix.

      - on vertex shader we can generate the normal matrix by using the inverse and transpose
        functions in the vertex shader that work on any matrix type.
      - note that we cast the matrix to a 3x3 matrix to ensure it loses its translation
        properties.

            [ file: shader.vs; section: 2.2.normal_matrix ]

            // inversing matrices is costly operation for shader, so wherever possible try
            // to avoid inverse operations since they have to be done on each vertex on your
            // screen. You'll likely want to calculate the normal matrix on the CPU and send
            // it to the shaders via a uniform before drawing.
    */




/*---------------------------------------------------------------------------------------
                      ============[ Specular Lighting ]============
---------------------------------------------------------------------------------------*/

/*
  - similar to diffuse lighting, specular lighting is based on the object's direction vector
    and the object's normal vectors, but this time, it is also based on the view direction.
  - specular lighting is based on the reflective properties of surfaces.

  - we calculate a reflection vector by reflecting the light direction around the normal
    vector.
  - then we calculate the angular distance between this reflection vector and the view direction.
  - the resulting effect is that we see abit of a highlight when we're looking at the light's
    direction reflected via the surface.

  - the view vector is the one extra variable we need for specular lighting which we can
    calculate using the viewe's world space position and the fragment's position.
  - then we calculate the specular's intensity, multiply this with the light color and
    add this to the ambient and diffuse components.

  - to get the world space coordinates of the viewer we simply take the position vector
    of the camera object.
  - so let's add another uniform to the fragment shader.

        [ file: shader.fs and "basic lighting.cpp"; section: 2.2.view_pos ]

  - now we can calculate the specular intensity
  - first we define a specular intensity value to give the specular highlight a medium-bright
    color.
  - next, we calculate the view direction vector and the corresponding reflect vector along
    the normal axis.
  - then what's left to do is to actually calculate the specular component.
  - this is accomplised with the following formula
        spec = pow(max(dor(viewDir, reflectDir), 0.0), 32);                 // 32 is the shininess value
  - the only thing left is to add it to the ambient and diffuse component.

        [ file: shader.fs; section: 2.2.calculate_specular ]
*/


// [ we now calculated all the lighting components of the Phong lighting model.]