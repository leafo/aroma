local g = aroma.graphics

local r = 0
local tex = g.newImage('game/hi.png')

function aroma.update(dt)
  r = r + dt
end

function aroma.draw()
  local size = tex:getHeight()
  g.draw(tex, 100, 100, r, 0.5, 0.5, size/2, size/2)
end

