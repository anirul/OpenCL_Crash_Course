/*
 * Copyright (c) 2014, Frederic Dubouchet
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Calodox nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY Frederic Dubouchet ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL Frederic DUBOUCHET BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stdexcept>
#include <string>
#include <vector>
#ifdef __linux__
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif
#ifdef __APPLE__
#include <glut/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#endif
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "cl_video.hpp"
#include "glut_win.hpp"
#include "win_video.hpp"

win_video::win_video(const std::pair<unsigned int, unsigned int>& size,
					 const std::vector<char> initial_image,
					 std::function<bool (std::vector<char>&)> callback,
					 const std::string& cl_file,
					 bool color,
					 bool gpu,
					 unsigned int device) :
range_(size),
video_(nullptr),
color_(color),
gpu_(gpu),
device_(device),
texture_id_(0),
cl_file_(cl_file),
callback_(callback)
{
	current_image_ = initial_image;
	best_time_ = boost::posix_time::minutes(60);
}

win_video::~win_video() {}

// inherited from the i_win interface
void win_video::init() {
	glClearColor(0, 0, 0, 0);
	gluOrtho2D(-1, 1, -1, 1);
	glGenTextures(1, &texture_id_);
	video_ = new cl_video(gpu_, device_);
	video_->init(cl_file_);
	video_->setup(range_, (color_) ? 4 : 1);
}

void win_video::display() {
	glClear(GL_COLOR_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture_id_);
	{
		glPushMatrix();
		glBegin(GL_QUADS);
		{
			glTexCoord2f(0, 1);
			glVertex2f(-1, 1);
			glTexCoord2f(1, 1);
			glVertex2f(1, 1);
			glTexCoord2f(1, 0);
			glVertex2f(1, -1);
			glTexCoord2f(0, 0);
			glVertex2f(-1, -1);
		}
		glEnd();
		glPopMatrix();
	}
	glDisable(GL_TEXTURE_2D);
	glFlush();
	glutPostRedisplay();
}

void win_video::idle() {
	glFinish();
	if (video_) {
		boost::posix_time::time_duration actual_time;
		// get the next frame
		callback_(current_image_);
		video_->prepare(current_image_);
		actual_time = video_->run(current_image_);
		if (actual_time < best_time_) best_time_ = actual_time;
		std::cout
		<< "\rCompute time    : " << actual_time
		<< " best " << best_time_;
		std::cout.flush();
	}
	glBindTexture(GL_TEXTURE_2D, texture_id_);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (color_) {
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_RGBA,
					 range_.first,
					 range_.second,
					 0,
					 GL_BGRA,
					 GL_UNSIGNED_BYTE,
					 &current_image_[0]);
	} else {
		glTexImage2D(GL_TEXTURE_2D,
					 0,
					 GL_LUMINANCE,
					 range_.first,
					 range_.second,
					 0,
					 GL_LUMINANCE,
					 GL_UNSIGNED_BYTE,
					 &current_image_[0]);
	}
	glFinish();
}

void win_video::reshape(int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, w, h);
	glMatrixMode(GL_MODELVIEW);
	glFinish();
}

void win_video::mouse_event(int button, int state, int x, int y) {}

void win_video::mouse_move(int x, int y) {}

void win_video::keyboard(unsigned char key, int x, int y) {}

void win_video::finish() {
	if (video_) {
		delete video_;
		video_ = nullptr;
	}
	std::cout << std::endl;
}
