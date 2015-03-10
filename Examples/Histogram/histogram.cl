#define NB_COLOR 256

// access format for the image
constant sampler_t format =
	CLK_NORMALIZED_COORDS_FALSE | 
	CLK_FILTER_NEAREST | 
	CLK_ADDRESS_CLAMP;

// luminosity from color (float4 -> float)
static float luminosity_from_color(const float4 col)
{
	return 0.21f * col.x + 0.72f * col.y + 0.07f * col.z;
}

// compute luminosity from RGBA color image
// size (image_x, image_y)
//	img					: image (in)
//	luminosity			: linear luminosity map (out)
kernel void histogram_luminosity(
	read_only image2d_t img,
	global uchar* luminosity)
{
	int2 d = (int2)(get_global_id(0), get_global_id(1));
	int l = get_global_id(0) + get_global_id(1) * get_global_size(0);
	float4 col = read_imagef(img, format, d);
	float lum = luminosity_from_color(col);
	luminosity[l] = convert_uchar_sat(min(lum, 1.0f) * 255.0f);
}

// init the partial memory to 0
// size (work_group_size * num_groups)
// 	partial_histogram 	: the buffer with the partial histogram (out)
kernel void histogram_init(
	global uint* partial_histogram)
{
	int group_indx = get_global_id(1) * NB_COLOR;
	int tid = get_global_id(0);
	if (tid < NB_COLOR)
		partial_histogram[group_indx + tid] = 0;
}

// set the partial memory
// size (work_group_size * num_group)
// 	luminosity			: luminosity image (in)
//	partial_histogram	: the buffer with the partial histogram (out)
kernel void histogram_partial(
	global const uchar* luminosity,
	global uint* partial_histogram)
{
	int image_len = get_global_size(0) * get_global_size(1);
	int group_indx = get_global_id(1) * NB_COLOR;
	int linear_index = get_global_id(1) * get_global_size(0) + get_global_id(0);

	if (linear_index < image_len) {
		uchar col_indx = luminosity[linear_index];
		atomic_inc(&partial_histogram[group_indx + col_indx]);
	}
}

// reduce the histogram (from partial to final)
// size (NB_COLOR)
//	partial_histogram	: the buffer with the partial histogram (in)
//	num_groups			: number of groups (in)
// 	histogram			: output histogram (out)
kernel void histogram_reduce(
	global const uint *partial_histogram,
	const int num_groups,
	global uint *histogram)
{
	int tid = (int)get_global_id(0);
	int group_indx;
	int n = num_groups;

	int tid_histogram = 0;
	
	for (int i = 0; i < num_groups * NB_COLOR; i += NB_COLOR)
		tid_histogram += partial_histogram[i + tid];

	// cumulative histogram
	histogram[tid] = tid_histogram;
}
