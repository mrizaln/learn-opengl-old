#version 330 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    sampler2D emission;
    float shininess;
};
uniform Material material;

// light struct
struct Light
{
    vec4 pos_dir;           // light position or light direction

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
uniform Light light;

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 viewPos;

void main()
{
    // material fragment calculation
    //======
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    // diffuse
    //--------
    vec3 norm = normalize(Normal);
    vec3 lightDir;

    // check whether the light is directional light or point light
    if (light.pos_dir.w == 0.0)
        lightDir = normalize(-light.pos_dir.xyz);               // directional light
    else if (light.pos_dir.w == 1.0)
        lightDir = normalize(light.pos_dir.xyz - FragPos);      // positional light (point light)

    float diff = max(dot(norm, lightDir), 0.0);         // values: 0 to 1
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    //-------

    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);         // -lightDir == incident light

    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    // emission
    vec3 emission = vec3(texture(material.emission, TexCoords));

    // result
    vec3 result = ambient + diffuse + specular + emission;
    //======


    // attenuation
    float attenuation = 1.0;
    if (light.pos_dir.w == 1.0)
    {
        float distance = length(light.pos_dir.xyz - FragPos);
        attenuation = 1.0 / (light.constant + light.linear*distance + light.quadratic*distance*distance);
    }
    FragColor = vec4(result * attenuation, 1.0);
}