
#include "renderer.h"

namespace aroma {
	void Renderer::rect(float x1, float y1, float x2, float y2) {
		float colors[] = {
			1,1,1,
			1,1,1,
			1,1,1,
			1,1,1,
		};

		float verts[] = {
			x1,y1,
			x2,y1,
			x1,y2,
			x2,y2
		};

		// GLuint buffs[2];
		GLuint vert_buffer;
		glGenBuffers(1, &vert_buffer);

		glBindBuffer(GL_ARRAY_BUFFER, vert_buffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, verts, GL_STATIC_DRAW);

		GLuint P = default_shader->attr_loc("P");

		glEnableVertexAttribArray(P);
		glVertexAttribPointer(P, 2, GL_FLOAT, false, 0, 0);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDeleteBuffers(1, &vert_buffer);
	}

	Renderer::Renderer(GLContext *context) :
		context(context),
		default_shader(NULL)
	{
		context->set_renderer(this);
	}

	void Renderer::init() {
		log("init renderer\n");
		glClearColor(0.1, 0.1, 0.1, 1.0);

		const char *vertex_src =
			"attribute vec2 P;\n"
			"void main(void) {\n"
			"  gl_Position = vec4(P, 0.0, 1.0);\n"
			"}\n"
			;

		const char *fragment_src =
			"void main(void) {\n"
			"	 gl_FragColor = vec4(1,0,1,1);\n"
			"}\n"
			;

		default_shader = new Shader();
		default_shader->add(GL_VERTEX_SHADER, vertex_src);
		default_shader->add(GL_FRAGMENT_SHADER, fragment_src);
		default_shader->link();
	}

	void Renderer::draw() {
		glClear(GL_COLOR_BUFFER_BIT);
		default_shader->bind();
		rect(0,0, 1,1);
	}

	// called for every frame
	void Renderer::tick() {
		context->make_current();

		glViewport(0, 0, context->width(), context->height());

		draw();
		context->flush();
	}

	void Renderer::reshape(const int w, const int h) {
		context->resize(w, h);
		tick(); // why  tick here?
	}

}
