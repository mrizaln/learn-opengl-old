#ifndef LIGHT_H
#define LIGHT_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



class Light
{
public:
    // color    
    glm::vec3 ambient{};
    glm::vec3 diffuse{};
    glm::vec3 specular{};

    Light(glm::vec3 amb, glm::vec3 diff, glm::vec3 spec)
        : ambient{ amb }
        , diffuse{ diff }
        , specular{ spec }
    {
    }
};


// directional light
class DirectionalLight : public Light
{
public:
    glm::vec3 direction{ 0.0f, -1.0f, 0.0f };

    DirectionalLight(
        glm::vec3 dir,
        glm::vec3 amb, glm::vec3 diff, glm::vec3 spec
    )
        : direction{ dir }
        , Light{ amb, diff, spec }
    {
    }
};


// point light
class PointLight : public Light
{
public:
    glm::vec3 position{};
    
    // attenuation
    float constant{ 1.0f };
    float linear{ 1.0f };
    float quadratic{ 1.0f };

    PointLight(
        glm::vec3 pos,
        glm::vec3 amb, glm::vec3 diff, glm::vec3 spec,
        float con, float lin, float quad
    )
        : position{ pos }
        , Light{ amb, diff, spec }
        , constant{ con }
        , linear{ lin }
        , quadratic{ quad }
        {
        }
};


// spot light
class SpotLight : public PointLight
{
public:
    glm::vec3 direction{ 0.0f, 0.0f, -1.0f };

    float cutOff{ 12.5f };          // in degrees
    float outerCutOff{ 15.5f };     // in degrees

    SpotLight(
        glm::vec3 pos, glm::vec3 dir,
        glm::vec3 amb, glm::vec3 diff, glm::vec3 spec,
        float con, float lin, float quad,
        float cutoff, float outercutoff
    )
        : PointLight{ pos, amb, diff, spec, con, lin, quad }
        , direction{ dir }
        , cutOff{ cutoff }
        , outerCutOff{ outercutoff }
    {
    }
};


#endif