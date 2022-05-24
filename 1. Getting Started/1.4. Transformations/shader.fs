#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;


// - the fragment shader should also have access to the texture object, but how do we pass
//   the texture object to the fragment shader?
// - GLSL has a built-in data type for thexture object called a [sampler] that takes as a
//   postfix the texture type we want e.g. [sampler1D], [sampler2D], and [sampler3D].

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float alpha;

void main()
{
    // - to sample the color of a texture we use GLSL's built-in [texture] function that 
    //   takes 2 arguments:
    //       > texture sampler
    //       > texture coordinate

    // FragColor = texture(ourTexture, TexCoord) * vec4(ourColor, 1.0f);
    

    // mix two textures (texture units)
    /*
      - mix function takes two values as input and linearly interpolates between them based
        on its third argument
      - if the third value is 0.0, it returns the first input; if it's 1.0 it returns the
        second input value.
      - a value of 0.2 will return 80% of the first input color and 20% of the second input
        color.
    */
    FragColor = mix(texture(texture0, TexCoord), texture(texture1, TexCoord),  alpha);
}