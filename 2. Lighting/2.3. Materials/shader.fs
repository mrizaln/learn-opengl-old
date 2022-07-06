#version 330 core

//======[ 2.3.material_struct ]======
struct Material
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

uniform Material material;
//===================================

//======[ 2.3.light_struct ]======
struct Light
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Light light;
//================================

in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    //======[ 2.3.material_fragment_calculation ]======
    // ambient
    vec3 ambient = vec3(0.1) * light.ambient * material.ambient;

    // diffuse
    vec3 norm = normalize(Normal);                      // always normalize
    vec3 lightDir = normalize(lightPos - FragPos);      // ^^^

    float diff = max(dot(norm, lightDir), 0.0);         // values: 0 to 1
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);         // -lightDir == incident light

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * material.specular);

    // result
    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}