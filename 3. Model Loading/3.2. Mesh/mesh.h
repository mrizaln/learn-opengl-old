#include <vector>
#include <string>

#include <glm/glm.hpp>
#include <glad/glad.h>

#include <shader_header/shader.h>

struct Vertex
{
    glm::vec3 m_position;
    glm::vec3 m_normal;
    glm::vec2 m_texCoords;
};

struct Texture
{
    unsigned int m_id;
    std::string m_type;       // e.g. diffuse or specular texture
};

class Mesh
{
public:
    // mesh data
    std::vector<Vertex>       m_vertices{};
    std::vector<unsigned int> m_indices{};
    std::vector<Texture>      m_textures{};

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
            we assume that each diffuse texture is named texture_diffuseN and each specular
            texture should be named texture_specularN where N is any number ranging from 1
            to the max number of texture samplers allowed.
        */

        unsigned int diffuseNr{ 1 };
        unsigned int specularNr{ 1 };

        for (unsigned int i{ 0 }; i < textures.size(); ++i)
        {
            glActiveTexture(GL_TEXTURE0+i);

            std::string number{};
            std::string name{ m_textures[i].m_type };

            if (name == "texture_diffuse")
                number = std::to_string(diffuseNr++);
            else if (name == "texture_specular")
                number = std::to_string(specularNr++);

            shader.setInt(("material." + name + number).c_str(), i);
            glBindTexture(GL_TEXTURE_2D, m_textures[i].m_id);
        }
        glActiveTexture(GL_TEXTURE0);

        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
    }

private:
    // render data
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    void setupMesh()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, m_vertices.size()*sizeof(Vertex), &m_vertices.front(), GL_STATIC_DRAW);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size()*sizeof(unsigned int), &m_indices.front(), GL_STATIC_DRAW);

        // vertex positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void*>(0));
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<void*>(offsetof(Vertex, m_normal)));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), static_cast<int*>(offsetof(Vertex, m_texCoords)));

        glBindVertexArray(0);
    }
};