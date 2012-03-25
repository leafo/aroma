local g = aroma.graphics
local tex = g.newImage('game/hi.png')
local r = 0

-- split entire image into quads
local count = 8
local s = tex:getWidth() / count

function q(x, y)
  return g.newQuad(x, y, s, s, tex:getWidth(), tex:getHeight())
end

function xy(i)
  return (i - 1) % count * s, math.floor((i - 1) / count) * s
end

local quads = {}
for i = 1, count^2 do
  table.insert(quads, q(xy(i)))
end

function aroma.update(dt)
  r = r + dt * 3
end

function aroma.draw()
  g.push()
  g.translate(20, 20)
  for i, q in ipairs(quads) do
    local x, y = xy(i)
    g.drawq(tex, q, x, y, r, 1, 1, s/2, s/2)
  end
  g.pop()
end
