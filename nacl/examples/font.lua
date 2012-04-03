
local g = aroma.graphics
local other_font = g.newFont("32px monospace")

function aroma.draw()
  g.print("hello world", 10, 10)
  g.setColor(255,128,128)
  g.print(other_font, "here is", 10, 70)
  g.setColor(255,255,255)
end

