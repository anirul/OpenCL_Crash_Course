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

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>

#include "cl_histogram.hpp"

using namespace boost::program_options;

int main(int ac, char** av) {
	std::string input_image = "";
	std::string input_cl = "./histogram.cl";
	bool gpu = true;
	std::pair<unsigned int, unsigned int> setup_size;
	unsigned int device = 0;
	unsigned int platform = 0;
	try {
		// command line parsing
		options_description desc("Allowed options");
		// argument list
		desc.add_options
		()
		("help,h", "produce help message")
		("input-image,i", value<std::string>(), "input image file")
		("input-cl,c", value<std::string>(), "input OpenCL file")
		("device,d", value<unsigned int>(), "selected device")
		("platform,p", value<unsigned int>(), "selected platform")
		("cpu", "OpenCL with CPU")
		;
		variables_map vm;
		store(command_line_parser(ac, av).options(desc).run(), vm);
		if (vm.count("help")) {
			std::cout << desc << std::endl;
			return 1;
		}
		if (vm.count("input-image")) {
			input_image = vm["input-image"].as<std::string>();
		}
		std::cout << "input image     : " << input_image << std::endl;
		if (vm.count("input-cl")) {
			input_cl = vm["input-cl"].as<std::string>();
		}
		std::cout << "input cl        : " << input_cl << std::endl;
		if (vm.count("cpu")) {
			gpu = false;
			std::cout << "OpenCL (CPU)    : enable" << std::endl;
		} else {
			std::cout << "OpenCL (GPU)    : disable" << std::endl;
		}
		if (vm.count("platform")) {
			platform = vm["platform"].as<unsigned int>();
		}
		if (vm.count("device")) {
			device = vm["device"].as<unsigned int>();
		}
		std::vector<uint8_t> vec_img;
		{ // OpenCV mess
			cv::Mat img = cv::imread(input_image, CV_LOAD_IMAGE_GRAYSCALE);
			cv::Size size = img.size();
			setup_size = std::make_pair(size.width, size.height);
			size_t img_size = size.width * size.height * sizeof(uint8_t);
			vec_img.resize(size.width * size.height);
			memcpy(&vec_img[0], img.ptr(), img_size);
		}
		{ 	// call the opencl histogram
			cl_histogram hist(gpu, platform, device);
			hist.init(input_cl);
			hist.setup(setup_size);
			hist.prepare(vec_img);
			std::vector<unsigned int> out(256, 0);
			auto time = hist.run(out);
			for (auto i : out) {
				std::cout << i << std::endl;
			}
			std::cout << "computed in     : " << time << std::endl;
		}
	} catch (cl::Error& er) {
		std::cerr << "exception (cl)  : " << er.what();
		std::cerr << "(" << er.err() << ")" << std::endl;
		return -2;
	} catch (std::exception& ex) {
		std::cerr << "exception (std) : " << ex.what() << std::endl;
		return -1;
	}
	return 0;
}