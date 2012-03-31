local a = aroma.audio

local bg = a.newSource("game/theme.ogg", "streaming")
local effect = a.newSource("game/start.ogg")

bg:setLooping(true)
bg:play()

function aroma.keypressed(key)
  if key == " " then
    effect:play()
  elseif key == "q" then
    bg:stop()
  elseif key == "w" then
    bg:play()
  end
end

