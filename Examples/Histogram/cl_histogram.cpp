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

#include <fstream>
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#include "cl_histogram.hpp"

cl_histogram::cl_histogram(
	bool gpu,
	unsigned int platform,
	unsigned int device)
{
	// setup devices_ and context_
	platform_used_ = platform;
	std::vector<cl::Platform> platforms;
	err_ = cl::Platform::get(&platforms);
	err_ = platforms[platform_used_].getDevices(
		(gpu) ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU,
		&devices_);
	for (int i = 0; i < devices_.size(); ++i) {
		std::cout << "device name [" << i << "] : ";
		std::cout << devices_[i].getInfo<CL_DEVICE_NAME>() << std::endl;
	}
	device_used_ = device;
	std::cout << "device used     : " << device_used_ << std::endl;
	cl_context_properties properties[] = {
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)(platforms[0])(),
		0
	};
	context_ = cl::Context(
		(gpu) ? CL_DEVICE_TYPE_GPU : CL_DEVICE_TYPE_CPU,
		properties);
	devices_ = context_.getInfo<CL_CONTEXT_DEVICES>();
	queue_ = cl::CommandQueue(context_, devices_[device_used_], 0, &err_);
	cl_work_group_size_ =
		devices_[device_used_].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
	std::cout << "work group size : " << cl_work_group_size_ << std::endl;
}

cl_histogram::~cl_histogram() {}

void cl_histogram::init(const std::string& cl_file) {
	std::ifstream ifs(cl_file);
	if (!ifs.is_open())
		throw std::runtime_error("could not open file : " + cl_file);
	std::string kernel_source(
		(std::istreambuf_iterator<char>(ifs)),
		std::istreambuf_iterator<char>());
	cl::Program::Sources source(
		1,
		std::make_pair(kernel_source.c_str(), kernel_source.size()));
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

void cl_histogram::setup(const std::pair<unsigned int, unsigned int>& s) {
	mdx_ = s.first;
	mdy_ = s.second;
	total_size_ = mdx_ * mdy_;
	cl_num_groups_ =
		(((mdx_ * mdy_) + cl_work_group_size_ - 1) / cl_work_group_size_);
}

void cl_histogram::prepare(const std::vector<uint8_t>& input) {
	if (input.size() != total_size_)
		throw std::runtime_error("input buffer size != image size!");
	kernel_histogram_init_ = cl::Kernel(program_, "histogram_init", &err_);
	kernel_histogram_partial_ = cl::Kernel(program_, "histogram_partial", &err_);
	kernel_histogram_reduce_ = cl::Kernel(program_, "histogram_reduce", &err_);
	cl_buffer_image_ = cl::Buffer(
		context_,
		CL_MEM_READ_WRITE | CL_MEM_USE_HOST_PTR,
		sizeof(uint8_t) * total_size_,
		(void*)&input[0],
		&err_);
	cl_buffer_histogram_partial_ = cl::Buffer(
		context_,
		CL_MEM_READ_WRITE,
		sizeof(cl_uint) * 256 * cl_num_groups_);
	cl_buffer_histogram_ = cl::Buffer(
		context_,
		CL_MEM_READ_WRITE,
		sizeof(cl_uint) * 256);
}

boost::posix_time::time_duration cl_histogram::run(
	std::vector<unsigned int>& output)
{
	auto start = boost::posix_time::microsec_clock::universal_time();
	kernel_histogram_init_.setArg(0, cl_buffer_histogram_partial_);
	queue_.finish();
	err_ = queue_.enqueueNDRangeKernel(
		kernel_histogram_init_,
		cl::NullRange,
		cl::NDRange(cl_work_group_size_, cl_num_groups_),
		cl::NullRange,
		nullptr,
		&event_);
	kernel_histogram_partial_.setArg(0, cl_buffer_image_);
	kernel_histogram_partial_.setArg(1, cl_buffer_histogram_partial_);
	queue_.finish();
	err_ = queue_.enqueueNDRangeKernel(
		kernel_histogram_partial_,
		cl::NullRange,
		cl::NDRange(cl_work_group_size_, cl_num_groups_),
		cl::NullRange,
		nullptr,
		&event_);
	queue_.finish();
	kernel_histogram_reduce_.setArg(0, cl_buffer_histogram_partial_);
	kernel_histogram_reduce_.setArg(1, cl_num_groups_);
	kernel_histogram_reduce_.setArg(2, cl_buffer_histogram_);
	queue_.finish();
	err_ = queue_.enqueueNDRangeKernel(
		kernel_histogram_reduce_,
		cl::NullRange,
		cl::NDRange(256),
		cl::NullRange,
		nullptr,
		&event_);
	auto end = boost::posix_time::microsec_clock::universal_time();
	if (output.size() != 256)
		output.resize(256);
	queue_.enqueueReadBuffer(
		cl_buffer_histogram_,
		CL_TRUE,
		0,
		output.size() * sizeof(cl_uint),
		&output[0]);
	queue_.finish();
	return (end - start);
}