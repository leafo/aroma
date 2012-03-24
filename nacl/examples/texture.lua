local g = aroma.graphics

local x, t = 0, 0
local tex = g.newImage('game/hi.png')

function aroma.update(dt)
  t = t + dt*3
  x = (math.sin(t) + 1)*100
end

function aroma.draw()
  g.draw(tex, x,x)
end
