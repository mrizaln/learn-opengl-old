#version 330 core

// 2.4_update_material_struct
//------
struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};
uniform Material material;
//------

struct Light
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
uniform Light light;

//================================

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{
    //======[ 2.3.material_fragment_calculation ]======
    // ambient
    // 2.4_update_ambient_and_diffuse
    //------
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));
    //------

    // diffuse
    vec3 norm = normalize(Normal);                      // always normalize
    vec3 lightDir = normalize(lightPos - FragPos);      // ^^^

    float diff = max(dot(norm, lightDir), 0.0);         // values: 0 to 1
    // 2.4_update_ambient_and_diffuse
    //------
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    //------

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);         // -lightDir == incident light

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    // emission
    vec3 emission = vec3(texture(material.emission, TexCoords));

    // result
    vec3 result = ambient + diffuse + specular + emission;
    FragColor = vec4(result, 1.0);
}