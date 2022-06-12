#version 330 core

//======[ 2.2.forward_normal.3 ]======
in vec3 Normal;
//====================================
//======[ 2.2.fragment_pos.3 ]======
in vec3 FragPos;
//==================================

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;

//======[ 2.2.light_pos ]======
uniform vec3 lightPos;
//=============================
//======[ 2.2.view_pos ]======
uniform vec3 viewPos;
//============================

void main()
{
    //======[ 2.2.ambient ]======
    // ambient light
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    // vec3 result = ambient * objectColor;
    //===========================

    //======[ 2.2.calculate_diffuse ]======
    vec3 norm = normalize(Normal);                      // always normalize
    vec3 lightDir = normalize(lightPos - FragPos);      // ^^^

    float diff = max(dot(norm, lightDir), 0.0);         // values: 0 to 1
    vec3 diffuse = diff * lightColor;
    // result += diffuse * objectColor;
    //====================================

    //======[ 2.2.calculate_specular ]======
    float specularStrength = 0.5;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);         // -lightDir == incident light

    int shininess = 32;
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3 specular = specularStrength * spec * lightColor;
    // result += specular * objectColor;
    //======================================

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}