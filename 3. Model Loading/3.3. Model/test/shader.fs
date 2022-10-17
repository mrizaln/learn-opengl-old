#version 330 core

// material
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

// point light source
struct PointLight
{
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
#define NR_POINT_LIGHTS 1                           // define the number of point lights we want to have in our scene
uniform PointLight pointLights[NR_POINT_LIGHTS];    // an array of point light

in vec2 TexCoords;
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 viewPos;

struct LightColor
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

LightColor mul(LightColor a, float b)
{
    return LightColor(
        a.ambient * b,
        a.diffuse * b,
        a.specular * b
    );
}

LightColor mul(float b, LightColor a)
{
    return mul(a, b);
}

vec3 toVec3(const in LightColor l){
    return (l.ambient + l.diffuse + l.specular);
}

//=======================================================================================
// function declarations

LightColor calcLight(vec3 lAmb, vec3 lDiff, vec3 lSpec, float diff, float spec);
float calcSpec(vec3 viewDir, vec3 reflectDir);
vec3 calcPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir);

//=======================================================================================

void main()
{
    // putting it all together
    //------------------------
    // properties
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result;

    // point lights
    for (int i = 0; i < NR_POINT_LIGHTS; ++i)
        result += calcPointLight(pointLights[i], norm, FragPos, viewDir);
    
    FragColor = vec4(result, 1.0);
    //------------------------
}


//=======================================================================================
// function definitions

float calcSpec(vec3 viewDir, vec3 reflectDir)
{
    float spec = 0;
    for (int i=0; i < NR_MATERIALS; ++i)
        spec += pow(max(dot(viewDir, reflectDir), 0.0), max(materials[i].shininess, 1.0));

    return spec;
}

LightColor calcLight(vec3 lAmb, vec3 lDiff, vec3 lSpec, float diff, float spec)
{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    for (int i = 0; i < NR_MATERIALS; ++i)
    {
        ambient += lAmb * texture(materials[i].texture_diffuse, TexCoords).xyz;
        diffuse += lDiff * diff * texture(materials[i].texture_diffuse, TexCoords).xyz;
        specular += lSpec * spec * texture(materials[i].texture_specular, TexCoords).xyz;
    }

    return LightColor(ambient, diffuse, specular);
}

// calculate point light
vec3 calcPointLight(PointLight light, vec3 normal, vec3 FragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - FragPos);        // direction from fragment to light source
    
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);               // -lightDir is incident light (ray from light source to fragment)
    float spec = calcSpec(viewDir, reflectDir);

    // attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

    // combine results
    LightColor lightColor = calcLight(light.ambient, light.diffuse, light.specular, diff, spec);
    lightColor = mul(lightColor,attenuation);

    return toVec3(lightColor);
}