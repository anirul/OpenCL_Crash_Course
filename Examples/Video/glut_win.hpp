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
 * DISCLAIMED. IN NO EVENT SHALL Frederic DUBOUCHET BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef GLUT_WIN_HEADER_DEFINED
#define GLUT_WIN_HEADER_DEFINED

#include <mutex>

class i_win {
public:
	virtual void init() = 0;
	virtual void display() = 0;
	virtual void idle() = 0;
	virtual void reshape(int w, int h) = 0;
	virtual void mouse_event(int button, int state, int x, int y) = 0;
	virtual void mouse_move(int x, int y) = 0;
	virtual void keyboard(unsigned char key, int x, int y) = 0;
	virtual void finish() = 0;
};

// window class
class glut_win {
	static glut_win* instance_;
	bool fullscreen_;
	glut_win(
		const std::string& name,
		const std::pair<unsigned int, unsigned int>& range,
		i_win* windesc,
		bool fullscreen = false);
public:
	i_win* pwin_;
	static glut_win* instance(
		const std::string& name,
		const std::pair<unsigned int, unsigned int>& range,
		i_win* windesc,
		bool fullscreen = false);
	static glut_win* instance();
	virtual ~glut_win();
	void run();
};

#endif // GLUT_WIN_HEADER_DEFINED
