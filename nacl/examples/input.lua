print "Click to focus, arrow keys move"
local g = aroma.graphics
local k = aroma.keyboard

local x, y = 100, 100
local speed = 200

function aroma.keypressed(key)
  if key == " " then
    print "Hello world!"
  end
end

function aroma.update(dt)
  local dx = k.isDown"left" and -1 or k.isDown"right" and 1 or 0
  local dy = k.isDown"up" and -1 or k.isDown"down" and 1 or 0

  x = x + dx * speed * dt
  y = y + dy * speed * dt
end

function aroma.draw()
  g.rectangle(x,y, 50, 50)
end
