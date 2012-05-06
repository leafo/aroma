# aroma v0.0.1
#### a game engine
Leaf Corcoran, 2012

<http://leafo.net/aroma/>

## About

Aroma is game creation framework/platform that targets [Chrome's Native
Client][1]. It lets you create games that can be distributed through the Chrome
Web Store.

Aroma games are written in the Lua programming language.

Aroma implements the [LÖVE][2] API in order to make porting existing games
easy.

## How it works

Upload Aroma's `.nexe` binaries, `.nmf` file, and javascript support to a
directory. Then upload your entire game (Lua code, images, and audio) alongside
it and write a simple html file.

See the [tutorial][3] for a more detailed guide.

Aroma facilitates resource loading for you. It also handles drawing to an
OpenGL powered canvas. Aroma will reuse features of the browser whenever
possible, for things like decoding images, playing audio and downloading code.

## Dependencies

* [Lua 5.1](http://lua.org)
* [Lua CJSON](http://www.kyne.com.au/~mark/software/lua-cjson.php)

The build system is [tup](http://gittup.org/tup/index.html).

  [1]: https://developers.google.com/native-client/
  [2]: http://love2d.org
  [3]: http://leafo.net/aroma/tutorial.html

