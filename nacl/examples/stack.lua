local g = aroma.graphics
local s = 100

function r()
  g.rectangle(0,0, s,s)
end

function flower()
  for i = 0, 2 do
    g.push()
    g.translate(s/2, s/2)
    g.rotate(i/10 * math.pi*2)
    g.translate(-s/2, -s/2)
    r()
    g.pop()
  end
end

function aroma.draw()
  g.setColor(128,255,128)
  r()

  g.setColor(255,128,255)
  g.push()
  g.translate(20, 20)
  r()
  g.pop()

  g.setColor(128,255,255)
  g.push()
  g.translate(10, 150)
  for i = 0, 5 do
    g.push()
    g.scale(0.3, 0.3)
    flower()
    g.pop()
    g.translate(50, 0)
  end
  g.pop()

end
