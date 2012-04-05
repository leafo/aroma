
local g = aroma.graphics
local other_font = g.newFont("32px monospace")

local t = aroma.timer

function aroma.draw()
  g.print("hello world", 10, 10)
  g.setColor(255,128,128)
  g.print(other_font, "here is fps: " .. t.getFPS(), 10, 70)
  g.setColor(255,255,255)
end

