
format "lib.reference"

package {
  name: "nacl"
  description: [[
    Interface to the NaCL container.
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
  }
}

package {
  name: "aroma.graphics"
  description: [[
    Functions responsible for drawing things on the screen or changing the
    state of drawing.
  ]]

  method {
    name: "setColor"
    args: { "red", "green", "blue", "alpha" }
    description: "Set the current color for drawing"
  }
}



