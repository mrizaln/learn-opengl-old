#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D texture0;     // container
uniform sampler2D texture1;     // happy face

void main()
{
    // to flip the face looks to other direction, we can reverse the x coord of the TexCoord
    // something like:     x  ==>  1-x

    vec2 TexCoord_1 = vec2((1.0 - TexCoord.x), TexCoord.y);

    FragColor = mix(texture(texture0, TexCoord),
                    texture(texture1, TexCoord_1),
                    0.2);
}