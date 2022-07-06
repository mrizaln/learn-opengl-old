#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D texture0;

vec4 sigmoid(vec4, float);
vec4 normPow(vec4, int);

void main()
{
    vec4 textureFrag = texture(texture0, TexCoord);
    // textureFrag = sigmoid(textureFrag, 10.0);
    textureFrag = normPow(textureFrag, 2);
    FragColor = textureFrag;
}

// returns 1/( exp(-a(x-0.5)) + 1)
vec4 sigmoid(vec4 avec4, float a)
{
    avec4.x = 1/(exp(-a*(avec4.x-0.5))+1);
    avec4.y = 1/(exp(-a*(avec4.y-0.5))+1);
    avec4.z = 1/(exp(-a*(avec4.z-0.5))+1);

    return avec4;
}

// return 1 - pow(-1, factor)*pow(avec4 - 1, factor)
vec4 normPow(vec4 avec4, int factor)
{
    avec4.x = 1.0 - pow(-1.0, factor)*pow(avec4.x - 1.0, factor);
    avec4.y = 1.0 - pow(-1.0, factor)*pow(avec4.y - 1.0, factor);
    avec4.z = 1.0 - pow(-1.0, factor)*pow(avec4.z - 1.0, factor);

    return avec4;
}
