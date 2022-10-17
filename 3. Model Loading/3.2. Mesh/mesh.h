#ifndef MESH_H
#define MESH_H

#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <shader_header/shader.h>


#define MAX_BONE_INFLUENCE 4


struct Vertex
{
    glm::vec3 m_position{};
    glm::vec3 m_normal{};
    glm::vec2 m_texCoords{};
    glm::vec3 m_tangent{};
    glm::vec3 m_bitangent{};
        // bone indexes which will influence this vertex
        int m_boneIDs[MAX_BONE_INFLUENCE]{};
        // weights from each bone
        float m_weights[MAX_BONE_INFLUENCE]{};
};

struct Texture
{
    unsigned int m_id{};
    std::string m_type{};       // e.g. diffuse or specular texture
    std::string m_path{};
};

class Mesh
{
public:
    // mesh data
    std::vector<Vertex>       m_vertices{};
    std::vector<unsigned int> m_indices{};
    std::vector<Texture>      m_textures{};
    unsigned int VAO{};

    Mesh(
        std::vector<Vertex> vertices,
        std::vector<unsigned int> indices,
        std::vector<Texture> textures
    )
        : m_vertices{ vertices }
        , m_indices{ indices }
        , m_textures{ textures }
    {
        setupMesh();
    }

    void draw(Shader& shader)
    {
        /*
            assimp allow up to 8 texture

            we assume that each diffuse texture is named texture_diffuseN and each specular
            texture should be named texture_specularN where N is any number ranging from 1
            to the max number of texture samplers allowed.

            naming candidate: texture_diffuseN
                              material.texture_diffuseN
                              materials[N].texture_diffuse
        */

        unsigned int diffuseNr { 0 };
        unsigned int specularNr{ 0 };
        unsigned int normalNr  { 0 };
        unsigned int heightNr  { 0 };

        for (unsigned int i{ 0 }; i < m_textures.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0+i);

            std::string number{};
            std::string name{ m_textures[i].m_type };

            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);
            else if (name == "texture_normal")
                number = std::to_string(normalNr++);
            else if (name == "texture_height")
                number = std::to_string(heightNr++);

            // shader.setInt(("material." + name + number).c_str(), i);            // material.texture_diffuseN
            shader.setInt(("materials[" + number + "]." + name).c_str(), i);     // materials[N].texture_diffuse
            // std::cout << "materials[" + number + "]." + name << '\n';
            glBindTexture(GL_TEXTURE_2D, m_textures[i].m_id);
        }

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        glActiveTexture(GL_TEXTURE0);       // set to default
    }

private:
    // render data
    unsigned int VBO{};
    unsigned int EBO{};

    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // a great thing about structs is that their memory layout is sequential, so we can do this:
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(Vertex), &m_vertices.front(), GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size()*sizeof(unsigned int), &m_indices.front(), GL_STATIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(0));
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_normal)));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_texCoords)));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_tangent)));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_bitangent)));
            // ids
            glEnableVertexAttribArray(5);
            glVertexAttribPointer(5, MAX_BONE_INFLUENCE, GL_INT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_boneIDs)));
            // weights
            glEnableVertexAttribArray(6);
            glVertexAttribPointer(6, MAX_BONE_INFLUENCE, GL_INT, GL_FALSE, sizeof(Vertex), (void*)(offsetof(Vertex, m_weights)));

        glBindVertexArray(0);
    }
};

#endif