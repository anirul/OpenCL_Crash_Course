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

#include <stdio.h>
#include <iostream>
#include <vector>
#define __CL_ENABLE_EXCEPTIONS
#include <cl.hpp>
#ifdef __linux__
#include <GL/glx.h>
#endif
#include <boost/date_time/posix_time/posix_time.hpp>

#include "cl_floyd_warshall.h"

using namespace boost::posix_time;

cl_floyd_warshall::cl_floyd_warshall(bool gpu) {
	//setup devices_ and context_
	std::vector<cl::Platform> platforms;
	err_ = cl::Platform::get(&platforms);
	device_used_ = 0;
	err_ = platforms[0].getDevices(
		(gpu) ?	CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU,
		&devices_);
	cl_context_properties properties[] = {
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)(platforms[0])(),
		0
	};
	context_ = cl::Context(
		(gpu) ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU ,
		properties);
	devices_ = context_.getInfo<CL_CONTEXT_DEVICES>();
	queue_ = cl::CommandQueue(context_, devices_[device_used_], 0, &err_);
}

void cl_floyd_warshall::init(const std::string& cl_file) {
	FILE* file = fopen(cl_file.c_str(), "rt");
	if (!file) throw std::runtime_error("could not open file " + cl_file);
	const unsigned int BUFFER_SIZE = 1024 * 1024;
	size_t bytes_read = 0;
	std::string kernel__source = "";
	do {
		char temp[BUFFER_SIZE];
		memset(temp, 0, BUFFER_SIZE);
		bytes_read = fread(temp, sizeof(char), BUFFER_SIZE, file);
		kernel__source += temp;
	} while (bytes_read != 0);
	fclose(file);
	cl::Program::Sources source(
		1,
		std::make_pair(
			kernel__source.c_str(),
			kernel__source.size()));
	program_ = cl::Program(context_, source);
	try {
		err_ = program_.build(devices_);
	} catch (cl::Error er) {
		std::cerr << "build status    : "
		<< program_.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices_[0]) << std::endl;
		std::cerr << "build options   : "
		<< program_.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices_[0]) << std::endl;
		std::cerr << "build log       : "  << std::endl
		<< program_.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices_[0]) << std::endl;
		throw er;
	}
}

void cl_floyd_warshall::setup(const std::pair<unsigned int, unsigned int>& s) {
	mdx_ = s.first;
	mdy_ = s.second;
}

void cl_floyd_warshall::prepare(const std::vector<float>& in) {
	total_size_ = mdx_ * mdy_;
	if (in.size() != (total_size_))
		throw std::runtime_error("input buffer size != image size!");
	kernel_ = cl::Kernel(program_, "floyd_warshall_buffer", &err_);
	//initialize our CPU memory arrays, send them to the device and set the kernel_ arguments
	cl_buffer_mat_ = cl::Buffer(
		context_,
		CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
		sizeof(float) * total_size_,
		(void*)&in[0],
		&err_);
	cl_buffer_in_x_ = cl::Buffer(
        context_,
        CL_MEM_READ_WRITE,
        sizeof(float) * mdx_,
        NULL,
        &err_);
	cl_buffer_in_y_ = cl::Buffer(
        context_,
        CL_MEM_READ_WRITE,
        sizeof(float) * mdy_,
        NULL,
        &err_);
	queue_.finish();
	//set the arguements of our kernel_
	err_ = kernel_.setArg(0, cl_buffer_mat_);
	err_ = kernel_.setArg(1, cl_buffer_in_x_);
	err_ = kernel_.setArg(2, cl_buffer_in_y_);
	//Wait for the command queue_ to finish these commands before proceeding
	queue_.finish();
}

time_duration cl_floyd_warshall::run(std::vector<float>& mat) {
	ptime before;
	ptime after;
	assert(mdx_ == mdy_);
	time_duration total = seconds(0);
	if (mat.size() != total_size_)
		mat.resize(total_size_);
	cl::size_t<3> dst_origin;
	dst_origin[0] = 0;
	dst_origin[1] = 0;
	dst_origin[2] = 0;
	for (int i = 0; i < mdx_; ++i) {
		std::cout << "Compute generation [" << i + 1 << "/" << mdx_ << "]\r";
		std::cout.flush();
		before = microsec_clock::universal_time();
		// create the cl_buffer_in_x_ and cl_buffer_in_y_
		{ // cl_buffer_x_
			cl::size_t<3> src_origin;
			cl::size_t<3> region;
			src_origin[0] = i * sizeof(float);
			src_origin[1] = 0;
			src_origin[2] = 0;
			region[0] = sizeof(float);
			region[1] = mdx_;
			region[2] = 1;
			queue_.enqueueCopyBufferRect(
				cl_buffer_mat_,
				cl_buffer_in_x_,
				src_origin,
				dst_origin,
				region,
				mdx_ * sizeof(float),
				0,
				sizeof(float),
				0,
				nullptr,
				&event_);
		}
		{ // cl_buffer_y_
			queue_.enqueueCopyBuffer(
				cl_buffer_mat_,
				cl_buffer_in_y_,
				mdx_ * i * sizeof(float),
				0,
				mdx_ * sizeof(float),
				nullptr,
				&event_);
		}
		queue_.finish();
		// make the computation
		err_ = queue_.enqueueNDRangeKernel(
			kernel_,
			cl::NullRange,
			cl::NDRange(mdx_, mdy_),
			cl::NullRange,
			NULL,
			&event_);
		queue_.finish();
		after = microsec_clock::universal_time();
		total += (after - before);
	}
	std::cout << std::endl;
	err_ = queue_.enqueueReadBuffer(
         cl_buffer_mat_,
         CL_TRUE,
         0,
         sizeof(float) * total_size_,
         &mat[0],
         NULL,
         &event_);
	queue_.finish();
	return total;
}

