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

#include <iostream>
#include <vector>
#define CL_HPP_ENABLE_EXCEPTIONS
#include <CL/cl2.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "cl_util.h"
#include "cl_floyd_warshall.h"
#include "ewd_file.h"

using namespace cl;
using namespace boost::program_options;
using namespace boost::posix_time;

int main(int ac, char** av) {
	bool enable_gpu = true;
	unsigned int nb_loops = 1;
	std::string cl_file = "floyd_warshall.cl";
	std::string graph_in = "rome99.txt";
	std::string graph_out{};
	try {
		// parse command line
		options_description desc("Allowed options");
		desc.add_options()
		("help,h", "produce help message")
		("opencl-cpu,c", "compute using openCL on CPU")
		("opencl-gpu,g", "compute using openCL on GPU (default)")
		("opencl-file,f", value<std::string>(), "input cl file")
		("file-in,i", value<std::string>(),
		 "Input graph file in EWD format")
		("file-out,o", value<std::string>(),
		 "Output graph file in EWD format")
		("loop,l", value<unsigned int>(),
		 "number of loops (only in no-window mode)")
		;
		variables_map vm;
		store(command_line_parser(ac, av).options(desc).run(), vm);
		if (vm.count("help")) {
			std::cout << desc << std::endl;
			return 1;
		}
		if (vm.count("opencl-cpu")) {
			enable_gpu = false;
		}
		if (vm.count("opencl-gpu")) {
			enable_gpu = true;
		}
		std::cout << "OpenCL (" << ((enable_gpu) ? "GPU" : "CPU");
		std::cout << ")    : enable" << std::endl;
		if (vm.count("file-in")) {
			graph_in = vm["file-in"].as<std::string>();
		}
		std::cout << "Graph file (in) : " << graph_in << std::endl;
		if (vm.count("file-out")) {
			graph_out = vm["file-out"].as<std::string>();
			std::cout << "Graph file (out): " << graph_out << std::endl;
		}
		if (vm.count("opencl-file")) {
			cl_file = vm["opencl-file"].as<std::string>();
			std::cout << "OpenCL file     : " << cl_file << std::endl;
		}
		if (vm.count("loop")) {
			nb_loops = vm["loop"].as<unsigned int>();
		}
		std::cout << "Loop            : " << nb_loops << std::endl;
		{
			// read the file with the data
			ewd_file ef;
			ef.import_file(graph_in);
			std::vector<float> initial_state(ef.size() * ef.size());
			ef.export_matrix(&initial_state[0], initial_state.size());
			if (initial_state.size() <= 256) ef.print_matrix(std::cout);
			// do the actual job
			cl_floyd_warshall cfw(enable_gpu);
			cfw.init(cl_file);
			std::cout
			<< "Setup           : (" << ef.size() << ", "
			<< ef.size() << ")" << std::endl;
			cfw.setup(std::make_pair<unsigned int, unsigned int>(
				static_cast<unsigned int>(ef.size()),
				static_cast<unsigned int>(ef.size())));
			time_duration best_time = minutes(60);
			for (unsigned int i = 0; i < nb_loops; ++i) {
				time_duration actual_time;
				std::cout
				<< "Prepare (buffer): " << initial_state.size() << std::endl;
				cfw.prepare(initial_state);
				std::cout
				<< "Run (buffer)    : " << initial_state.size() << std::endl;
				actual_time = cfw.run(initial_state);
				if (actual_time < best_time) best_time = actual_time;
				std::cout
				<< "\riteration [" << i + 1 << "/" << nb_loops << "] : "
				<< "actual_time : " << actual_time;
				std::cout.flush();
			}
			std::cout << std::endl
			<< "Finished : [" << nb_loops << "] Best time : " << best_time
			<< std::endl;
			if (graph_out.size() || initial_state.size() <= 256)
				ef.import_matrix(&initial_state[0], initial_state.size());
			// export to file out (in case exist)
			if (graph_out.size()) {
				ef.export_file(graph_out);
			}
			if (initial_state.size() <= 256) ef.print_matrix(std::cout);
		}
		// error handling
	} catch (cl::Error er) {
		std::cerr
		<< "exception (CL)  : " << er.what()
		<< "(" << er << ")" << std::endl;
		return -2;
	} catch (std::exception& ex) {
		std::cerr << "exception (std) : " << ex.what() << std::endl;
		return -1;
	}
	return 0;
}

