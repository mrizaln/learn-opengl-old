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
    // spotlight
    //-----
    vec3 position;
    vec3 direction;
    float cutOff;           // is a cosine of cutOff angle
    float outerCutOff;      // is a cosine of outerCutOff angle
    //-----

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
    //------
    // ambient
    vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCoords));

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);                     // values: 0 to 1
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCoords));
    
    // specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);                     // -lightDir == incident light
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCoords));

    // emission
    vec3 emission = vec3(texture(material.emission, TexCoords));

    // smooth edges
    float theta = dot(lightDir, normalize(-light.direction));       // negated because we want the vectors to point towards the light source
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff)/epsilon, 0.0, 1.0);

    // attenuation
    float attenuation = 1.0;
    float distance = length(light.position - FragPos);
    attenuation = 1.0 / (light.constant + light.linear*distance + light.quadratic*distance*distance);

    // result
    vec3 result = ambient + intensity * attenuation * (diffuse + specular + emission);
    //------
    
    FragColor = vec4(result, 1.0);
}