
package.path = package.path .. ';../lib/?.lua'

require "aroma"
require "ply"

local win, canvas = aroma.window("3d test", 640, 480);
canvas:clearColor(34,34,34)
canvas:view3d(60)

local dragging = false
local mx, my
local theta_x, theta_y = 0, 0

-- local m = canvas.mesh({
-- 	-1, 1,
-- 	 1, 1,
-- 	 1, -1,
-- 	-1, -1,
-- 	}, 2, "quads")

local ship = ply.parse"ship.ply"
local m = canvas.mesh(ship:getVertices(), 3)

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
		theta_x, theta_y = theta_x + dx / 180, theta_y + dy / 180
	end
	
	canvas:look(5*math.cos(theta_x), 5*math.sin(theta_x), 5, 0,0,1)
	m:render()

	win.running = canvas:flush() and not win.keyDown(win.key.esc)
end

