#version 330 core

float normalize(float x)
{
    return (x + 1) / 2.0;
}

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;
uniform float xOffset;
uniform float yOffset;

void main()
{
    gl_Position = vec4(aPos.x + xOffset, aPos.y + yOffset, aPos.z, 1.0f);
    // gl_Position = vec4(aPos.x + aPosRotate.x,
    //                    aPos.y + aPosRotate.y,
    //                    aPos.z + aPosRotate.z,
    //                    1.0);
    // gl_Position = vec4(aPos.x + aPos.x,
    //                    aPos.y + aPos.y,
    //                    aPos.z + aPos.z,
    //                    1.0);
    ourColor = vec3(normalize(gl_Position.x), normalize(gl_Position.y), normalize(gl_Position.z));
}