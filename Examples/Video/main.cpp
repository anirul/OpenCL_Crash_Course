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

#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <boost/program_options.hpp>
#include <opencv2/opencv.hpp>

#include "win_video.hpp"
#include "glut_win.hpp"

using namespace boost::program_options;

int main(int ac, char** av) {
	std::string input_video = "";
	std::string input_cl = "./video.cl";
	bool gpu = true;
	bool color = true;
	unsigned int device = 0;
	try {
		options_description desc("Allowed options");
		desc.add_options
		()
		("help,h", "produce help message")
		("input-video,i", value<std::string>(), "input video file")
		("input-cl,c", value<std::string>(), "input cl file")
		("cpu", "OpenCL with CPU")
		("device,d", value<unsigned int>(), "OpenCL device")
		("black-white,b", "black and white mode")
		;
		variables_map vm;
		store(command_line_parser(ac, av).options(desc).run(), vm);
		if (vm.count("help")) {
			std::cout << desc << std::endl;
			return 1;
		}
		if (vm.count("input-video")) {
			input_video = vm["input-video"].as<std::string>();
			std::cout << "input video     : " << input_video << std::endl;
		}
		if (vm.count("input-cl")) {
			input_cl = vm["input-cl"].as<std::string>();
		}
		std::cout << "input cl file   : " << input_cl << std::endl;
		if (vm.count("cpu")) {
			gpu = false;
			std::cout << "OpenCL (CPU)    : enable" << std::endl;
		} else {
			std::cout << "OpenCL (GPU)    : enable" << std::endl;
		}
		if (vm.count("device")) {
			device = vm["device"].as<unsigned int>();
		}
		if (vm.count("black-white")) {
			color = false;
		}
		std::cout
		<< "color mode      : "
		<< ((color) ? "enable" : "disable")
		<< std::endl;
		cv::VideoCapture video;
		if (input_video.size())
			video.open(input_video);
		else
			video.open(0);
		if (!video.isOpened())
			throw std::runtime_error("could not open video " + input_video);
		// get the first frame to get the size
		std::pair<unsigned int, unsigned int> size;
		std::vector<char> initial_frame;
		cv::Mat frame;
		cv::Mat temp;
		if (video.grab()) {
			video.retrieve(frame);
			if (!color)
				cv::cvtColor(frame, temp, CV_BGR2GRAY);
			else
				cv::cvtColor(frame, temp, CV_BGR2BGRA);
			cv::Size frame_size = frame.size();
			size.first = frame_size.width;
			size.second = frame_size.height;
			initial_frame.resize(size.first * size.second * ((color) ? 4 : 1));
			memcpy(&initial_frame[0], temp.ptr(), size.first * size.second);
		}
		win_video* pwv = new win_video
		(size,
		 initial_frame,
		 [&](std::vector<char>& vec) {
			 if (video.grab()) {
				 video.retrieve(frame);
				 cv::flip(frame, temp, 0);
				 if (!color)
					 cv::cvtColor(temp, frame, CV_BGR2GRAY);
				 else
					 cv::cvtColor(temp, frame, CV_BGR2BGRA);
				 vec.resize(size.first * size.second * frame.elemSize());
				 memcpy(&vec[0],
						frame.ptr(),
						size.first * size.second * frame.elemSize());
				 return (cv::waitKey(1) != 27);
			 } else {
				 return false;
			 }
		 },
		 input_cl,
		 color,
		 gpu,
		 device);
		glut_win* pgw = glut_win::instance("OpenCL Video", size, pwv);
		pgw->run();
		video.release();
	} catch(std::exception& ex) {
		std::cerr << "exception (std) : " << ex.what() << std::endl;
		return -1;
	}
	return 0;
}
