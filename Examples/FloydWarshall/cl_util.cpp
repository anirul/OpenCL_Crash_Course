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

#include <stdio.h>
#include <iostream>
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

#include "cl_util.h"

std::ostream& operator<<(std::ostream& os, cl::Error err) {
	switch (err.err()) {
		case CL_BUILD_PROGRAM_FAILURE :
			os << "CL_BUILD_PROGRAM_FAILURE";
			break;
		case CL_COMPILER_NOT_AVAILABLE :
			os << "CL_COMPILER_NOT_AVAILABLE";
			break;
		case CL_DEVICE_NOT_AVAILABLE :
			os << "CL_DEVICE_NOT_AVAILABLE";
			break;
		case CL_DEVICE_NOT_FOUND :
			os << "CL_DEVICE_NOT_FOUND";
			break;
		case CL_IMAGE_FORMAT_MISMATCH :
			os << "CL_IMAGE_FORMAT_MISMATCH";
			break;
		case CL_IMAGE_FORMAT_NOT_SUPPORTED :
			os << "CL_IMAGE_FORMAT_NOT_SUPPORTED";
			break;
		case CL_INVALID_ARG_SIZE :
			os << "CL_INVALID_ARG_SIZE";
			break;
		case CL_INVALID_ARG_VALUE :
			os << "CL_INVALID_ARG_VALUE";
			break;
		case CL_INVALID_BINARY :
			os << "CL_INVALID_BINARY";
			break;
		case CL_INVALID_BUFFER_SIZE :
			os << "CL_INVALID_BUFFER_SIZE";
			break;
		case CL_INVALID_BUILD_OPTIONS :
			os << "CL_INVALID_BUILD_OPTIONS";
			break;
		case CL_INVALID_COMMAND_QUEUE :
			os << "CL_INVALID_COMMAND_QUEUE";
			break;
		case CL_INVALID_CONTEXT :
			os << "CL_INVALID_CONTEXT";
			break;
		case CL_INVALID_DEVICE :
			os << "CL_INVALID_DEVICE";
			break;
		case CL_INVALID_DEVICE_TYPE :
			os << "CL_INVALID_DEVICE_TYPE";
			break;
		case CL_INVALID_EVENT :
			os << "CL_INVALID_EVENT";
			break;
		case CL_INVALID_EVENT_WAIT_LIST :
			os << "CL_INVALID_EVENT_WAIT_LIST";
			break;
		case CL_INVALID_GL_OBJECT :
			os << "CL_INVALID_GL_OBJECT";
			break;
		case CL_INVALID_GLOBAL_OFFSET :
			os << "CL_INVALID_GLOBAL_OFFSET";
			break;
		case CL_INVALID_HOST_PTR :
			os << "CL_INVALID_HOST_PTR";
			break;
		case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR :
			os << "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
			break;
		case CL_INVALID_IMAGE_SIZE :
			os << "CL_INVALID_IMAGE_SIZE";
			break;
		case CL_INVALID_KERNEL_NAME :
			os << "CL_INVALID_KERNEL_NAME";
			break;
		case CL_INVALID_KERNEL :
			os << "CL_INVALID_KERNEL";
			break;
		case CL_INVALID_KERNEL_ARGS :
			os << "CL_INVALID_KERNEL_ARGS";
			break;
		case CL_INVALID_KERNEL_DEFINITION :
			os << "CL_INVALID_KERNEL_DEFINITION";
			break;
		case CL_INVALID_MEM_OBJECT :
			os << "CL_INVALID_MEM_OBJECT";
			break;
		case CL_INVALID_OPERATION :
			os << "CL_INVALID_OPERATION";
			break;
		case CL_INVALID_PLATFORM :
			os << "CL_INVALID_PLATFORM";
			break;
		case CL_INVALID_PROGRAM :
			os << "CL_INVALID_PROGRAM";
			break;
		case CL_INVALID_PROGRAM_EXECUTABLE :
			os << "CL_INVALID_PROGRAM_EXECUTABLE";
			break;
		case CL_INVALID_QUEUE_PROPERTIES :
			os << "CL_INVALID_QUEUE_PROPERTIES";
			break;
		case CL_INVALID_SAMPLER :
			os << "CL_INVALID_SAMPLER";
			break;
		case CL_INVALID_VALUE :
			os << "CL_INVALID_VALUE";
			break;
		case CL_INVALID_WORK_ITEM_SIZE :
			os << "CL_INVALID_WORK_ITEM_SIZE";
			break;
		case CL_MAP_FAILURE :
			os << "CL_MAP_FAILURE";
			break;
		case CL_MEM_OBJECT_ALLOCATION_FAILURE :
			os << "CL_MEM_OBJECT_ALLOCATION_FAILURE";
			break;
		case CL_MEM_COPY_OVERLAP :
			os << "CL_MEM_COPY_OVERLAP";
			break;
		case CL_OUT_OF_HOST_MEMORY :
			os << "CL_OUT_OF_HOST_MEMORY";
			break;
		case CL_OUT_OF_RESOURCES :
			os << "CL_OUT_OF_RESOURCES";
			break;
		case CL_PROFILING_INFO_NOT_AVAILABLE :
			os << "CL_PROFILING_INFO_NOT_AVAILABLE";
			break;
		case CL_SUCCESS :
			os << "CL_SUCCESS";
			break;
		default :
			os << err.err();
			break;
	}
	return os;
}

