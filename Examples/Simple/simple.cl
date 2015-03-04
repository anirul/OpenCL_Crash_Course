// very simple kernel

__kernel void simple(
      __global read_only float* in1,
      __global read_only float* in2,
      __global write_only float* out)
{
   const uint pos = get_global_id(0);
   out[pos] = in1[pos] * in2[pos];
}

