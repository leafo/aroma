    template: "tutorial"
--
<div class="index">$index</div>

## Before Starting

### What is Native Client?

Already know? Skip to [Enable Native Client Everywhere](#enable_native_client_everywhere).

Native Client is a (fairly) new technology that is built into all modern
versions of Chrome. It enables developers to package compiled code of languages
such as C and C++ into binaries that are downloaded and run on a users
computer.

Typically the language of web applications has been JavaScript, but with Native
Client our options are expanded significantly.

Aroma uses the scripting language [Lua][2] to enable you write games. Because Lua is
written in C, it can be compiled into a Native Client binary and distributed to
users visiting the site where your game is located.

There are have been attempts to port Lua to JavaScript, but the results aren't
perfect. Depending on how it's done, there are either missing features (like
coroutines) or significant performance hits.

The version of Lua running in Aroma is the real Lua. It's fast, and everything
works as expected.

There are a couple caveats though. First, Native Client is only supported by
Google Chrome. Second, Native Client applications are only allowed to run if
they are installed as an application from the Chrome Web Store.

Even with these two minor issues, Native Client still makes a great platform
to develop games in.

  [2]: http://www.lua.org

### Enable Native Client Everywhere

By default Chrome restricts where Native Client applications can run. This
default setting only lets them run when they are installed as an application
from the Chrome Web Store.

This is fine for when you want to distribute your game, but it's not a good way
to do development. For this reason, we will enable Native Client everywhere.

To do this, type `chrome://flags` into your address bar and go there.

Scroll down until you find the entry for **Native Client**:

<img src="$root/img/tutorial-1.png" alt="Native Client" />

Just click enable, then restart your browser as directed.

### Testing Locally

There is one more restriction Native Client puts on us. When testing your game
locally, the page it runs on must be served by a web server. It can not be
opened from your file system.

You must run a web server locally on your computer, then place all of your game
resources somewhere accessible.

*More information about this can be found
[here](https://developers.google.com/native-client/devguide/devcycle/running#Local).*


## Creating a Test Game

### Downloading & Installing

Start by downloading the latest version:

<div><a href="$root/bin/aroma-$version.zip">aroma-$version.zip</a></div>

Inside the zip file, you need the following files to be uploaded alongside your
game, so place them in a folder to begin with:

 * `aroma.nmf`
 * `aroma_i686.nexe`
 * `aroma_x86_64.nexe`
 * `js/aroma.js`

### Hello World

For the sake of the tutorial, we will create a small *Hello World* demo. Add a
file called `main.lua` to the folder:

    ```lua
    -- what is going on
    function aroma.draw()
      aroma.graphics.print("Hello World!", 10, 10)
    end
    ```

This simple example will just write `"Hello World!"` to the Aroma frame at
position 10, 10.

Next we need to create the HTML page that will be the container for our game.
In the same folder create `index.html`:

    ```html
    <!DOCTYPE HTML>
    <html lang="en">
    <head>
      <script type="text/javascript" src="js/aroma.js"></script>
    </head>
    <body>
      <div id="game_container"></div>
      <script type="text/javascript">
        var game = new Aroma("game_container", 640, 480);
      </script>
    </body>
    </html>
    ```

A couple interesting things are happening here. But first, let's load it and
see what happens.

If we navigate to the URL of our page right, you will see a nice loading
animation as aroma loads the frame. Then our Lua code is loaded from `main.lua`
and `Hello World!` should appear in the frame.

So far, the simple HTML page above loads the `aroma.js` dependency and creates
an [Aroma][5] object. The first argument is the id of the HTML element where the
game is placed. The second and third are the size of the game frame in pixels.

That's it! You're now ready to start making games using the [Lua API][4].


## Porting LÖVE Games

One of the advantages of Aroma is that it implements parts of the LÖVE API, an
existing game framework. The simplest way to port you game is to just assing
`aroma` to `love` at the top of your game:

    ```lua
    -- main.lua
    love = aroma

    function love.draw()
      love.graphics.print("Hello from Aroma!", 10, 10)
    end
    ```
*In the future Aroma hopes to implement the whole LÖVE API, but for the time
being only what is listed in the [Lua API Manual][4] is supported.*

## Next Steps

 * [Lua Reference][4]
 * [JavaScript Reference][3]

Check out the project on GitHub, <https://github.com/leafo/aroma>. Fork the code and post issues!

<div class="github-buttons">
<iframe src="http://markdotto.github.com/github-buttons/github-btn.html?user=leafo&repo=aroma&type=watch&count=true" allowtransparency="true" frameborder="0" scrolling="0" width="110px" height="20px"></iframe>
<br />
<iframe src="http://markdotto.github.com/github-buttons/github-btn.html?user=leafo&repo=aroma&type=fork&count=true" allowtransparency="true" frameborder="0" scrolling="0" width="95px" height="20px"></iframe>
</div>

Post about aroma on Twitter:

<a href="https://twitter.com/share" class="twitter-share-button" data-url="http://leafo.net/aroma/" data-text="Aroma - The Native Client Game engine powered by Lua" data-count="horizontal" data-via="moonscript">Tweet</a><script type="text/javascript" src="//platform.twitter.com/widgets.js"></script>

Or follow the author on Twitter for updates: [@moonscript](http://twitter.com/moonscript).

 [3]: ./js_reference.html
 [4]: ./reference.html
 [5]: ./js_reference.html#aroma.js.Aroma


