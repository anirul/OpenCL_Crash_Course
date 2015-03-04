// simple OpenCL example

#include <iostream>
#include <fstream>
#include <vector>
#define __CL_ENABLE_EXCEPTIONS
#include <CL/cl.hpp>

const unsigned int vector_size = 1024;

int main(int ac, char** av) {
   try {
      // get the device (here the GPU)
      std::vector<cl::Platform> platforms;
      std::vector<cl::Device> devices_;
      cl::Platform::get(&platforms);
      platforms[0].getDevices(CL_DEVICE_TYPE_GPU, &devices_);
      cl_context_properties properties[] = { 
         CL_CONTEXT_PLATFORM, 
         (cl_context_properties)(platforms[0])(), 
         0
      };
      cl::Context context_ = cl::Context(CL_DEVICE_TYPE_GPU, properties);
      devices_ = context_.getInfo<CL_CONTEXT_DEVICES>();
      cl::CommandQueue queue_(context_, devices_[0]);
      // load the kernel code
      std::ifstream ifs("./simple.cl");
      std::string str(
            (std::istreambuf_iterator<char>(ifs)), 
            std::istreambuf_iterator<char>());
      ifs.close();
      // compile
      cl::Program::Sources source(1, std::make_pair(str.c_str(), str.size()));
      cl::Program program_(context_, source);
      program_.build(devices_);
      // create the kernel
      cl::Kernel kernel_(program_, "simple");
      // prepare the buffers
      std::vector<float> in1(vector_size);
      std::vector<float> in2(vector_size);
      for (std::vector<float>::iterator ite = in1.begin(); ite != in1.end(); ++ite)
         (*ite) = (float)random();
      for (std::vector<float>::iterator ite = in2.begin(); ite != in2.end(); ++ite)
         (*ite) = (float)random();
      cl::Buffer buf_in1_ = cl::Buffer(
            context_, 
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(float) * vector_size,
            (void*)&in1);
      cl::Buffer buf_in2_ = cl::Buffer(
            context_,
            CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR,
            sizeof(float) * vector_size,
            (void*)&in2);
      cl::Buffer buf_out_ = cl::Buffer(
            context_, 
            CL_MEM_WRITE_ONLY, 
            sizeof(float) * vector_size);
      // set the arguments
      kernel_.setArg(0, buf_in1_);
      kernel_.setArg(1, buf_in2_);
      kernel_.setArg(2, buf_out_);
      // wait to the command queue to finish before proceeding
      queue_.finish();
      // run the kernel
      std::vector<float> out(vector_size);
      queue_.enqueueNDRangeKernel(
            kernel_, 
            cl::NullRange, 
            cl::NDRange(vector_size), 
            cl::NullRange);
      queue_.finish();
      // get the result out
      queue_.enqueueReadBuffer(
            buf_out_, 
            CL_TRUE, 
            0, 
            vector_size * sizeof(float), 
            &out[0]);
      queue_.finish();
      std::cout << "Operation successfull" << std::endl;
   } catch (cl::Error& er) {
      std::cerr << "Exception(CL) : " << er.what() << std::endl;
   } catch (std::exception& ex) {
      std::cerr << "Exception(STL) : " << ex.what() << std::endl;
   }
   return 0;
}

