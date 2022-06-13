#version 330 core

out vec4 FragColor;

uniform vec3 lightColor;

void main()
{
    vec3 LightColor = vec3(
        1.0 - pow((lightColor.r-1.0), 4),
        1.0 - pow((lightColor.g-1.0), 4),
        1.0 - pow((lightColor.b-1.0), 4)
        );
    FragColor = vec4(LightColor, 1.0);
}