#version 330 core

layout (location = 0) in vec3 aPos;

//======[ 2.2.vertex_normal ]======
layout (location = 1) in vec3 aNormal;
//=================================


//======[ 2.2.forward_normal.1 ]======
out vec3 Normal;
//====================================
//======[ 2.2.fragment_pos.1 ]======
out vec3 FragPos;
//==================================


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    //======[ 2.2.forward_normal.2 ]======
    // Normal = aNormal;        
    //====================================

    //======[ 2.2.normal_matrix ]======
    Normal = mat3(transpose(inverse(model))) * aNormal;
    //=================================

    //======[ 2.2.fragment_pos.2 ]======
    FragPos = vec3(model * vec4(aPos, 1));
    //==================================
}