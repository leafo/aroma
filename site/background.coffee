
set_background = (canvas) ->
  document.body.style.backgroundImage = "url(#{canvas.toDataURL()})"

canvas = document.createElement "canvas"
canvas.width = 32
canvas.height = 32

ctx = canvas.getContext "2d"
ctx.fillStyle = "#F1F1F1"

ctx.fillRect 0,0, 32,32

ctx.save()

ctx.translate 16, 16
ctx.rotate Math.PI/4
ctx.scale 12,12

ctx.fillStyle = "#E3E3E3"
ctx.fillRect -1, -1, 2, 2

ctx.restore()

setTimeout (-> set_background canvas), 0

