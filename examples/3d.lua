
require "aroma"

local win, canvas = Window("3d test", 640, 480);
canvas:clearColor(34,34,34)
canvas:view3d(60)

local dragging = false
local mx, my
local theta = 90 

local m = canvas.mesh({
	-1, 1,
	 1, 1,
	 1, -1,
	-1, -1,
	}, 2, "quads")

while win.running do
	-- print(canvas.mouse.x, canvas.mouse.y)
	

	-- figure out how to drag mouse
	if not dragging and canvas.mouse.left then
		dragging = true
		mx, my = canvas.mouse.x, canvas.mouse.y
	elseif dragging and not canvas.mouse.left then
		dragging = false
	elseif dragging then
		-- do the drag stuff
		local dx, dy = canvas.mouse.x - mx, canvas.mouse.y - my
		mx, my = canvas.mouse.x, canvas.mouse.y
		theta = theta + dx / 180
	end
	

	canvas:look(5*math.cos(theta), 5, 5*math.sin(theta), 0,0,0)
	-- m:render()
	canvas:sphere(1.0)

	win.running = canvas:flush() and not win.keyDown(win.key.esc)
end

