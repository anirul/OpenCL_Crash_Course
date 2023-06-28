// simple kernel that copy in into out

__kernel void video_image(
    __read_only image2d_t in,
    __write_only image2d_t out)
{
    const sampler_t format =
        CLK_NORMALIZED_COORDS_FALSE |
        CLK_FILTER_NEAREST |
        CLK_ADDRESS_CLAMP;
    const int2 d = (int2)(get_global_id(0), get_global_id(1));
    float4 col = read_imagef(in, format, d);
    float bw = col.x + col.y + col.z / 3.0;
    write_imagef(out, d, (float4)(bw, bw, bw, 1.0));
}
