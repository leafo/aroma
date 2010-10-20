
-- todo framebuffer object

require "aroma"

local win, canvas = aroma.window("game", 640, 200)
canvas:clearColor(34,34,34)
canvas:view2d(0,0, 640, 200)

local shader = win.shader(
[[
	varying vec3 P;
	void main() {
		gl_Position = ftransform();
		gl_TexCoord[0] = gl_MultiTexCoord0;
		P = vec3(gl_ModelViewMatrix * gl_Vertex);
	}
]],
[[
	varying vec3 P;
	void main() {
		gl_FragColor = vec4(gl_TexCoord[0].st, 0.0, 1.0);
	}
]])


while win.running do

	shader:bind()
	local x = (math.sin(canvas.time*2)+1)*(640-160-20)/2+10
	canvas.rect(x, 20, x+160, 160, {255,255,255})
	shader:release()

	win.running = canvas:flush() and not win.keyDown(win.key.esc)
end

