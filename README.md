# aroma v0.0.3
#### a game engine
Leaf Corcoran, 2012

<http://leafo.net/aroma/>
[Changelog](http://leafo.net/aroma/changelog.html)

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

## How To Build

Building can be a bit challenging, but here is a brief overview. I build Aroma
on Linux. It should work on OSX. If you're on Windows you're on your own! (But
if you figure it out, tell me and I'll update this guide)

The following dependencies are required before building:

 * [tup](http://gittup.org/tup/index.html) -- the build system
 * [git](http://git-scm.com) -- used in the 32 bit build script
 * [NaCL SDK](https://developers.google.com/native-client/sdk/download) -- Pepper 18
 * [moonscript-dev-1](http://moonscript.org/#source)
 * [CoffeeScript](http://coffeescript.org)
 * [xxd](http://linux.die.net/man/1/xxd)

*If you want to build the website (which you probably don't), you need
[sitegen](http://leafo.net/sitegen) and [lessphp](http://leafo.net/lessphp)*

After all the dependencies are set up, head into the `nacl` folder and run:

    $ ./installs_deps

This will download and extract Lua 5.1.2 and Lua CJSON.

Next, head to the root directory, set up tup and tell it to build!

    $ tup init
    $ tup upd

By default this will build the 64 bit version of Aroma. I've provided a
makefile in the `nacl` folder with a directive for building 32 bit, just run:

    $ make 32

If you want to make tup build 32 by default then edit `tup.config` in the root
directory, and uncomment the line:

    CONFIG_NACL_ARCH=i686

And finally, I use tup for this reason, run this command:

    $ tup monitor -a -f

This watches the filesystem and builds what's needed when you change something.

  [1]: https://developers.google.com/native-client/
  [2]: http://love2d.org
  [3]: http://leafo.net/aroma/tutorial.html

