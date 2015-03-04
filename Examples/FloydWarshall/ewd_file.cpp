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

#include <limits>
#include <stdexcept>
#include <sstream>
#include <math.h>
#include "ewd_file.h"

ewd_file::ewd_file() : nb_vector_(0), nb_edges_(0) {
   edges_.clear();
}

ewd_file::~ewd_file() {
   nb_vector_ = 0;
   nb_edges_ = 0;
   edges_.clear();
}

void ewd_file::import_file(const std::string& name) throw(std::exception)
{
   std::ifstream ifs;
   ifs.open(name.c_str());
   if (!ifs.is_open()) 
      throw std::runtime_error("Could not open file : " + name);
   ifs >> nb_vector_;
   if (nb_vector_ == 0)
      throw std::runtime_error("Parse error invalid vectors size");
   ifs >> nb_edges_;
   if (nb_edges_ == 0)
      throw std::runtime_error("Parse error invalid edges size");
   std::cout << "Graph size      : " << nb_vector_ << std::endl;
   for (int i = 0; i < nb_edges_; ++i) {
      std::cout << "Graph edge [" << (i + 1) << "/" << nb_edges_ << "]\r";
      std::cout.flush();
      std::stringstream ss("");
      ss << "Parse error at edge : " << i;
      unsigned int v1;
      unsigned int v2;
      float d;
      ifs >> v1;
      ifs >> v2;
      ifs >> d;
      if (v1 >= nb_vector_) {
         ss << " invalid first vector : " << v1;
         throw std::runtime_error(ss.str());
      }
      if (v2 >= nb_vector_) {
         ss << " invalid second vector : " << v2; 
         throw std::runtime_error(ss.str());
      }
      if (d <= 0.0f) {
         ss << " invalid distance : " << d;
         throw std::runtime_error(ss.str());
      }
      edges_.insert(
            std::make_pair<std::pair<unsigned int , unsigned int>, float>(
               std::make_pair<unsigned int, unsigned int>(v1, v2),
               d));
   }
   std::cout << std::endl;
   ifs.close();
}

void ewd_file::export_file(const std::string& name) throw(std::exception)
{
   std::ofstream ofs;
   ofs.open(name.c_str());
   if (!ofs.is_open())
      throw std::runtime_error("Could not open file : " + name);
   this->operator<<(ofs);
   ofs.close();
}

float ewd_file::dist(unsigned int v1, unsigned int v2) const {
   if (v1 == v2) 
      return 0.0f;
   std::map<std::pair<unsigned int, unsigned int>, float>::const_iterator ite;
   ite = edges_.find(std::make_pair<unsigned int, unsigned int>(v1, v2));
   if (ite == edges_.end()) 
      return huge_float;
   return ite->second;
}

size_t ewd_file::size() const {
   return nb_vector_;
}

void ewd_file::import_matrix(float* p, size_t size) throw(std::exception)
{
   nb_vector_ = sqrt(size);
   for (int x = 0; x < nb_vector_; ++x) {
      for (int y = 0; y < nb_vector_; ++y) {
         float distance = p[x + (y * nb_vector_)];
         if (distance < huge_float) {
            edges_.insert(
                  std::make_pair<std::pair<unsigned int , unsigned int>, float>(
                     std::make_pair<unsigned int, unsigned int>(x, y),
                     distance));
         }
      }
   }
}

void ewd_file::export_matrix(float* p, size_t size) throw(std::exception)
{
   if ((nb_vector_ * nb_vector_) != size)
      throw std::runtime_error("Unmatched size!");
   for (int x = 0; x < nb_vector_; ++x) {
      std::cout 
         << "Export matrix line [" << x + 1 << "/" << nb_vector_ << "]\r";
      std::cout.flush();
      for (int y = 0; y < nb_vector_; ++y) {
         p[x + (y * nb_vector_)] = dist(x, y);
      }
   }
   std::cout << std::endl;
}

void ewd_file::print_matrix(std::ostream& os) {
   for (int x = 0; x < nb_vector_; ++x) {
      for (int y = 0; y < nb_vector_; ++y) {
         std::stringstream ss("");
         ss << dist(x, y);
         int line_left = 8 - ss.str().size();
         os << ss.str();
         for (int i = 0; i < line_left; ++i)
            os << " ";
      }
      std::cout << std::endl;
   }
}

std::ostream& ewd_file::operator<<(std::ostream& os) {
   os << nb_vector_ << std::endl;
   os << nb_edges_ << std::endl;
   std::map<std::pair<unsigned int, unsigned int>, float>::const_iterator ite;
   for (ite = edges_.begin(); ite != edges_.end(); ++ite) {
      os 
         << ite->first.first << "\t" 
         << ite->first.second << "\t"
         << ite->second << std::endl;
   }
   return os;
}

