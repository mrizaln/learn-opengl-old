/*---------------------------------------------------------------------------------------
                      ============[ Lighting Maps ]============
---------------------------------------------------------------------------------------*/

/*
  - in the previous chapter we defined a material for an entire object as a whole.
  - objects in real world however usually do not consist of a single material, but of several
    materials.

  - think of a car: its exterior conststs of a shiny fabric, it has windows that partly reflect
    the surrounding environment, its tires are all but shiny so they don't have specular
    highlights and it has rims that are super shiny.
  - all in all, such an object has different material properties for each of its different
    parts.

  - sothe material system in the previous chapter isn't sufficient for all but the simplest
    models so we need to extend the system by introducing /diffuse/ and /specular/ maps.
*/




/*---------------------------------------------------------------------------------------
                      ============[ Diffuse Maps ]============
---------------------------------------------------------------------------------------*/

/*
  - what we want is some way to set the diffuse colors of an object for each individual fragment.
  - we're just using a different name for the same undelying principle: using an image wrapped
    around an object that we can index for unique color values per fragment.
  - in lit scenes this is usually called a [diffuse map]
  
  - to demonstrate diffuse maps, we're going to use the following image:
            [../../resources/img/container2.png]

  - using a diffuse map in shaders is exactly like we showed in the texture chapter.
  - this time however we store the texture as a sampler2D inside the Material struct.

        | keep in mind that sampler2D is a so called [opaque type] which means we can't
        | instantiate these types, but only define them as uniforms. if the struct would
        | be instatntiated other that as a uniform GLSL could throw strange errors; the
        | same thus applies to any struct holding such opaque types.

  - we also remove the ambient material color vector since the ambient color is equal to
    the diffuse color anyway.

        [ file: shader.fs; section: 2.4_update_material_struct ]

  - note that we are going to need texture coordinates again in the fragment shader, so we
    declared an extra input variable.
  - then we simply sample from the texture to retrieve the fragment's diffuse color value.
  - also don't forget to set the ambient material's color equal to the diffuse material's
    color as well.

        [ file: shader.fs; section: 2.4_update_ambient_and_diffuse ]

  - to get the diffuse map working, we need to update the vertex data with texture coordinates,
    transfer them as vertex attributes to the fragment shader, load the texture, and bind
    the texture to the appropiate texture unit.

        [ we already know this ]

  - before rendering the cube we want to assign the right texture unit to the [material.diffuse]
    uniform sampler and bind the container texture to this texture unit
*/




/*---------------------------------------------------------------------------------------
                      ============[ Specular Maps ]============
---------------------------------------------------------------------------------------*/

/*
  - basically not so different from diffuse maps.
  - it is used to give an object a different specular on its parts.

  - with specular map, we can specify with enormous detail what parts of an object have shiny
    properties and we can even control the corresponding intensity.
  - specular maps give us an added layer of control over lighting on top of the diffuse map.
*/