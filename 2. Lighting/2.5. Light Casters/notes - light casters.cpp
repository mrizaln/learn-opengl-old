/*---------------------------------------------------------------------------------------
                       ============[ Light Casters ]============
---------------------------------------------------------------------------------------*/

/*
  - in real world, we have several types of light that each act different.
  - a light source that /casts/ light upon object is called a [light caster].
  - in this chapter we'll discuss several different types of light casters.
*/




/*---------------------------------------------------------------------------------------
                     ============[ Directional Light ]============
---------------------------------------------------------------------------------------*/

/*
  - when a light source is far away, the light rays coming from the light source are coles
    to parallel to each other.
  - it looks like all the light rays are coming from the same direction, regardless of where
    the object and/or the viewer is.
  - when a light source is modeled to be /infinitely/ far away, it isa called a [directional
    light] since all its ligh rays have the same direction.

  - we can model such a directional light by defining a light direction vector instead of
    a position vector.
  - the shader calculations remain mostly the same except this time we directly use the
    light's direction vector instead calculating the lightDir vector using the lights
    position vector.

        [file: shader.fs; section:diffuse ]

  - the directional light direction generally specified as a global direction pointing from
    the light source.

  - don't forget to actually specify the direction of the light source (note that we define
    the direction as a direction from the light source)

        [file: "directional light/directional light.cpp"; section: "set light uniforms"]

        | we've been passing the light's position and direction vectors as vec3 for a while
        | now, but some people prefer to keep all the vectors defined as vec4. when defining
        | position vectors as a vec4 it is important to set the w component to 1.0 so
        | translations and projections are properly applied. however, when defining a direction
        | vector as a vec4 we don't want translations to have an effect so then we define
        | the w component to be 0.0.

        | this trick above can function as an easy check for light types: you could check
        | if the w component is equal to 1.0 to see that we now have a light's position vector
        | and if w is equal to 0.0 we have a light's direction vector.
*/




/*---------------------------------------------------------------------------------------
                       ============[ Point Lights ]============
---------------------------------------------------------------------------------------*/

/*
  - a point light is a light source with a given position somewhere in a world that illuminates
    in all directions, where the light rays fade out over distance.
  - in the earlier chapters we've been working with a simplistic point light.
  - we had a light source at a given position that scatters light in all directions that
    that never fades out.
*/



    //-------------------
    //    attenuation
    //-------------------

    /*
      - to reduce the intensity of light over the distance a light ray travels is generally
        called [attenuation].
      - in the real world, lights intensity are decresing quadratically.
      - the following formula calculates an attenuation value based on a fragment's distance
        to the light source

            F = 1.0 / (K₀ + K₁⋅d + K₂⋅d²)

                K₀ : constant term          (kept at 1.0)
                K₁ : linear term
                K₂ : quadratic term
    */


        // [ choosing the right values ]

        /*
          - setting the right values depend on many factors: the environment, the distance
            you want a light to cover, the type of light, etc.
          - in most cases, it simply is a question of experience and a moderate amount of
            tweaking.

          - the following table shows some of the values these terms could take to simulate
            a realistic (sort of) light source that covers a specific radius

                distance |  constant |  linear |    quadratic
                ---------------------------------------------
                7 	     |  1.0      |  0.7    |    1.8
                13 	     |  1.0      |  0.35   |    0.44
                20 	     |  1.0      |  0.22   |    0.20
                32 	     |  1.0      |  0.14   |    0.07
                50 	     |  1.0      |  0.09   |    0.032
                65 	     |  1.0      |  0.07   |    0.017
                100 	 |  1.0      |  0.045  |    0.0075
                160 	 |  1.0      |  0.027  |    0.0028
                200 	 |  1.0      |  0.022  |    0.0019
                325 	 |  1.0      |  0.014  |    0.0007
                600 	 |  1.0      |  0.007  |    0.0002
                3250 	 |  1.0      |  0.0014 |    0.000007

                source: http://www.ogre3d.org/tikiwiki/tiki-index.php?page=-Point+Light+Attenuation
        */


        // [ implementing attenuation ]

        /*
          - to implement attenuation we'll be needing 3 extra values in the fragment shader:
            namely the constant, linear, and quadratic term of the equation.
          - these are best stored in the [Light] struct we defined earlier.
          - note that we need to calculate lightDir again using position as this is a point
            light.

                [ file: "point lights/shader.fs"; section: "light struct" ]

          - then we set the termns in our application

                [ file: "point lights/point lights.cpp"; section: "set light attenuation constants" ]

          - implementing attenuation in the fragment shader is relatively straightforward:
            simply calculate an attenuation value based on the equation and multiply this
            with the ambient, diffuse and specular components.

                [ file: "point lights/shader.fs"; section: attenuation]
        */




/*---------------------------------------------------------------------------------------
                        ============[ spotlight ]============
---------------------------------------------------------------------------------------*/

/*
  - a spotlight is a light source that is located somewhere in the environment that, instead
    of shooting light rays in all directions, only shoots them in a specific direction.
  - the result is that only the objects within a certain radius of the spotlight's direction
    are lit and everything else stays dark.

  - a spotlight in OpenGL is represented by a world-space position, a direction and a [cutoff]
    angle that specifies the radius of the spotlight.
  - for each fragment we calculate if the fragment is between the spotlight's cutoff directions
    and if so, we lit the fragment accordingly.

                        [light]                     lightDir: the vector pointing from the fragment to the light source
                         / | \                      spotDir : the direction the spotlight is aiming at
                        /  |  \                     ϕ       : the cutoff angle
                       / ϕ |   \                    θ       : the angle between lightDir and spotDir
                      /    |    \
                     /     ↓     \
                    /    spotDir  \
            ---------■---------------------
            |     / fragment        \     |
            |    /                   \    |
            |   /                     \   |

  - so what we basically need to do, is calculate the dot product between the lightDir vector
    and the spotDir vector and compare this with the cutoff angle ϕ
*/



    //------------------
    //    flashlight
    //------------------

    /*
      - a flashlight is a spotlight located at the viewer's position and usually aimed straight
        ahead from the player's perspective.
      - a flashlight is basically a normal spotlight, but with its position and direction
        continually updated based on the player's position and orientation.

      - so the values we're going to need for the fragment shader are the spotlight's position
        vector, the spotlight's direction vector and the cutoff angle.
      - we can store these values in the [Light] struct.

            [ file: spotlight/shader.fs; section: spotlight ]

      - next we pass the appropiate values to the shader

            [ file: spotlight/spotlight.cpp; section: "set spotlight" ]

      - now what's left to do is calculate the theta θ value and compare this with the cutoff
        ϕ value to determine if we're in or outside the spotlight

            [ file: spotlight/shader.fs; section: "spotlight calculation" ]
    */



    //-------------------------
    //    smooth/soft edges
    //-------------------------

    /*
      - to create the effect of a smoothly-edged spotlightwe want to simulate a spotlight
        having an inner and outer code.
      - we can set the inner cone as the cone defined in the previous section but we also
        want an outer cone that gradually dims the light from the inner to the edges of the
        outer cone.

      - to create an outer we simply defined another cosine value that represents the angle
        between the spotlight's direction vector and the outer cone's vector.
      - then if a fragment is between the inner and the outer cone, it should calculate an
        intensity between 0.0 and 1,0.

      - we can calculate such a value using:
                    I = (θ - γ)/ϵ

            ϵ = the cosine difference between the inner (ϕ) and outer (γ) cone.
        
      - we now have an intensity value that is either negative when outside the spotlight,
        higher that 1.0 when inside the inner cone, and somewhere in between around the edges.
      - if we properly clamp the values we don't need an if-else in the fragment shader anymore
        and we can simply multiply the light components with the calculated intensity value.

            [ file: spotlight/shader.fs; section: "smooth edges"]
    */