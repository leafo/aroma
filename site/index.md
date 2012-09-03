
## How It Works

Native Client files are distributed as `.nexe` binaries where are compiled for
both 64-bit and 32-bit computers. Aroma comes as a compiled `.nexe` file that's
ready to use after uploading to your server. All you need to do is upload your
game alongside Aroma.

### Reusing Your Browser

In order to keep the `.nexe` binary small, Aroma reuses your browser to
accomplish many tasks without depending on additional libraries. Things like
rendering fonts and decoding images are handled by the `canvas` tag. The
loading of game resources is transparently turned into HTTP requests to your
sever.

### Asynchronous Loading

Aroma loads all of your game code from the web. You typically will upload your
code alongside Aroma's `.nexe` and `.js` files. This enables you to quickly
deploy changes without having to recompile anything.

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

Aroma also has the ability to batch asynchronous requests together in order to
perform them faster.

### LÖVE API

 Aroma implements parts of the [LÖVE][3] Lua API, so if you've got a game
 already written it's easy to port.

  [1]: https://developers.google.com/native-client/
  [2]: https://lua.org
  [3]: https://love2d.org


## Get Started Today

Aroma is early in development but ready for testing. Get started with the
following resources:

 * Download the latest version: <a href="$root/bin/aroma-$version.zip" class="event_bottom_download">aroma-$version.zip</a>
 * Read the <a href="$root/tutorial.html">beginning tutorial</a>
 * <a href="$root/js_reference.html">JavaScript Reference</a>
 * <a href="$root/reference.html">Lua Reference</a>
 * <a href="$root/changelog.html">Changelog</a>

