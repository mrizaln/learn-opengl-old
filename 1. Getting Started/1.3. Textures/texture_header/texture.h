#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>  // include glad to get all the required OpenGL headers
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include <iostream>


class Texture{
    static inline unsigned int s_textureUnitCount{ 0 };

    int imageWidth{};
    int imageHeight{};
    int nrChannels{};
    unsigned char* imageData{};

public:
    unsigned int textureUnitNum;
    unsigned int textureID;

    Texture() = default;

    Texture(const char* texFilePath, bool flipVertically = true)
    {
        textureUnitNum = s_textureUnitCount++;

        stbi_set_flip_vertically_on_load(flipVertically);
        imageData = stbi_load(texFilePath, &imageWidth, &imageHeight, &nrChannels, 0);

        if(!imageData)
            std::cerr << "Failed to load texture: " << texFilePath;
        else
            generateTexture();
    
        stbi_image_free(imageData);
    }

private:
    // TODO: make a generateTexture() function that accepts parameters of texture wrap, texture min filter, and texture mag filter.
    void generateTexture()
    {
        // generate texture
        glGenTextures(1, &textureID);

        // bind texture
        glBindTexture(GL_TEXTURE_2D, textureID);

        // set texture parameters
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // now generate texture from image
        int imageFormat{ GL_RGB };
        if (nrChannels < 3)
        {
            std::cerr << "Image's color channel is less than 3 (no function can handle that for now)";
            return;
        }
        else if (nrChannels == 4)
            imageFormat = GL_RGBA;
 
        glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, imageWidth, imageHeight, 0, imageFormat, GL_UNSIGNED_BYTE, imageData);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
};


#endif