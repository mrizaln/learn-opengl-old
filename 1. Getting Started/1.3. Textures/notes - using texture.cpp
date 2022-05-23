// DON'T COMPILE THIS FILE

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <shader_header/shader.h>


/*---------------------------------------------------------------------------------------
                        ============[ Texture ]============
---------------------------------------------------------------------------------------*/

int texture()
{
    // in this lesson we use texture coordinate in 2D: (x,y).
    // each coordinate spans from 0 to 1.
    // it spans from (0,0) on the bottom-left corner to (1,1) on the top-right corner.

    float texCoords[] {                   
        0.0f,   0.0f,       // bottom-left
        1.0f,   0.0f,       // bottom-right
        0.5f,   1.0f,       // top-center
    };



    //----------------------//
    //   texture wrapping   //
    //----------------------//

    /*
        if the texture coodinate spans outside the range of (0,0) to (1,1), OpenGL can treat
        them differently:
            1.  GL_REAPEAT          : repeats the texture image     [ default ]
            2.  GL_MIRRORED_REPEAT  : same as above but mirrored
            3.  GL_CLAMP_TO_EDGE    : clamps the coordinate between 0 and 1, (coord > 1 becomes 1, coords < 0 becomes 0)
            4.  GL_CLAMP_TO_BORDER  : coordinates outside the range are now given a user-specified border color.
    */

    //each of the options can be set per coordinate axis
    // (s, t, r)   =>   equivalent to   (x, y, z)  coordinates in 3d space

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);   // if 3D

    // if we chose the GL_CLAMP_TO_BORDER option we need to specify border color;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    float borderColor[] { 1.0f, 1.0f, 0.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);



    //-----------------------//
    //   Texture Filtering   //
    //-----------------------//

    /*
      - texture coordinates do not depend on resulotion but can be any floating point value,
        thus OpenGL has to figure out which texture pixel (known as texel) to map the texture
        coordinate to
      - OpenGL has options for this [texture filtering].
      - there are several options available but for now, we'll discuss the most important
        options:    [GL_NEAREST] and [GL_LINEAR].

        > GL_NEAREST
            OpenGL selects the texel that center is closest to the texture coordinate.
        > GL_LINEAR
            [bilinear filtering]: takes an interpolated value from the texture coordinate's
                                  neighboring texels
    */

    /*
      - texture filtering can be set for [magnifying] and [minifying] operations.
      - we have to specify the filtering method for both operatoins via [glTexParameter*]
    */

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



    //-------------//
    //   Mipmaps   //
    //-------------//

    /*
      - basically a collection of texture images where each subsequent texture is twice as
        small compared to the previous one.

      - the idea behind mipmaps is that after a certain distance threshold from the viewer,
        OpenGL will use a different mipmap texture that best suits the distance to the object.
    */

    /*
      - creating a collection of mipmapped textures for each texture image is cumbersome to
        do manually.
      - luckily, OpenGL is able to do all the work for us with a single call to [glGenerateMipMaps]
        after we've created a texture.
    */

    /*
      - when switching btween mipmaps levels during rendering, OpenGL may show some artifacs
        like sharp edges visible between the two mipmap layes.
      - just like normal texture filtering, it is also possible to filter between mipmap
        levels using NEAREST and LINEAR filtering for switching between mipmap levels.
      
      - to specify the filtering method between mipmap levels we can replace the original
        filtering methods with one of the following:
            1. GL_NEAREST_MIPMAP_NEAREST    : takes the nearest mipmap to match the pixel size and uses nearest neighbor interpolation for texture sampling
            2. GL_LINEAR_MIPMAP_NEAREST     : takes the nearest impmap level and samples that level using linear interpolation
            3. GL_NEAREST_MIPMAP_LINEAR     : linearly interpolates between the two mipmaps that most closely match the size of a pixel and samples the interpolated level via nearest neighbor interpolation
            4. GL_LINEAR_MIPMAP_LINEAR      : linearly interpolates between the two closest mipmaps and samples the interpolated level via linear interpolation
    */

    // just like texture filtering, we can set the filtering method using [glTexParameteri]:

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // a common mistake is to set one of the mipmap filtering options as the magnification
    // filter (this doesn't have any effect).
}





/*---------------------------------------------------------------------------------------
            ============[ Loading and Creating Textures ]============
---------------------------------------------------------------------------------------*/

// first thing we need to do to actually use texture is to laod them to our application.

// we can use an image-loading library that supports several popular formats and does all
// the hard work for us.
// like: [stb_image.h]



//-------------------//
//    stb_image.h    //
//-------------------//

// stb_image.h can be downloaded here: https://github.com/nothings/stb/blob/master/stb_image.h

// add the file to you project as stb_image.h, and create an additional C++ file with the following code:

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>        // or something similar



int main()
{
    // to load an image using [stb_image.h], we use its [stbi_load] function:
    // we are using ./img/container.jpg file here

    int width, height, nrChannels;
    unsigned char* data { stbi_load("img/container.jpg", &width, &height, &nrChannels, 0) };



//--------------------------//
//   generating a texture   //
//--------------------------//

    // like any of the previous objects in OpenGL, textures are referenced with an ID
    unsigned int texture;
    glGenTextures(1, &texture);

    // we need to bind it
    glBindTexture(GL_TEXTURE_2D, texture);

    // now it has bound, we can start generating a texture using the previously loaded image data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0 /*should always be zero (legacy)*/, GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);    // automatically generate all the required mipmaps for the currently bound texture

    // after we're done generating the texture (and mipmaps), it's good practice to free
    // the image memory:
    stbi_image_free(data);



//-----------------------//
//   applying textures   //
//-----------------------//

    // for the upcoming sections we will use the rectangle shape drawn with [glDrawElements]

    float vertices[] {
        // positions            // colors               // texture coords
         0.5f,  0.5f, 0.0f,     1.0f, 0.0f, 0.0f,       1.0f, 1.0f,         // top-right
         0.5f, -0.5f, 0.0f,     0.0f, 1.0f, 0.0f,       1.0f, 0.0f,         // bottom-right
        -0.5f, -0.5f, 0.0f,     0.0f, 0.0f, 1.0f,       0.0f, 0.0f,         // bottom-left
        -0.5f,  0.5f, 0.0f,     1.0f, 1.0f, 0.0f,       0.0f, 1.0f          // top-left
    };

    // since we'ce added an extra vertex attribute, we agan have to notify OpenGL of the new vertex format

    /*
    | X|Y|Z | R|G|B | S|T | X|Y|Z | R|G|B | S|T | X|Y|Z | R|G|B | S|T |      // 4 bytes each
    
        position:   - offset:  0
                    - stride: 32
        
        color   :   - offset: 12
                    - stride: 32
        
        texture :   - offset: 24
                    - stride: 32
    */

    // position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // texture
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // next, we need to alter the vertex shader and the fragment shader to accept the texture
    // open [shader.vs] and [shader.fs] for more information


    // all that left to do now is to bind the texture before calling [glDrawElements] and
    // it will automatically assign the texture to the fragment shader's sampler.

    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);





//-------------------//
//   Texture Units   //
//-------------------//

    /*
      - you probably wondered, why the [sampler2D] variable is a uniform if we didn't even assign
        it some value with [glUniform].
      - using [glUniform1i] we an actually assign a location value to the texture sampler so we
        can set multiple textures at once in a fragment shader.
      - this location of a texture is more commonly known as a [texture unit].
      - the default texture unit for a texture is 0 which is the default active texture unit so
        we didn't need to assign a location in the previous section (not all graphics drivers
        assign a default texture unit)

      - the main purpose of texture units is to allow us to use more than 1 texture in our shaders.
      - by assigning texture units to the samplers, we can bind to multiple textures at once as
        long as we activate the corresponding texture unit first.
      - we can activate texture units using [glAtivateTexture] passing in the texture unit we'd
        like to use
    */

    glActiveTexture(GL_TEXTURE0);               // activate the texture unit first before binding texture.
    glBindTexture(GL_TEXTURE_2D, texture);


    // we still, however, need to edit the fragment shader to accept another sampler.
    // the final output color is now the combination of two texture lookups.


    // we also need to load and create another texture
    unsigned char* data = stbi_load("img/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // note that we now laod a .png file that includes an alpha channel; we need to specify
    // the image data contains an alpha channel as wee by using [GL_RGBA]


    // to use the second texture (and the first texture) we'd have to change the rendering
    // procedute a bit by binding both textures to the corresponding texture unit:
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture1);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


    // we also have to tell OpenGL to which texture unit each shader sampler belong to by
    // setting each sampler using [glUniform1i]
    theShader.use();
    glUniform1i(glGetUniformLocation(theShader.ID, "texture1"), 0);     // set it manually
    theShader.setInt("texture2", 1);        // or with shader class

    while (...)
    {
        [...];
    }
}
