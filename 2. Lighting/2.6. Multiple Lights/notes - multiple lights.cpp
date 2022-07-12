/*---------------------------------------------------------------------------------------
                      ============[ Multiple Lights ]============
---------------------------------------------------------------------------------------*/

/*
  - to use more than one light source in the scene, we want to encapsulate the lighting
    calculations into GLSL functions.
  - the reason is that the code quickly gets nasty when do lighting computations with multiple
    light types, each requiring different computations.

  - functions in GLSL are just C-like functions.
  - we have a functions name, a return type and we need to declare a prototype at the top
    of the code file if the function hasn't been declared yet before the main function.
  - we'll create a different function for each of the light types: directional lights, point
    lights, and spotlights.

  - when using multiple lights in a scene the approach is usually as follows:
      + we have a single color vector that represents the fragment's output color.
      + for each light, the light's contribution to the fragment is added to this output
        color vector.
      + so each light in the scene will calculate its individual impact and contribute that
        to the final output color.
  - a general structure would look something like this:
*/

    #ifdef GLSL_CODE
        out vec4 FragColor;

        void main()
        {
            // define an output color value
            vec3 output = vec3(0.0);

            // add the directional light's conribution to the output
            output += someFunctionToCalculateDirectionalLight();

            // do the same for all point lights
            for (int i = 0; i < nr_of_point_lights; ++i)
                output += someFunctionToCalculatePointLight();

            // and add others lights as well (like spotlight)
            output += someFunctionsToCalculateSpotLight();

            FragColor = vec4(output, 1.0);
        }
    #endif

/*
  - the actual code will likely differ per implementation, but the general structure remains
    the same.
  - we define several functions that calculate the impact per light source and add its resulting
    color to an output color vector.
*/



    //-------------------------
    //    directional light
    //-------------------------

    /*
      - we want to define a function in the fragment shader that calculates the contribution
        a directional light has on the correspoindif fragment.
      
      - first we need to set the required variables that we minimally need for a directional
        light source.
      - we can store the variables in a struct called [DirLight] and define it as a uniform.
    
            [ file: shader.fs; section: "directional light source" ]

      - we can then create a function that calculate directional light

            [ file: shader.fs; section: "calculate directional light" ]
    */



    //-------------------
    //    point light
    //-------------------

    /*
      - similar to directional lights, we also want to define a function that calculates
        the contribution a point light has on the given fragment, including its attenuation.
      - we define the struct first.

            [ file: shader.fs; section: "point light source" ]

      - we then define a function that calculate points light

            [ file: shader.fs; section: "calculate point light" ]
    */



    //-----------------
    //    spotlight
    //-----------------

    //      [ file: shader.fs; section: "spotlight" ]
    //      [ file: shader.fs; section: "calculate spotlight" ]



    //-------------------------------
    //    putting it all together
    //-------------------------------

    /*
      - put all to the main() function

            [ file: shader.fs; section: "putting it all together" ]

      - each light type adds its contribution to the resulting output color until all light
        sources are processed.
      - the resulting color contains the color impact of all the light sources in the scene
        combined.

      - after that, we just need to set the uniforms inside our application.
    */