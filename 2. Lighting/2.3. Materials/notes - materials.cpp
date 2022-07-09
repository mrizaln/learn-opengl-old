/*---------------------------------------------------------------------------------------
                        ============[ materials ]============
---------------------------------------------------------------------------------------*/

/*
  - in the real world, each object has a different reaction to light.
  - steel objects are often shiier than a clay vase for example and a wooden container does
    not reacht the same to light as a steel container.
  - if we want to simulate several types of objects in OpenGL, we have to define [material]
    properties specific to each surface.

  - when describing a surface we can define a material color for each of the 3 lighting
    component: ambient, diffuse, and specular lighting.
  - by specifying a color for each of the components we have fine-grained control over the
    color output of the surface.
  - now add shininess component to those 3 colors and we have all the material properties
    we need

  - in the fragment shader we create a struct to store the material properties of the surface.

        [ file: shader.fs; section: 2.3.material_struct ]

  - with these 4 components that define an object's material, we can simulate many real-world
    materials.
  - a table as found at [https://devernay.free.fr] shows a list of material properties that
    simulate real material material found in the outside world.
*/




/*---------------------------------------------------------------------------------------
                      ============[ setting materials ]============
---------------------------------------------------------------------------------------*/

/*
  - we have created a uniform material struct in the fragment shader, so next we want to
    change the lighting calculations to comply with the new material properties.

      [ file: shader.fs; section: 2.3.material_fragment_calculation ]

  - we can set the material of the object in the application by setting the appropiate
    uniform.
  - a struct in GLSL however is not special in any regard when setting uniforms; a struct
    only really acts as a namespace of uniform variables.
  - if we want to fill the struct, we will have to set the individual uniforms, but prefixed
    with the struct's name:

        [ file: materials.cpp; section: 2.3.set_material_uniform ]
*/



    //------------------------
    //    light properties
    //------------------------

    /*
      - light sources have different intensities for their ambient, diffuse, and specular
        components respectively.
      - in the previous chapter, we solved this by varying the ambient and specular intensities
        with a strength value.
      - we want to do something similar, but this time by specifying intensity vectors for
        each of the lighting components.

      - we'll want to create something similar to the material struct for the light properties.

            [ file:shader.fs; section: 2.3.light_struct ]

      - and then we change the lighting calculation.

            [ file: shader.fs; section: 2.3.material_fragment_calculation ]
      
      - last, we set the light intensities in the application

            [ file:materials.cpp; section: 2.3.set_light_intensities ]
    */



    //------------------------------
    //    different light colors
    //------------------------------

    /*
      - so far we use light colors to only vary the intensity of their individual components
        by choosing colors that range from white to gray to black, not affecting the actual
        colors of the object.
      - since we now have easy access to the light's properties, we can change their colors
        over time to get some really interesting effects.

            [ file: materials.cpp; section: 2.3.changing_light_color ]
    */