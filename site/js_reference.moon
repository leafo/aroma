
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
    description: [[
      Hello world!
    ]]

    method {
      name: "constructor"
    }
  }

}

