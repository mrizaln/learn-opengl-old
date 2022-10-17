#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

struct Material
{
    sampler2D texture_diffuse;
    sampler2D texture_specular;
    sampler2D texture_normal;
    sampler2D texture_height;
    float shininess;
};
#define NR_MATERIALS 1
uniform Material materials[NR_MATERIALS];

void main()
{    
    FragColor = texture(materials[0].texture_diffuse, TexCoords);
}
