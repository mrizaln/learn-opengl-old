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


/*-----------------
    Walk Around
-----------------*/

/*
  - we can look around by updating the view matrix by changing the camera position.
*/


/*---------------------------------------------------------------------------------------
                        ============[ Look Around ]============
---------------------------------------------------------------------------------------*/

/*
  - to look around the scene we have to change the cameraFront vector based on the input
    of the mouse.
*/


    //------------------------
    //    [ euler angles ]
    //------------------------

    /*
      - euler angles are 3 values that can represent any rotation in 3D.
      - there are 3 euler angles:
        > pitch:    look up/down        x-axis rotation
        > yaw:      look right/left     y-axis rotation (up direction)
        > roll:     how much we roll    z-axis rotation (or y if up if z)

      - for our camera system, we only care about the yaw and pitch values.
    */
        namespace camera
        {
            float pitch {};             // altitude (angular) (if in physics: pitch = 2pi - theta)
            float yaw { -90.0f };       // azimuth (if in physics: yaw = phi)
            glm::vec3 direction(1.0f);
            auto cameraFront { glm::normalize(direction) };
            
            void updateDirection()
            {
                direction.y = sin(glm::radians(pitch));
                direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                cameraFront = glm::normalize(direction);
            }
        }


    //-----------------------
    //    [ mouse input ]
    //-----------------------

    /*
      - the yaw and pitch values are obtained from mouse (or controller/joystick) movement
        where horizontal mouse-movement affects the yaw and vertical mouse-movement affects
        the pitch.
      - the idea is to store the last frame's mouse positions and calculate in the current
        frame how much the mouse values changed.
      - the higher the horizontal or vertical difference, the more we update the pitch or
        yaw value.

      - first, we will tell GLFW that is should hide the cursor and capture it.
    */
        GLFWwindow* window { glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL) };
        
        void setToCaptureMouseInput()
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    /*
      - after this call, whenever we move the mouse it won't be visible and it should not
        leave the window

      - to calculate the pitch and yaw values, we need to tell GLFW to listen to mouse-movement
        events.
    */
        void mouse_callback(GLFWwindow* window, double xpos, double ypos);
        
        void setCursorPosCallback()
        {
            glfwSetCursorPosCallback(window, mouse_callback);
        }

    /*
      - when handling mouse input for a fly style camera, there are several steps we have
        to take before we're able to fully calculate the camera's direction vector:
            1. calculate the mouse's offset since the last frame.
            2. add the offset values to the camera's yaw and pitch values.
            3. add some constraints to the minimum/maximum pitch values.
            4. calculate the direction vector.
    */
        namespace mouse
        {
            float lastX { 400 };    // center of screen
            float lastY { 300 };    // center of screen
            const float sensitivity { 0.1f };
        }

        void mouse_callback(GLFWwindow* window, double xPos, double yPos)
        {
            // 1. calculate offset
            float xOffset { xPos - mouse::lastX };
            float yOffset { yPos - mouse::lastY };

            mouse::lastX = xPos;
            mouse::lastY = yPos;

            xOffset *= mouse::sensitivity;
            yOffset *= mouse::sensitivity;

            // 2. add last offset
            camera::yaw   += xOffset;
            camera::pitch += yOffset;

            // 3. constraints
            if (camera::pitch >  89.0f) camera::pitch =  89.0f;
            if (camera::pitch < -89.0f) camera::pitch = -89.0f;

            // 4. calculate the direction vector
            camera::updateDirection();
        }
    

    //----------------
    //    [ zoom ]
    //----------------

    /*
      - we can change the FOV of projection matrix to get an illusion of zooming in.
      - we control the zoom using the mouse's wheel.
      - similar to mouse movement, we have a callback function for mouse scrolling.
    */
        namespace camera
        {
            float fov { 45.0f };
        }

        void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
        {
            camera::fov -= static_cast<float>(yOffset);
            if (camera::fov <  1.0f) camera::fov =  1.0f;
            if (camera::fov > 45.0f) camera::fov = 45.0f;
        }
    /*
      - when scrolling, the yOffset value tells us the amount we scrolled vertically.
      - after this, we need to update the projection matrix

      - don't forget to register the scroll callback function
    */
        void setScrollCallback()
        {
            glfwSetScrollCallback(window, scroll_callback);
        }
