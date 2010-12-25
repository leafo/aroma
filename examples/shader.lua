
require "aroma"

aroma(640, 200) {
	clear_color = {34,34,34},
	ondraw = function(self)
		local shader = self.shader([[
			varying vec3 P;
			void main() {
				gl_Position = ftransform();
				gl_TexCoord[0] = gl_MultiTexCoord0;
				P = vec3(gl_ModelViewMatrix * gl_Vertex);
			}
		]], [[
			varying vec3 P;
			void main() {
				gl_FragColor = vec4(gl_TexCoord[0].st, 0.0, 1.0);
			}
		]])

		self.ondraw = function()
			shader:bind()
			local x = (math.sin(self.time*2)+1)*(640-160-20)/2+10
			self:rect(x, 20, x+160, 160, {255,255,255})
			shader:release()
		end
	end
}

