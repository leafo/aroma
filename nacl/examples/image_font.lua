
local font = aroma.graphics.newImageFont("game/font.png",
  [[ abcdefghijklmnopqrstuvwxyz-1234567890!.,:;'"?$&]])

aroma.graphics.setFont(font)

function aroma.draw()
  aroma.graphics.print("hello world!", 10, 10)
  aroma.graphics.print([[ abcdefghijklmnopqrstuvwxyz-1234567890!.,:;'"?$&]], 10, 20)
end
