
-- todo framebuffer object

require "aroma"

local win, canvas = aroma.window("game", 600, 600)
canvas:clearColor(34,34,34)
canvas:view2d(0,0, 600, 600)

local shader = win.compileShader(
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
		// gl_FragColor = vec4(1.0, P.x/100.0, 0.0, 1.0);
		gl_FragColor = vec4(gl_TexCoord[0].st, 0.0, 1.0);
	}
]])


while win.running do

	shader:bind()
	canvas.rect(20,20, 100, 100, {255,255,255})
	shader:release()

	win.running = canvas:flush() and not win.keyDown(win.key.esc)
end

