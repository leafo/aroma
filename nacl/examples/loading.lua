local g = aroma.graphics
local audio = aroma.audio

local start = aroma.timer.getTime()

nacl.prefetch {
  "game/world.lua",
  "game/main.lua",
  "game/another.lua",
  image = {
    "game/hi.png",
    "game/tiles.png"
  },
  sound = {
    "volcanox/sound/start.wav",
    "volcanox/sound/hit_me.wav"
  },
  music = {
    "volcanox/sound/theme.ogg"
  }
}

require "game.world"
require "game.world"
require "game.main"
require "game.another"

local i1 = g.newImage"game/hi.png"
local i2 = g.newImage"game/tiles.png"

local a_bg = audio.newSource "volcanox/sound/theme.ogg"
local a_start = audio.newSource "volcanox/sound/start.wav", "static"
local a_hit_me = audio.newSource "volcanox/sound/hit_me.wav", "static"

print(aroma.timer.getTime() - start)

function aroma.draw()
  g.draw(i1, 10, 10)
end
