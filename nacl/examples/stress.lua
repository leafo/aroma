local amount = 2000

local g = aroma.graphics
local img = g.newImage("game/tiles.png")

local tw = 16
local count = img:getWidth() / tw

g.setBackgroundColor(200, 200, 200)

local quads = {}
function blit_tile(i, x, y)
  local q = quads[i]
  if not q then
    q = g.newQuad(
      tw * (i % count),
      math.floor(tw / count),
      tw, tw,
      img:getWidth(), img:getHeight())

    quads[i] = q
  end

  g.drawq(img, q, x, y)
end

function aroma.draw()
  g.translate((g:getWidth()-tw)/2, (g:getHeight()-tw)/2)

  for k = 1, amount do
    local rad = math.sqrt(k)
    local d = rad*4
    blit_tile(k % 3, math.cos(rad)*d, math.sin(rad)*d)
  end
end


