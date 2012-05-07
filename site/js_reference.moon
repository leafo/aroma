
format "lib.reference"

package {
  name: "aroma.js"
  description: [[
    The JavaScript API manages the creation and existence of your game in the
    browser. JavaScript is used to create the Aroma window and start the
    execution of your game.

    You can use this API to set up the size of your game window and handle
    printing to standard out.

    Create a game frame like so:

        <script type="text/javascript" src="aroma.js"></script>
        <div id="game_container"></div>

        <script type="text/javascript">
          var container = document.getElementById("game_container");
          var game = new Aroma(container, 640, 480, {
            loaded: function() {
              alert("Aroma is ready!");
            }
          });
        </script>

    The third argument of the `Aroma` constructor is an object of functions for
    various events. In this example we provide a function for the `loaded`
    event. The loaded event fires when Aroma's binary has finished downloading.

    After Aroma has loaded, it waits idly for some code to run. Let's write
    something to the screen. We use the `execute` method to run arbitrary Lua
    in the game frame:

        var game = new Aroma(container, 640, 480, {
          loaded: function() {
            game.execute("function aroma.draw() aroma.graphics.print('Hello World!', 10, 10) end");
          }
        });

    The example above is one of the simplest examples of an Aroma game.

    By default, standard out is sent into Chrome's `console.log` and standard
    error is sent into `console.error`. This means if we use the Lua function
    `print` we will see the output in in the console. Likewise, if your code
    crashes the error information will also show up in the console. (You can
    open up the console by clicking on the wrench icon and going to Tools >
    JavaScript Console)
  ]]

  type {
    name: "Aroma"
    instance_name: "game"

    description: [[
      Controls the game instance and message passing between the game and the
      browser.
    ]]

    constructor {
      args: {"container", "width", "height", "event_handlers"}
      returns: {"aroma"}
      description: [[
        Creates Aroma frame and Lua environment. Causes the `nexe` to be
        downloaded if it hasn't been already.

        * `container` -- A dom node where the game will be placed. Can also be a
          string which represents an id of a dom node.

        * `width` -- Width of the frame in pixels.

        * `height` -- Height of the frame in pixels.

        * `event_handlers` -- An object containing functions for certain named
          events.


        Possible event handlers include:

        * `"loaded"` -- called when the frame is ready to execute Lua.

        * `"std_out"` -- Called whenever Lua writes to standard out. Recieves one
          argument, what was written.

        * `"std_err"` -- Same as above, except for standard error.


        By default, `std_out` is set to write to `console.log` and `std_err` is
        set to write to `console.err`.
      ]]

      code: [[
        var game = new Aroma("game_container", 640, 480, {
          loaded: function() {
            game.execute("print 'hello world!'");
          },
          std_out: function(msg) {
            alert(msg);
          }
        });
      ]]
    }

    method {
      name: "execute"
      args: {"lua_code"}

      description: [[
        Takes one argument, `lua_code`, a string of Lua code that will be
        executed.

        Will reset all game state. This should only be called when you want to
        run a new game.
      ]]
    }
  }

}

