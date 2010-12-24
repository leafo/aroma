
#include "framebuffer.h"

int FrameBuffer::_new(lua_State *l) {
	int width = luaL_checkinteger(l, -2);
	int height = luaL_checkinteger(l, -1);

	FrameBuffer *self = newuserdata(FrameBuffer);
	cout << "creating framebuffer " << $(width) << $(height) << endl;
	self->width = width;
	self->height = height;
	self->tex.create(width, height, NULL);

	glGenFramebuffersEXT(1, &self->fbo);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, self->fbo);

	// attach texture to fbo
	glFramebufferTexture2DEXT(GL_FRAMEBUFFER_EXT,
			GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D,
			self->tex.texid, 0);
	
	// create depth buffer
	glGenRenderbuffersEXT(1, &self->depth);
	glBindRenderbufferEXT(GL_RENDERBUFFER_EXT, self->depth);
	glRenderbufferStorageEXT(GL_RENDERBUFFER_EXT,
			GL_DEPTH_COMPONENT24, width, height);

	// attach depth to fbo
	glFramebufferRenderbufferEXT(GL_FRAMEBUFFER_EXT,
			GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, self->depth);

	// see if it worked
	GLenum status = glCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
	if (status != GL_FRAMEBUFFER_COMPLETE_EXT) {
		cout << "Failed to setup framebuffer" << endl;
	}

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	if (luaL_newmetatable(l, "FrameBuffer")) {
		lua_newtable(l); // the index table
		setfunction("bind", FrameBuffer::_bind);
		setfunction("release", FrameBuffer::_release);
		setfunction("render", FrameBuffer::_render);
		setfunction("bindTex", FrameBuffer::_bindTex);
		setfunction("draw", FrameBuffer::_draw);

		lua_setfield(l, -2, "__index");
	}
	lua_setmetatable(l, -2);

	return 1;
}

int FrameBuffer::_render(lua_State *l) {
	FrameBuffer *self = getself(FrameBuffer);

	lua_pushvalue(l, 2); // make sure the function is on top
	lua_pushinteger(l, self->width);
	lua_pushinteger(l, self->height);


	glPushAttrib(GL_VIEWPORT_BIT);
	glViewport(0, 0, self->width, self->height);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, self->fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // don't do this

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, self->width, self->height, 0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	lua_call(l, 2, 0);

	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);

	glPopAttrib();

	return 0;
}

int FrameBuffer::_bind(lua_State *l) {
	FrameBuffer *self = getself(FrameBuffer);
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, self->fbo);
	return 0;
}

int FrameBuffer::_release(lua_State *l) {
	glBindFramebufferEXT(GL_FRAMEBUFFER_EXT, 0);
	return 0;
}

int FrameBuffer::_bindTex(lua_State *l) {
	FrameBuffer *self = getself(FrameBuffer);
	self->tex.bind();
	return 0;
}

int FrameBuffer::_draw(lua_State *l) {
	FrameBuffer *self = getself(FrameBuffer);

	Point size = {self->width, self->height};
	if (lua_gettop(l) == 5) { size = Point::pop(l); }
	Point origin = Point::pop(l);

	// self->tex.draw(p.x, p.y); // this is upside down
	glBindTexture(GL_TEXTURE_2D, self->tex.texid);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 1);
		glVertex2d(origin.x, origin.y);
		glTexCoord2f(1, 1);
		glVertex2d(origin.x + size.x, origin.y);
		glTexCoord2f(1, 0);
		glVertex2d(origin.x + size.x, origin.y + size.y);
		glTexCoord2f(0, 0);
		glVertex2d(origin.x, origin.y + size.y);
	glEnd();
	
	return 0;
}


