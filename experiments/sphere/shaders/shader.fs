#version 330 core

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoord;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
uniform float lightStrength;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform sampler2D texture0;

void main()
{
    // ambient light
    //--------------
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    //--------
    vec3 norm = normalize(Normal);                      // always normalize
    vec3 lightDir = normalize(lightPos - FragPos);      // ^^^

    float diff = max(dot(norm, lightDir), 0.0);         // values: 0 to 1
    vec3 diffuse = diff * lightColor;
    // result += diffuse * objectColor;

    // specular
    //---------
    float specularStrength = 0.7;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);         // -lightDir == incident light

    int shininess = 64;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;
    // result += specular * objectColor;

    // texture
    //--------
    vec4 textureFrag = texture(texture0, TexCoord);

    // result
    //-------
    // vec3 result = (ambient + diffuse + specular) * objectColor;
    // FragColor = mix(textureFrag, vec4(result,1.0), 0.5);
    vec4 TextureFrag = (vec4(ambient, 1.0) + vec4(diffuse, 1.0) + vec4(specular, 1.0)) * textureFrag;
    FragColor = lightStrength * TextureFrag;
}