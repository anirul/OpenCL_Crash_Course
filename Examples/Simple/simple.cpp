/*
* Copyright (c) 2012-2015, Frederic Dubouchet
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

#include <iostream>
#include <fstream>
#include <vector>
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 210
#include <CL/cl2.hpp>
#include <chrono>
#include <random>

using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::system_clock;

std::string kernel_source = R"cl(
// very simple kernel

__kernel void simple(
      __global float* in1,
      __global float* in2,
      __global float* out)
{
   uint pos = get_global_id(0);
   out[pos] = in1[pos] * in2[pos];
}
)cl";

const unsigned int platform_id = 0;
const unsigned int device_id = 0;

int main(int ac, char** av) {
	size_t vector_size = 1024 * 1024;
	if (ac > 1)	vector_size = atoi(av[1]);
	try {
		// get the device (here the GPU)
		std::vector<cl::Platform> platforms;
		cl::Platform::get(&platforms);
		std::cout << "platform size  : " << platforms.size() << std::endl;
		for (auto& platform : platforms)
		{
			std::cout 
				<< "using platform : " 
				<< platform.getInfo<CL_PLATFORM_NAME>()
				<< std::endl;
			std::vector<cl::Device> devices_;
			platform.getDevices(CL_DEVICE_TYPE_ALL, &devices_);
			std::cout << "device size    : " << devices_.size() << std::endl;
			for (auto& device : devices_)
			{
				std::cout 
					<< "using device   : "
					<< device.getInfo<CL_DEVICE_NAME>()
					<< std::endl;
				cl_context_properties properties[] = {
					CL_CONTEXT_PLATFORM,
					(cl_context_properties)(platform)(),
					0
				};
				cl::Context context_ = 
					cl::Context(CL_DEVICE_TYPE_ALL, properties);
				cl::CommandQueue queue_(context_, device, 0 , nullptr);
				// compile
				cl::Program::Sources source(1, kernel_source);
				cl::Program program_(context_, source);
				try {
					program_.build(devices_);
				}
				catch (cl::Error& er) {
					std::cerr << "Exception(CL)  : " << er.what() << std::endl;
					if (er.err() == CL_BUILD_PROGRAM_FAILURE) {
						// Determine the size of the log
						size_t log_size;
						clGetProgramBuildInfo(
							program_(), 
							device(), 
							CL_PROGRAM_BUILD_LOG, 
							0, 
							NULL, 
							&log_size);

						// Allocate memory for the log
						char* log = (char*)malloc(log_size);

						// Get the log
						clGetProgramBuildInfo(
							program_(), 
							device(), 
							CL_PROGRAM_BUILD_LOG, 
							log_size, 
							log, 
							NULL);

						// Print the log
						printf("%s\n", log);
					}
					continue;
				}
				// create the kernel
				cl::Kernel kernel_(program_, "simple");
				// prepare the buffers
				std::vector<float> in1(vector_size);
				std::vector<float> in2(vector_size);
				std::vector<float> out(vector_size);
				for (auto& ite : in1) ite = (float)rand();
				for (auto& ite : in2) ite = (float)rand();
				cl::Buffer buf_in1_ = cl::Buffer(
					context_,
					CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
					sizeof(cl_float) * in1.size(),
					in1.data());
				cl::Buffer buf_in2_ = cl::Buffer(
					context_,
					CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
					sizeof(cl_float) * in2.size(),
					in2.data());
				cl::Buffer buf_out_ = cl::Buffer(
					context_,
					CL_MEM_WRITE_ONLY,
					sizeof(cl_float) * out.size());
				// set the arguments
				kernel_.setArg(0, buf_in1_);
				kernel_.setArg(1, buf_in2_);
				kernel_.setArg(2, buf_out_);
				// wait to the command queue to finish before proceeding
				queue_.finish();
				auto start = system_clock::now();
				// run the kernel
				queue_.enqueueNDRangeKernel(
					kernel_,
					cl::NullRange,
					cl::NDRange(vector_size),
					cl::NullRange);
				queue_.finish();
				auto end = system_clock::now();
				// get the result out
				queue_.enqueueReadBuffer(
					buf_out_,
					CL_TRUE,
					0,
					vector_size * sizeof(float),
					&out[0]);
				queue_.finish();
				std::cout << "Computing time : ";
				std::cout << duration_cast<microseconds>(end - start).count();
				std::cout << "us" << std::endl;
			}
		}
	} catch (cl::Error& er) {
		std::cerr << "Exception(CL)  : " << er.what();
		std::cerr << "(" << er.err() << ")" << std::endl;
	} catch (std::exception& ex) {
		std::cerr << "Exception(STL) : " << ex.what() << std::endl;
	}
	return 0;
}
