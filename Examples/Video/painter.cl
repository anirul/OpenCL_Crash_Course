// simple kernel that copy in into out

#define radius 3
#define power_radius (radius * radius)
#define intensity_level 20

__kernel void video_image(
    __read_only image2d_t in,
    __write_only image2d_t out)
{
    const sampler_t format =
        CLK_NORMALIZED_COORDS_FALSE |
        CLK_FILTER_NEAREST |
        CLK_ADDRESS_CLAMP;
    const int2 d = (int2)(get_global_id(0), get_global_id(1));
    int intensity_count[intensity_level];
    float4 average_color[intensity_level];
    // cleanup
    for (int i = 0; i < intensity_level; ++i) {
        intensity_count[i] = 0;
        average_color[i] = (float4)(0.0f, 0.0f, 0.0f, 1.0f);
    }
    // step 1
    // for each pixel within radius of pixel
    for (int x = d.x - radius; x < d.x + radius; ++x) {
        for (int y = d.y - radius; y < d.y + radius; ++y) {
            float2 abs_pos = (float2)(d.x - x, d.y - y);
            if (power_radius < dot(abs_pos, abs_pos))
                continue;
            float4 color_element = read_imagef(in, format, (int2)(x, y));
            int current_intensity =
                (dot(color_element, (float4)(1.0f, 1.0f, 1.0f, 0.0f)) / 3.0f) *
                intensity_level;
            current_intensity = (current_intensity >= intensity_level) ?
                intensity_level - 1 :
                current_intensity;
            intensity_count[current_intensity] += 1;
            average_color[current_intensity] += color_element;
        }
    }
    // step 2
    // find the maximum level of intensity
    int max_level = 0;
    int max_index = 0;
    for (int level = 0; level < intensity_level; ++level) {
        if (intensity_count[level] > max_level) {
            max_level = intensity_count[level];
            max_index = level;
        }
    }
    // step 3
    // write the final color
    float4 out_col = average_color[max_index] / max_level;
    out_col.w = 1.0f;
    write_imagef(out, d, out_col);
}
