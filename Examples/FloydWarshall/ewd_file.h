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

#ifndef EWD_FILE_HEADER_DEFINED
#define EWD_FILE_HEADER_DEFINED

#include <iostream>
#include <fstream>
#include <map>
#include <string>

const float huge_float = 1e30f;

class ewd_file {
protected :
	size_t nb_vector_;
	size_t nb_edges_;
	std::map<std::pair<unsigned int, unsigned int>, float> edges_;
public :
	ewd_file();
	virtual ~ewd_file();
	void import_file(const std::string& name) throw(std::exception);
	void export_file(const std::string& name) throw(std::exception);
	void import_matrix(float* p, size_t size) throw(std::exception);
	void export_matrix(float* p, size_t size) throw(std::exception);
	size_t size() const;
	float dist(unsigned int v1, unsigned int v2) const;
   void print_matrix(std::ostream& os);
	std::ostream& operator<<(std::ostream& os);
};

#endif

