/*
 * Copyright (c) 2015, Frederic Dubouchet
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
#ifndef CL_HISTOGRAM_HEADER_DEFINED
#define CL_HISTOGRAM_HEADER_DEFINED

#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

class cl_histogram {
private:
	cl::Image2D cl_buffer_image_;
	cl::Buffer cl_buffer_luminosity_;
	cl::Buffer cl_buffer_histogram_partial_;
	cl::Buffer cl_buffer_histogram_;
	cl::CommandQueue queue_;
	cl::Program program_;
	cl::Kernel kernel_luminosity_;
	cl::Kernel kernel_init_;
	cl::Kernel kernel_partial_;
	cl::Kernel kernel_reduce_;
	std::vector<cl::Device> devices_;
	cl::Context context_;
	unsigned int device_used_;
	unsigned int platform_used_;
	cl_uint cl_num_groups_;
	cl_uint cl_work_group_size_;
	cl_int err_;
	cl::Event event_;
	unsigned int mdx_;
	unsigned int mdy_;
	unsigned int total_size_;
public:
	cl_histogram(bool gpu, unsigned int platform = 0, unsigned int device = 0);
	virtual ~cl_histogram();
public:
	void init(const std::string& cl_file);
	void setup(const std::pair<unsigned int, unsigned int>& s);
	void prepare(const std::vector<uint8_t>& input);
	boost::posix_time::time_duration run(std::vector<unsigned int>& output);
};

#endif // CL_HISTOGRAM_HEADER_DEFINED