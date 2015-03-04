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

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>
#ifdef __linux__
#include <GL/glx.h>
#endif
#include <boost/date_time/posix_time/posix_time.hpp>

#include "cl_video.hpp"

cl_video::cl_video(bool gpu,
				   unsigned int device) :
device_used_(device),
err_(0),
mdx_(0),
mdy_(0)
{
	//setup devices_ and context_
	std::vector<cl::Platform> platforms;
	err_ = cl::Platform::get(&platforms);
	err_ = platforms[0].getDevices((gpu) ?
								   CL_DEVICE_TYPE_GPU :
								   CL_DEVICE_TYPE_CPU,
								   &devices_);
	//	int t = devices_.front().getInfo<CL_DEVICE_TYPE>();
	int i = 0;
	for (auto device : devices_) {
		std::cout
		<< "device name [" << i << "] : "
		<< device.getInfo<CL_DEVICE_NAME>()
		<< std::endl;
		i++;
	}
	// take the last device (work around Intel GPU)
	device_used_ = device;
	std::cout << "device used     : " << device_used_ << std::endl;
	try {
		throw cl::Error(0, "cheat pass");
#if defined (__APPLE__) || defined(MACOSX)
		CGLContextObj kCGLContext = CGLGetCurrentContext();
		CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
		cl_context_properties props[] =
		{
			CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE,
			(cl_context_properties)kCGLShareGroup,
			0
		};
		context_ = cl::Context(CL_DEVICE_TYPE_GPU, props);
#else
#if defined WIN32 // Win32
		cl_context_properties props[] =
		{
			CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(),
			CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(),
			CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(),
			0
		};
		context_ = cl::Context(CL_DEVICE_TYPE_GPU, props);
#else
		//		throw cl::Error(0, "cheat pass");
		cl_context_properties props[] =
		{
			CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(),
			CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(),
			CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(),
			0
		};
		context_ = cl::Context(CL_DEVICE_TYPE_GPU, props);
#endif
#endif
	} catch (cl::Error er) {
		std::cerr
		<< "Warning         : could not attach GL and CL together!"
		<< std::endl;
		cl_context_properties properties[] = {
			CL_CONTEXT_PLATFORM,
			(cl_context_properties)(platforms[0])(),
			0
		};
		context_ = cl::Context((gpu) ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU,
							   properties);
		devices_ = context_.getInfo<CL_CONTEXT_DEVICES>();
	}
	queue_ = cl::CommandQueue(context_, devices_[device_used_], 0, &err_);
}

cl_video::~cl_video() {}

void cl_video::init(const std::string& cl_file) {
	std::ifstream ifs(cl_file);
	if (!ifs.is_open())
		throw std::runtime_error("could not open file : " + cl_file);
	std::string kernel__source((std::istreambuf_iterator<char>(ifs)),
							   std::istreambuf_iterator<char>());
	cl::Program::Sources source(1,
								std::make_pair(kernel__source.c_str(),
											   kernel__source.size()));
	program_ = cl::Program(context_, source);
	try {
		err_ = program_.build(devices_);
	} catch (cl::Error er) {
		std::cerr
		<< "build status    : "
		<< program_.getBuildInfo<CL_PROGRAM_BUILD_STATUS>(devices_[0])
		<< std::endl;
		std::cerr
		<< "build options   : "
		<< program_.getBuildInfo<CL_PROGRAM_BUILD_OPTIONS>(devices_[0])
		<< std::endl;
		std::cerr
		<< "build log       : "  << std::endl
		<< program_.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices_[0])
		<< std::endl;
		throw er;
	}
}

void cl_video::setup(const std::pair<unsigned int, unsigned int>& s,
					 unsigned int nb_col)
{
	mdx_ = s.first;
	mdy_ = s.second;
	nb_col_ = nb_col;
}

void cl_video::prepare(const std::vector<char>& input) {
	total_size_ = mdx_ * mdy_ * nb_col_;
	origin_[0] = 0;
	origin_[1] = 0;
	origin_[2] = 0;
	region_[0] = mdx_;
	region_[1] = mdy_;
	region_[2] = 1;
	if (input.size() != (total_size_))
		throw std::runtime_error("input buffer size != image size!");
	kernel_ = cl::Kernel(program_, "video_image", &err_);
	// initialize our CPU memory arrays, send them to the device and set
	// the kernel_ arguements
	cl::ImageFormat format;
	if (nb_col_ == 1)
		format = cl::ImageFormat(CL_INTENSITY, CL_UNORM_INT8);
	else
		format = cl::ImageFormat(CL_BGRA, CL_UNORM_INT8);
	cl_image_input_ = cl::Image2D(context_,
								  CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
								  format,
								  mdx_,
								  mdy_,
								  0,
								  (void*)&input[0],
								  &err_);
	cl_image_output_ = cl::Image2D(context_,
								   CL_MEM_WRITE_ONLY,
								   format,
								   mdx_,
								   mdy_,
								   0,
								   NULL,
								   &err_);
	queue_.finish();
	//set the arguements of our kernel_
	err_ = kernel_.setArg(0, cl_image_input_);
	err_ = kernel_.setArg(1, cl_image_output_);
	//Wait for the command queue_ to finish these commands before proceeding
	queue_.finish();
}

boost::posix_time::time_duration cl_video::run(std::vector<char>& output) {
	boost::posix_time::ptime before;
	boost::posix_time::ptime after;
	if (output.size() != total_size_)
		output.resize(total_size_);
	before = boost::posix_time::microsec_clock::universal_time();
	// make the computation
	err_ = queue_.enqueueNDRangeKernel(kernel_,
									   cl::NullRange,
									   cl::NDRange(mdx_, mdy_),
									   cl::NullRange,
									   NULL,
									   &event_);
	queue_.finish();
	after = boost::posix_time::microsec_clock::universal_time();
	err_ = queue_.enqueueReadImage(cl_image_output_,
								   CL_TRUE,
								   origin_,
								   region_,
								   0,
								   0,
								   (void*)&output[0]);
	queue_.finish();
	return (after - before);
}
