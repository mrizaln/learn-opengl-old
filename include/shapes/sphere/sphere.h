#ifndef SPHERE_H
#define SPHERE_H



#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

//==========================
//  Create sphere
//==========================


// sphere constants
namespace sphere_constant
{
    constexpr int min_sector_count{ 3 };
    constexpr int min_stack_count{ 2 };
}

class Sphere
{
    // member variables
    //-----------------
    // vertices data
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<unsigned int> indices;
    std::vector<unsigned int> lineIndices;

    // interleaved vertices data
    std::vector<float> interleavedVertices;
    int interleavedVerticesStride;

    // buffers
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    // primary
    float radius;
    int sectorCount;        // longitude
    int stackCount;         // latitude

    // config
    bool swapYZ;            // in case you use Y as up, set this to true


public:
    // ctor: this code assume you uze z-axis as up direction, set swapYZ to true if you set y-axis as up direction
    Sphere(float radius, int sectors, int stacks, bool swapYZ = false) : interleavedVerticesStride{ 8*sizeof(float) }
    {
        this->radius = radius;
        this->sectorCount = glm::max(sectors, sphere_constant::min_sector_count);
        this->stackCount = glm::max(stacks, sphere_constant::min_stack_count);

        this->swapYZ = swapYZ;

        buildVertices();
        setBuffers();
    }

    ~Sphere()
    {
        clearArrays();
        // deleteBuffers();     // for some reason it results in segmentation fault ???
                                // for now i'll just commment it out
    }

    void draw() const
    {
        // bind buffer
        glBindVertexArray(VAO);
        
        // draw
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        //unbind
        glBindVertexArray(0);
    }

    void deleteBuffers()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

private:
    void buildVertices()
    {
        // vertices
        //----------------------------------
        float x{},  y{},  z{},  xy{};                   // vertex position
        float nx{}, ny{}, nz{};                         // vertex normal
        float s{},  t{};                                // vertex texCoord

        float lengthInv{ 1.0f /radius };

        float sectorStep{ static_cast<float>(2 * M_PI / sectorCount) };     // longitude step
        float sectorAngle{};                            // longitude angle
        float stackStep{ static_cast<float>(M_PI / stackCount) };           // latitude step
        float stackAngle{};                             // latitude angle

        // stack:
        for (int i{ 0 }; i <= stackCount; ++i)
        {
            stackAngle = M_PI / 2 - i * stackStep;      // starting from pi/2 to -pi/2
            xy = radius * cosf(stackAngle);         // xy plane
            z  = radius * sinf(stackAngle);         // z axis

            // sector:
            // add (sectorCount+1) vertices per stack
            // the first and last vertices have same position and normal, but different tex coords
            for (int j{ 0 }; j <= sectorCount; ++j)
            {
                sectorAngle = j * sectorStep;           // starting from 0 to 2pi

                // vertex position (x, y, z)
                x = xy * cosf(sectorAngle);
                y = xy * sinf(sectorAngle);
                
                vertices.push_back(x);
                vertices.push_back(y);
                vertices.push_back(z);

                // normalized vertex normal
                nx = x * lengthInv;
                ny = y * lengthInv;
                nz = z * lengthInv;

                normals.push_back(nx);
                normals.push_back(ny);
                normals.push_back(nz);

                // vertex texture coord, range: [0, 1]
                s = static_cast<float>(j) / sectorCount;
                t = static_cast<float>(i) / stackCount;

                texCoords.push_back(s);
                texCoords.push_back(t);
            }
        }

        // triangulation
        //--------------
        /*
            a snippet of section on a sphere (a sector on a stack)
                k1----k1+1
                |    / |            2 triangles:
                |   /  |                k1   -> k2 -> k1+1
                |  /   |                k1+1 -> k2 -> k2+1
                | /    |
                k2----k2+1
        */
        // generate CCW index list of sphere triangle

        unsigned int k1{}, k2{};

        // stack
        for (int i{ 0 }; i < stackCount; ++i)
        {
            k1 = i * (sectorCount + 1);         // beginning of current stack
            k2 = k1 + sectorCount + 1;          // beginning of next stack

            // sector
            for (int j{ 0 }; j < sectorCount; ++j, ++k1, ++k2)
            {
                // 2 triangles per sector excluding first and last stacks
                if (i != 0)
                {   // k1 -> k2 -> k1+1
                    indices.push_back(k1);
                    indices.push_back(k2);
                    indices.push_back(k1+1);
                }
                if (i != (stackCount-1))
                {   // k1+1 -> k2 -> k2+1
                    indices.push_back(k1+1);
                    indices.push_back(k2);
                    indices.push_back(k2+1);
                }

                // store indices for lines
                // vertical lines for all stacks, k -> k2
                lineIndices.push_back(k1);
                lineIndices.push_back(k2);
                // horizontal lines except 1st stack, k1 -> k1+1
                if(i != 0)
                {
                    lineIndices.push_back(k1);
                    lineIndices.push_back(k1+1);
                }
            }
        }

        generateInterleavedVertices();
    }

    void generateInterleavedVertices()
    {
        size_t i{}, j{};
        size_t count{ vertices.size() };
    
        if (!swapYZ)    // z-axis as up
        {
            for (i = 0, j = 0; i < count; i += 3, j += 2)
            {
                interleavedVertices.push_back(vertices[i]);         // x
                interleavedVertices.push_back(vertices[i+1]);       // y
                interleavedVertices.push_back(vertices[i+2]);       // z (up)

                interleavedVertices.push_back(normals[i]);          // nx
                interleavedVertices.push_back(normals[i+1]);        // ny
                interleavedVertices.push_back(normals[i+2]);        // nz

                interleavedVertices.push_back(texCoords[j]);
                interleavedVertices.push_back(texCoords[j+1]);
            }
        }
        else            // y axis as up
        {
            int a{ 0 };
            int b{ 0 };
            for (i = 0, j = 0; i < count; i += 3, j += 2)
            {
                interleavedVertices.push_back( vertices[i]);         // x -> x
                interleavedVertices.push_back( vertices[i+2]);       // y -> z
                interleavedVertices.push_back(-vertices[i+1]);       // z -> -y

                interleavedVertices.push_back( normals[i]);          // nx -> nx
                interleavedVertices.push_back( normals[i+2]);        // ny -> nz
                interleavedVertices.push_back(-normals[i+1]);        // nz -> -ny

                // int s = a * 2 * (sectorCount) - b;
                // int t = a * 2 * (sectorCount) - b - 1;
                interleavedVertices.push_back(texCoords[j]);
                interleavedVertices.push_back(texCoords[j+1]);
                // if (b == 2 * (sectorCount))
                //     ++a;
                // b -= 2;
            }
        }
    }

    void clearArrays()
    {
        vertices.clear();
        normals.clear();
        texCoords.clear();
        indices.clear();
        lineIndices.clear();
    }

    void setBuffers()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // bind
        //-----
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, interleavedVertices.size()*sizeof(interleavedVertices[0]), &interleavedVertices.front(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(indices[0]), &indices.front(), GL_STATIC_DRAW);

        // vertex attribute
        //-----------------
        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, interleavedVerticesStride, (void*)0);
        glEnableVertexAttribArray(0);

        // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, interleavedVerticesStride, (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);

        // tex coords
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, interleavedVerticesStride, (void*)(6*sizeof(float)));
        glEnableVertexAttribArray(2);

        // unbind
        //-------
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};



#endif