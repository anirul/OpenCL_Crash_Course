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
#ifndef CL_VIDEO_HEADER_DEFINED
#define CL_VIDEO_HEADER_DEFINED

#include <string>

#include <CL/cl.hpp>

class cl_video {
private:
	cl::Image2D cl_image_input_;
	cl::Image2D cl_image_output_;
	cl::CommandQueue queue_;
	cl::Program program_;
	cl::Kernel kernel_;
	std::vector<cl::Device> devices_;
	cl::Context context_;
	unsigned int device_used_;
	cl_int err_;
	cl::Event event_;
	unsigned int mdx_;
	unsigned int mdy_;
	unsigned int nb_col_;
	unsigned int total_size_;
	cl::size_t<3> origin_;
	cl::size_t<3> region_;
public:
	cl_video(bool gpu, unsigned int device);
	virtual ~cl_video();
public:
	void init(const std::string& cl_file);
	void setup(const std::pair<unsigned int, unsigned int>& s,
			   unsigned int nb_col);
	void prepare(const std::vector<char>& input);
	boost::posix_time::time_duration run(std::vector<char>& output);
};

#endif // CL_VIDEO_HEADER_DEFINED