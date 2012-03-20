local g = aroma.graphics

local x, t = 0, 0

function aroma.update(dt)
  t = t + dt*3
  x = (math.sin(t) - 1)/2
end

function aroma.draw()
  g.rectangle(x,x,1,1)
end
