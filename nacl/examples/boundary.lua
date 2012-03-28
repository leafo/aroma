local g = aroma.graphics

local x, t = 0, 0
local img = nil

function aroma.draw()
  if not img then
    img = g.newImage('game/hi.png')
  end
  g.draw(img, 10, 10)
end
