
format "lib.reference"

set "title", "Lua Reference"

lua_type = (o) ->
  o.invoke_operator = ":"
  type o

package {
  name: "nacl"
  description: [[
    Interface to the NaCl container.
  ]]

  method {
    name: "prefetch"
    args: {"resource_table"}
    description: [[
      Causes Aroma to block and fetch a collection of resources at once. The
      browser will download all the resources in parallel, which will take much
      less time than requesting them individually.

      This will not return any of the resources, it will only cache their
      values on the client so when you do ask for the request later it will be
      available without blocking the game.
    ]]
    code: [[
      nacl.prefetch({
        images = {
          "thing.png",
          "world.png",
        }
      })
    ]]
  }

  method {
    name: "track_event"
    args: {"category", "action", "label", "value", "interactive"}
    description: [[
      This will attempt to send a custom event to Google Analytics. Only works
      if it's been set up on the game page.
    ]]

    code: [[
      if player.beat_game then
        nacl.track_event("my_game", "player_event", "win")
      end
    ]]
  }
}

package {
  name: "aroma"
  description: [[
    The `aroma` object serves two parts. It contains all Aroma
    submodules, which are also listed on this page, and it is where callback
    functions are assigned.

    Callbacks are functions that you create and assign to the `aroma` object.
    They will be called by the engine when certain events happen.

    You only need to assign these callback functions when if you are using
    them, leaving them out is perfectly okay. You should never need to call
    these functions manually (but you can).

    Here is the complete list of callbacks:
  ]]

  method {
    name: "draw"
    description: [[
      Where all of your drawing code should go.
    ]]
    code: [[
      function aroma.draw()
        aroma.graphics.print("hello world", 10, 10)
      end
    ]]
  }

  method {
    name: "update"
    args: {"dt"}
    description: [[
      Where game state updates should go. Takes one argument, `dt`, the amount
      of time in seconds since the last frame.

      You should not draw from this function, nothing will show up.
    ]]
    code: [[
      function aroma.update(dt)
        player.x = player.x + speed * dt
      end
    ]]
  }

  method {
    name: "focus"
    args: {"has_focus"}
    description: [[
      Called when the focus of the game window has changed
    ]]
    code: [[
      function aroma.focus(has_focus)
        print("Focus changed:", has_focus)
      end
    ]]
  }

  method {
    name: "keypressed"
    args: {"key_name", "key_code"}
    description: [[
      Called when a key is pressed down. Given two arguments:

      * `key_name` -- A string representing the name of the key pressed

      * `key_code` -- An integer representing the code of the key
    ]]

    code: [[
      function aroma.keypressed(key_name, key_code)
        print("Key pressed:", key_name, key_code)
      end
    ]]
  }

  method {
    name: "keyreleased"
    args: {"key_name", "key_code"}
    description: [[
      Callen when a key is released.

      Arguments are the same as [aroma.keypressed](#aroma.keypressed).
    ]]
  }
}



package {
  name: "aroma.graphics"
  show_tags: true
  tag_order: { "drawing", "constructors", "transformations", "color" }

  description: [[
    Functions responsible for drawing things on the screen or changing the
    state of drawing.
  ]]

  method {
    name: "setColor"
    tag: "color"
    args: { "red", "green", "blue", "[alpha]" }
    description: "Set the current color for drawing."

    code: [[
      -- draw a red rectangle
      function aroma.draw()
        aroma.graphics.setColor(255,0,0)
        aroma.graphics.rectangle(10,10, 50,50)
      end
    ]]
  }

  method {
    name: "getColor"
    tag: "color"
    returns: {"red", "green", "blue", "alpha"}
    description: "Gets the current color for drawing."
  }

  method {
    name: "setBackgroundColor"
    tag: "color"
    args: { "red", "green", "blue", "alpha" }
    description: "Set the color the screen is cleared to after every frame."
  }

  method {
    name: "setLineWidth"
    args: {"width"}
    description: "Set the width of lines lines drawn."
  }

  method {
    name: "getWidth"
    returns: { "width" }
    description: "Gets the width of the viewport in pixels."
  }

  method {
    name: "getHeight"
    returns: { "height" }
    description: "Gets the height of the viewport in pixels."
  }

  method {
    name: "reset"
    description: [[
      Resets graphics state to default. This includes:

        * Background color
        * Drawing color
        * Blend mode
    ]]
  }

  method {
    name: "rectangle"
    tag: "drawing"
    args: { "[render_style]", "x", "y", "width", "height" }
    description: [[
      Draws a rectangle on the screen.

      The color of the rectangle is the current color. See
      [aroma.graphics.setColor](#aroma.graphics.setColor).

      `render_style` is an optional string that determines how the rectangle is
      drawn. `"fill"` is the default. Must be one of the following:

        * `"fill"`
        * `"line"`
    ]]
  }

  method {
    name: "draw"
    tag: "drawing"
    args: {
      "drawable", "x", "y",
      "[rotate]", "[scale_x]", "[scale_y]", "[origin_x]", "[origin_y]"
    }
    description: [[
      Draws a *drawable* object on the screen. Right now the only drawable
      objects are images.

      `rotate` is clockwise rotation in degrees.

      `origin_x` and  `origin_y` can be used to control the origin of rotation.
    ]]
  }

  method {
    name: "drawq"
    tag: "drawing"
    args: {
      "drawable", "quad", "x", "y",
      "[rotate]", "[scale_x]", "[scale_y]", "[origin_x]", "[origin_y]"
    }
    description: [[
      Similar to [aroma.graphics.draw](#aroma.graphics.draw), but takes a
      [Quad](#aroma.graphics.Quad) as second argument.

      A Quad can be used to only draw a portion of an image. Useful for drawing
      sprites from a single image.
    ]]

    code: [[
      local img = aroma.graphics.newImage("hi.png")
      local q = aroma.graphics.newQuad(5, 5, 10, 10, img:getWidth(), img:getHeight())

      -- draws the part of the image specified by the quad at 10, 10
      function aroma.draw()
        aroma.graphics.drawq(img, q, 10, 10)
      end
    ]]
  }

  method {
    name: "setDefaultShader"
    args: {"shader"}
    description: [[
      Replaces the shader responsible for all drawing. Be careful with this. If
      the shader does not work in a specific way then nothing may be drawn or
      Aroma may crash.

      `shader` must be a shader created with
      [aroma.graphics.newShader](#aroma.graphics.newShader).
    ]]
  }

  method {
    name: "newShader"
    tag: "constructors"
    args: {"vertex_shader_source", "fragment_shader_source"}
    returns: {"shader"}
    description: [[
      Create a new shader.
    ]]
  }

  method {
    name: "newQuad"
    tag: "constructors"
    args: {"x", "y", "width", "height", "source_width", "source_height"}
    returns: {"quad"}
    description: [[
      Creates a new `Quad` for use with [aroma.graphics.drawq](#aroma.graphics.drawq).

      `source_width` and `source_height` are typically the width and height of
      the image the quad is being used on.

      `x`, `y`, `width`, `height` represent the rectangle that should be drawn
      from the image.
    ]]
  }

  method {
    name: "newImage"
    tag: "constructors"
    args: {"image_url"}
    returns: {"image"}
    description: [[
      Loads and image by url and returns it as an [Image](#Image) object.

      Images can be drawn to the screen using
      [aroma.graphics.draw](#aroma.graphics.draw).
    ]]

    code: [[
      local image = aroma.graphics.newImage("hello.png")

      function aroma.draw()
        aroma.graphics.draw(image, 10, 10)
      end
    ]]
  }

  method {
    name: "newFont"
    tag: "constructors"
    args: {"font_name", "[alphabet]"}
    returns: {"font"}
    description: [[
      Creates a new font and returns it. Fonts are rendered by the browser in a
      `<canvas>` tag and then passed back to Aroma.

      `font_name` must be a valid CSS font value.

      `alphabet` is an optional string whose characters will be the glyphs
      available in the font. Defaults to all numbers, letters, and symbols on
      an English keyboard.

      Custom fonts can be made available by embedding fonts into the stylesheet
      of the host page using `@font-face`.
    ]]

    code: [[
      local monospace = aroma.graphics.newFont("32px monospace")

      -- only numbers can be written with this font
      local numbers = aroma.graphics.newFont("10pt serif", "0987654321")
    ]]
  }

  method {
    name: "getFont"
    returns: { "font" }
    description: [[
      Gets the current font.
    ]]
  }

  method {
    name: "setFont"
    args: { "font" }
    description: [[
      Sets the current font. `font` must be a font return by
      [aroma.graphics.newFont](#aroma.graphics.newFont).
    ]]
  }

  method {
    name: "push"
    tag: "transformations"
    description: [[
      Copies and pushes the current transformation onto the transformation
      stack.
    ]]
  }

  method {
    name: "pop"
    tag: "transformations"
    description: [[
      Pops the current transformation off the transformation stack.
    ]]
  }

  method {
    name: "translate"
    tag: "transformations"
    args: {"tx", "tx"}
    description: [[
      Modifies the current transformation by translating all draws by `tx` and
      `ty`.
    ]]

    code: [[
      function aroma.draw()
        aroma.graphics.translate(20, 20)
        -- will be drawn at 25, 25
        aroma.graphics.rectangle(5, 5, 10, 10)
      end
    ]]
  }

  method {
    name: "scale"
    tag: "transformations"
    args: { "sx", "sy" }
    description: [[
      Modifies the current transformation by scaling all draws by `sx` and
      `sy`.
    ]]
  }

  method {
    name: "rotate"
    tag: "transformations"
    args: { "deg" }
    description: [[
      Modifies the current transformation to rotate all draws by `deg` degrees.

      The center of origin is at `(0,0)`. It is helpful to combine this with
      translation control the center of origin relative to the object being
      drawing.
    ]]
  }

  method {
    name: "print"
    tag: "drawing"
    args: {"[font]", "text", "x", "y"}
    description: [[
      Draws text on the screen. If `font` is not provided, the default font
      will be used. See [aroma.graphics.setFont](#aroma.graphics.setFont).
    ]]
  }


  lua_type {
    name: "Image"

    description: [[
      Images can be drawn with
    ]]

    method {
      name: "getWidth"
      returns: {"width"}

      description: [[
        Gets the width of the image.
      ]]
    }

    method {
      name: "getHeight"
      returns: {"height"}

      description: [[
        Gets the height of the image.
      ]]
    }

    method {
      name: "setWrap"
      args: {"horiz_wrap", "vert_wrap"}
      description: [[
        Sets the wrapping mode of the image when overdrawn (by something like
        `drawq`). By default wrap is set to `"clamp"`.

        Possible values for each:

        * `"clamp"` -- the color at the edge of the texture will be used to
          fill the extra space.

        * `"repeat"` -- The texture will repeat to fill extra space.
      ]]
    }

    method {
      name: "setFilter"
      args: {"min_filter", "mag_filter"}
      description: [[
        Sets how images are filtered when they are scaled. By default wrap is
        set to `"linear"` for both min and mag.

        * `min_filter` -- applied when the image is rendered smaller
          than its original dimensions.

        * `mag_filter` -- applied when the image is rendered larger
          its original dimensions.


        Posible values include:

        * `"linear"` --  The colors of nearby pixels are blended, creates a
          smoothing effect.

        * `"nearest"` -- no blending is done. This is the best filter to use
          when working with pixel art.

      ]]
    }
  }


  lua_type {
    name: "Font"
    description: [[
      Represents a loaded font. Created with [aroma.graphics.newFont](#aroma.graphics.newFont).

      Can be drawn with [setFont](#aroma.graphics.setFont) or
      [print](#aroma.graphics.print).
    ]]
  }

  lua_type {
    name: "Quad"
    description: [[
      Represents a rectangular portion of something. Created with [aroma.graphics.newQuad](#aroma.graphics.newQuad).

      Drawn with [aroma.graphics.drawq](#aroma.graphics.drawq).
    ]]


    method {
      name: "flip"
      args: {"x", "y"}
      description: [[
        * `x` -- Flips Quad horizontally if true

        * `y` -- Flips Quad vertically if true
      ]]
    }
  }
}

package {
  name: "aroma.audio"
  description: [[
    Functions for working with sound.
  ]]

  method {
    name: "newSource"
    args: {"source_url", "[source_type]"}
    returns: {"audio"}
    description: [[
      Loads a new source by url. Returns an [AudioSource](#aroma.audio.AudioSource).

      `source_type` is an optional value that determines how the sound is
      loaded. `"static"` is the default. The following types are valid:

      * `"static"` -- The function will block until the entire audio source is
        loaded into memory and ready be played. Uses the Web Audio API.
        Suitable for short samples such as sound effects.

      * `"streaming"` -- Uses the `<audio>` tag. The function will not wait for
        the entire file to download, will only verify that the file exists.
        Playback of the sound might be delayed. Can also be unreliable.
        Suitable for background music.
    ]]

    code: [[
      local bg = aroma.audio.newSource("game/theme.ogg", "streaming")
      local effect = aroma.audio.newSource("game/shoot.ogg")

      bg:play() -- not guaranteed to play immediately, could still be downloading

      effect:play() -- guaranteed to play
    ]]
  }

  lua_type {
    name: "AudioSource"
    instance_name: "source"

    description: [[
      A static or stremaing audio srouce. Created with [aroma.audio.newSource](#aroma.audio.newSource).
    ]]

    method {
      name: "play"
    }

    method {
      name: "pause"
    }

    method {
      name: "stop"
    }

    method {
      name: "setLooping"
      args: {"looping"}
    }

  }

}

package {
  name: "aroma.image"
  description: [[
    Functions for manipulating images and image data.
  ]]

  method {
    name: "newImageData"
    args: {"image_url"}
    returns: {"data"}
  }
}

package {
  name: "aroma.keyboard"
  description: "Functions for querying information about the keyboard."

  method {
    name: "isDown"
    args: {"key_name"}
    returns: {"is_down"}
    description: [[
      Checks if a key is currently pressed down.
    ]]

    code: [[
      function aroma.update()
        if aroma.keyboard.isDown(" ") then
          print("Space is pressed!")
        end
      end
    ]]
  }
}

package {
  name: "aroma.timer"
  description: "Functions relating to the internal timer."

  method {
    name: "getTime"
    returns: {"time"}
    description: [[
      Gets the value of the internal timer in seconds. The value by itself is
      meaningless but it useful when compared against other times.
    ]]
  }

  method {
    name: "getFPS"
    returns: {"fps"}
    description: "Gets the frames per second of the game."
  }

}

