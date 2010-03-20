
-- todo framebuffer object

require "aroma"
local win, canvas = aroma.window("game", 600, 600)

local i = canvas.image("hi.png")
local fbo = canvas.framebuffer(256, 256)

while win.running do
	fbo:render(function(w, h)
		canvas:view2d(0,0, w, h)
		canvas:clear(128,128,128);
		--i:draw(0,0)
		canvas:rect(0, 0, 200, 200, 255,255,255)
	end)
	canvas:view2d(0,0, 600, 600)
	canvas:clear(0,0,0);

	fbo:draw(10, 10)

	canvas:rect(math.sin(canvas.time*3) * 300 + 300,0, 100, 100, {255,0,0, 64})

	win.running = canvas:flush() and not win.keyDown(win.key.esc)
end

