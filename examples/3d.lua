
package.path = package.path .. ';../lib/?.lua'

require "aroma"
require "ply"

aroma(640, 480) {
	clear_color = {34,34,34},
	view3d = 60,
	ondraw = function(self)
		local dragging = false
		local mx, my
		local theta_x, theta_y = 0, 0

		local ship = ply.parse"res/ship.ply"
		local m = self.mesh(ship:getVertices(), 3)

		self.ondraw = function()
			if not dragging and self.mouse.left then
				dragging = true
				mx, my = self.mouse.x, self.mouse.y
			elseif dragging and not self.mouse.left then
				dragging = false
			elseif dragging then
				-- do the drag stuff
				local dx, dy = self.mouse.x - mx, self.mouse.y - my
				mx, my = self.mouse.x, self.mouse.y
				theta_x, theta_y = theta_x + dx / 180, theta_y + dy / 180
			end
			
			self:look(5*math.cos(theta_x), 5*math.sin(theta_x), 5, 0,0,1)
			m:render()
		end
	end
}


