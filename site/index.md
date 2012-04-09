
Aroma is game creation framework/platform that targets both [Chrome's Native
Client][1] and desktops. It lets you run your games in the browser with Chrome
or distribute it as a standalone executable on Linux, Windows and OSX.

Aroma games are written in the [Lua][2] programming language. Aroma implements
parts of the [LÖVE][3] Lua API, so if you've got a game already written it's
easy to port.

  [1]: https://developers.google.com/native-client/
  [2]: https://lua.org
  [3]: https://love2d.org

<div>$index</div>

## How It Works

Native client files are distributed as `.nexe` binaries where are compiled for
both 64-bit and 32-bit computers. Aroma comes as a compiled `.nexe` file that's
ready to use after uploading to your server.

### Reusing Your Browser

Aroma comes with some support JavaScript that must be uploaded alongside your
game. The JavaScript facilitiates things like loading code, loading images, and
rendering fonts.

In order to keep the `.nexe` binary small, things that can already be
accomplished by your browser are handled there. Thing like rendering fonts and
decoding images are handled by the `canvas` tag, which functions as an
invisible buffer.

### Asynchronous Loading

Aroma loads all of your game code from the web. You typically will upload your
code alongside the `.nexe` files. This enables you to quickly deploy changes
without having to recompile anything.

If you've ever done web programming you're probably familiar with making
asynchronous requests to a server to fetch resources by providing some sort of
a callback function. On the other hand, when loading code in Lua typically
write something synchronous like:

    ```lua
    require "mygame.enemy"
    ```

Aroma translates this call into an asynchronous request to the browser,
blocking the execution of the game until the resource has been fetched.
Combined with the caching provided by the browser, this makes loading code from
the web pleasant.

## Download

## Getting Started

## Reference
