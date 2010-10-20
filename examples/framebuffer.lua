
-- todo framebuffer object

require "aroma"
local win, canvas = aroma.window("game", 600, 600)

local i = canvas.image("hi.png")
local fbo = canvas.framebuffer(256, 256)

while win.running do
	fbo:render(function(w, h)
		canvas:view2d(0,0, w, h)
		canvas:clear(128,128,128);
		-- i:draw(,0)
		local whole = {0,0, i:size()}

		i:blit(whole, 0,0, w/2, h/2)
		i:blit(whole, w/2, 0, w/2, h/2)

		i:blit(whole, 0, h/2, w/2, h/2)
		i:blit(whole, w/2, h/2, w/2, h/2)
	end)

	canvas:view2d(0,0, 600, 600)
	canvas:clear(0,0,0);

	for i = -10,400,90 do fbo:draw(i,i) end

	win.running = canvas:flush() and not win.keyDown(win.key.esc)
end

