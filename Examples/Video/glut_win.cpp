/*
 * Copyright (c) 2012, Frederic Dubouchet
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the CERN nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Frederic Dubouchet ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Frederic DUBOUCHEDT BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdexcept>
#include <string>
#ifdef __linux__
#include <GL/glut.h>
#endif
#ifdef __APPLE__
#include <glut/glut.h>
#endif
#ifdef WIN32
#include <GL/glut.h>
#endif
#include "glut_win.hpp"

static void init() {
	glut_win::instance()->pwin_->init();
}

static void finish() {
	glut_win::instance()->pwin_->finish();
}

static void display() {
	static bool s_first = true;
	if (s_first) {
		s_first = false;
		init();
	} else {
		glut_win::instance()->pwin_->display();
	}
	glutSwapBuffers();
}

static void keyboard(unsigned char key, int x, int y) {
	switch(key) {
		case 27 :
			finish();
			exit(0);
			break;
	}
	glut_win::instance()->pwin_->keyboard(key, x, y);
}

static void reshape(int w, int h) {
	glut_win::instance()->pwin_->reshape(w, h);
}

static void idle() {
	glut_win::instance()->pwin_->idle();
	glutPostRedisplay();
}

static void mouse_move(int x, int y) {
	glut_win::instance()->pwin_->mouse_move(x, y);
}

static void mouse_event(int button, int state, int x, int y) {
	glut_win::instance()->pwin_->mouse_event(button, state, x, y);
}

glut_win* glut_win::instance_ = nullptr;

glut_win* glut_win::instance(
	const std::string& name,
	const std::pair<unsigned int, unsigned int>& range, 
	i_win* windesc, 
	bool fullscreen)
{
	if (!instance_)
		instance_ = new glut_win(name, range, windesc, fullscreen);
	return instance_;
}

glut_win* glut_win::instance() {
	if (instance_) return instance_;
	throw std::runtime_error(std::string("glut_win was not initialized!"));
}

glut_win::glut_win(
	const std::string& name,
	const std::pair<unsigned int, unsigned int>& range, 
	i_win* windesc, 
	bool fullscreen) :
		fullscreen_(fullscreen),
		pwin_(windesc)
{
	if (!pwin_) throw std::runtime_error(std::string("Window class == NULL"));
	int ac = 0;
	glutInit(&ac, NULL);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	if (!fullscreen_)
		glutInitWindowSize(range.first, range.second);
	glutCreateWindow(name.c_str());
	if (fullscreen_)
		glutFullScreen();
}

glut_win::~glut_win() { }

void glut_win::run()
{
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMotionFunc(mouse_move);
	glutMouseFunc(mouse_event);
	glutIdleFunc(idle);
	glutMainLoop();
}

