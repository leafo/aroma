
require "aroma"

aroma(600 ,600) {
	ondraw = function(self)
		local i = self.image"res/hi.png"
		local fbo = self.framebuffer(256,256)

		self.ondraw = function()
			fbo:render(function(w,h)
				self:viewport{w,h}
				self:clear(128,128,128)

				-- draw something in the fbo
				local whole = {0,0, i:size()}

				i:blit(whole, 0,0, w/2, h/2)
				i:blit(whole, w/2, 0, w/2, h/2)

				i:blit(whole, 0, h/2, w/2, h/2)
				i:blit(whole, w/2, h/2, w/2, h/2)

			end)

			self:viewport(1)
			self:clear(0,0,0)

			for i = -10,400,90 do fbo:draw(i,i) end

		end
	end
}

