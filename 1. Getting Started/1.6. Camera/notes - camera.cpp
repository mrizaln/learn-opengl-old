// glad
#include <glad/glad.h>

// GLFW
#include <GLFW/glfw3.h>

// GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


/*---------------------------------------------------------------------------------------
                          ============[ Camera ]============
---------------------------------------------------------------------------------------*/

/*
    OpenGL by itself is not familiar with the concept of a camera, but we can try to simulate
    one by moving all objects in the scene in the reverse direction, giving the illusion that
    [we] are moving.
*/



    //-----------------------------
    //    [ camera/view space ]
    //-----------------------------

    /*
      - when we're talking about camera/view space we're talking about all the vertex
        coordinates as seen from the camerea's perspective as the origin of the scene.
      - to define a camera we need is position in world space, the direction it's looking
        at, a vector pointing to the right and a vector pointing upwards from the camera.
      - it creates a coordinate system with 3 perpendicular unit axes with the camera's
        position at the origin
    */



        // [ 1. camera position ]

        /*
          - the camera position is a vector in world space that points to the camera's
            position.
        */
            namespace camera
            {
                glm::vec3 cameraPos{ 0.0f, 0.0f, 3.0f };
            }
        


        // [ 2. camera direction ]      (camera z-axis)

        /*
          - the next vector required is the camera's direction e.g. at what direction it
            is pointing at.
          - for now we let the camera point to the origin of our scene (0, 0, 0).
        */
            namespace camera
            {
                glm::vec3 cameraTarget{ 0.0f, 0.0f, 0.0f };        // target the origin
                auto cameraDirection{ glm::normalize(cameraPos - cameraTarget) };
            }
        // the name direction vector is not the best chosen name, since it is acrually
        // pointing in the reverse direction of what it is targeting.
        


        // [ 3. right axis ]            (camera x-axis)

        /*
          - the next vector is a right vector that represents the positive x-axis of the
            camera space.

          - to get the right vector we use a little trick by first specifying an up vector
            that points upwards (in world space).
        */
            namespace camera
            {
                glm::vec3 up{ 0.0f, 1.0f, 0.0f };
                auto cameraRight{ glm::normalize(glm::cross(up, cameraDirection)) };
            }



        // [ 4. up axis ]               (camera y-axis)

        /*
          - now that we have both the x-axis and z-axis, retrueving the vector that points
            to the camera's positive y-axis is relatively easy: cross product of z and x.
        */
            namespace camera
            {
                glm::vec3 cameraUp{ glm::normalize(glm::cross(cameraDirection, cameraRight)) };
            }
        
    

    //------------------
    //    [ LookAt ]
    //------------------

    /*
      - a great thing about matrices is that if you define a coordinate space using 3
        perpendicular (or non-linear) axes you can create a matrix with those 3 axes plus
        a translation vector and you can transform any vector to that coordinate space by
        multiplying it with this matrix.
    
      - this is exactly what the LookAt matrix does.
    
                 ⎡R₁ R₂ R₃ 0⎤   ⎡1 0 0 -P₁⎤         R = camera's right     vector
        LookAt = ⎢U₁ U₂ U₃ 0⎥ ∙ ⎢0 1 0 -P₂⎥         U = camera's up        vector
                 ⎢D₁ D₂ D₃ 0⎥   ⎢0 0 1 -P₃⎥         D = camera's direction vector
                 ⎣0  0  0  1⎦   ⎣0 0 0  1 ⎦         P = camera's position  vector
    
      - note that the rotation (left matrix) and translation (right matrix) parts are inverted
        since we want to rotate and translate the world in the opposite direction of where
        we want the camera to move.

      - using this LookAt matrix as our view matrix effectively transforms all the world
        coordinates to the view space we just defined.

      - luckily, GLM already does all this work for us.
      - we only have to specify: a camera position, a target position, and a vector that
        represents the up vector in world space        
    */
        namespace camera
        {
            glm::mat4 view{ glm::lookAt(cameraPos, cameraTarget, up) };
        }