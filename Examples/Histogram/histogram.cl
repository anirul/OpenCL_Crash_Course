#define NB_COLOR 256

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
