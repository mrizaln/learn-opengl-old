#ifndef CUBE_H
#define CUBE_H

#include <cstddef>

#include <glad/glad.h>

#include <iostream>


class Cube
{
    static inline float s_CubeVertices[]{
        -1.0, -1.0, -1.0,
         1.0, -1.0, -1.0, 
         1.0,  1.0, -1.0, 
         1.0,  1.0, -1.0, 
        -1.0,  1.0, -1.0, 
        -1.0, -1.0, -1.0, 

        -1.0, -1.0,  1.0,
         1.0, -1.0,  1.0,
         1.0,  1.0,  1.0,
         1.0,  1.0,  1.0,
        -1.0,  1.0,  1.0,
        -1.0, -1.0,  1.0,

        -1.0,  1.0,  1.0,
        -1.0,  1.0, -1.0,
        -1.0, -1.0, -1.0,
        -1.0, -1.0, -1.0,
        -1.0, -1.0,  1.0,
        -1.0,  1.0,  1.0,

         1.0,  1.0,  1.0,
         1.0,  1.0, -1.0,
         1.0, -1.0, -1.0,
         1.0, -1.0, -1.0,
         1.0, -1.0,  1.0,
         1.0,  1.0,  1.0,

        -1.0, -1.0, -1.0,
         1.0, -1.0, -1.0,
         1.0, -1.0,  1.0,
         1.0, -1.0,  1.0,
        -1.0, -1.0,  1.0,
        -1.0, -1.0, -1.0,

        -1.0,  1.0, -1.0,
         1.0,  1.0, -1.0,
         1.0,  1.0,  1.0,
         1.0,  1.0,  1.0,
        -1.0,  1.0,  1.0,
        -1.0,  1.0, -1.0,
    };

    static inline float s_CubeNormals[]{
         0.0f,  0.0f, -1.0,
         0.0f,  0.0f, -1.0, 
         0.0f,  0.0f, -1.0, 
         0.0f,  0.0f, -1.0, 
         0.0f,  0.0f, -1.0, 
         0.0f,  0.0f, -1.0, 

         0.0f,  0.0f, 1.0,
         0.0f,  0.0f, 1.0,
         0.0f,  0.0f, 1.0,
         0.0f,  0.0f, 1.0,
         0.0f,  0.0f, 1.0,
         0.0f,  0.0f, 1.0,

        -1.0,  0.0f,  0.0f,
        -1.0,  0.0f,  0.0f,
        -1.0,  0.0f,  0.0f,
        -1.0,  0.0f,  0.0f,
        -1.0,  0.0f,  0.0f,
        -1.0,  0.0f,  0.0f,

         1.0,  0.0f,  0.0f,
         1.0,  0.0f,  0.0f,
         1.0,  0.0f,  0.0f,
         1.0,  0.0f,  0.0f,
         1.0,  0.0f,  0.0f,
         1.0,  0.0f,  0.0f,

         0.0f, -1.0,  0.0f,
         0.0f, -1.0,  0.0f,
         0.0f, -1.0,  0.0f,
         0.0f, -1.0,  0.0f,
         0.0f, -1.0,  0.0f,
         0.0f, -1.0,  0.0f,

         0.0f,  1.0,  0.0f,
         0.0f,  1.0,  0.0f,
         0.0f,  1.0,  0.0f,
         0.0f,  1.0,  0.0f,
         0.0f,  1.0,  0.0f,
         0.0f,  1.0,  0.0f
    };

    static inline float s_CubeTexCoords[72]{
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,

        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f,

        0.0f, 1.0f,
        1.0f, 1.0f,
        1.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 0.0f,
        0.0f, 1.0f
    };

    float sideLength();

    // vertices data
    float vertices[108]{};
    float normals[108]{};
    float texCoords[108]{};

    // interleaved vertices data
    float interleavedVertices[108*2 + 72]{};
    int interleavedVerticesStrideSize{};

    // buffers
    unsigned int VAO;
    unsigned int VBO;

public:
    Cube(float sideLength = 1.0f)
        : interleavedVerticesStrideSize{ 8*sizeof(float) }
    {
        // copy vertices multiplied by sidelength
        for (std::size_t i{ 0 }; i < std::size(vertices); i++)
            vertices[i] = s_CubeVertices[i] * sideLength;
        // std::copy(std::begin(s_CubeVertices), std::end(s_CubeVertices), std::begin(vertices));
        // std::for_each(std::begin(vertices), std::end(vertices), [sideLength](float& a){ a *= sideLength; });

        // copy normals
        std::copy(std::begin(s_CubeNormals), std::end(s_CubeNormals), std::begin(normals));

        // copy texCoords
        std::copy(std::begin(s_CubeTexCoords), std::end(s_CubeTexCoords), std::begin(texCoords));

        buildInterleavedVertices();
        setBuffers();
    }

    ~Cube()
    {
        // deleteBuffers();     // segmentation fault???
    }

    void draw() const
    {
        // bind buffer
        glBindVertexArray(VAO);

        // draw
        glDrawArrays(GL_TRIANGLES, 0, std::size(interleavedVertices));

        // unbind buffer
        glBindVertexArray(0);
    }

    void deleteBuffers()
    {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    }

    void print() const
    {
        auto& v{ interleavedVertices };
        for (std::size_t i{ 0 }; i < std::size(interleavedVertices); i += 8)
        {
            std::cout.precision(2);
            std::cout << v[i  ] << '\t' << v[i+1] << '\t' << v[i+2] << "\t\t"
                      << v[i+3] << '\t' << v[i+4] << '\t' << v[i+5] << "\t\t"
                      << v[i+6] << '\t' << v[i+7] << '\n';
        }
    }


private:
    void buildInterleavedVertices()
    {
        for (std::size_t i{ 0 }, j{ 0 }, k{ 0 }, l{ 0 }; i < std::size(interleavedVertices); i += 8, j += 3, k += 3, l += 2)
        {
            interleavedVertices[i]   = vertices[j];
            interleavedVertices[i+1] = vertices[j+1];
            interleavedVertices[i+2] = vertices[j+2];

            interleavedVertices[i+3] = normals[k];
            interleavedVertices[i+4] = normals[k+1];
            interleavedVertices[i+5] = normals[k+2];

            interleavedVertices[i+6] = texCoords[l];
            interleavedVertices[i+7] = texCoords[l+1];
        }
    }

    void setBuffers()
    {
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        //bind
        //----
        glBindVertexArray(VAO);

        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(interleavedVertices), interleavedVertices, GL_STATIC_DRAW);

        // vertex attribute
        //-----------------
        // position
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, interleavedVerticesStrideSize, (void*)(0));
        glEnableVertexAttribArray(0);

        // normal
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, interleavedVerticesStrideSize, (void*)(3*sizeof(float)));
        glEnableVertexAttribArray(1);

        // texcoords
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, interleavedVerticesStrideSize, (void*)(6*sizeof(float)));
        glEnableVertexAttribArray(2);

        // unbind
        //----
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }
};

#endif