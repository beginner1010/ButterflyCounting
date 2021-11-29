#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <math.h>
#include <unordered_map>
#include <unordered_set>
#include <cassert>
#include <sstream>

#include "IO.h"

#define mmax(a,b) ((a)>(b)?(a):(b))
#define mmin(a,b) ((a)<(b)?(a):(b))

#define EXACT "Exact algorithm -- Wang et al. Vertex Priority Based Butterfly Counting for Large-scale Bipartite Networks. arXiv 2018"
#define ONE_SHOT_DOULION "(one shot) Edge Sparsification Algorithm  -- (inspired by) Tsourakakis et al. Doulion: counting triangles in massive graphs with a coin. KDD'09."
#define ONE_SHOT_COLORFUL "(one shot) Colorful Sparsification Algorithm -- (inspired by) Pagh & Tsourakakis. Colorful triangle counting. IPL'12."
#define FAST_EDGE_SAMPLING "(local sampling) Vanilla Fast Edge Sampling -- Sanei-Mehri et al., Butterfly Counting in Bipartite Networks. KDD'18"
#define FAST_CENTERED_EDGE_SAMPLING "(local sampling) Centered Fast Edge Sampling, vertices are ordered by degrees"
#define WEDGE_SAMPLING "(local sampling) Vanilla Wedge Sampling"
#define PATH_SAMPLING "(local sampling) Vanilla Path Sampling -- (inspired by) Jha et al. Path sampling: A fast and provable method for estimating 4-vertex subgraph counts. WWW'15."
#define PATH_CENTERED_SAMPLING "(local sampling) Centered Path Sampling, vertices are ordered by degrees -- (inspired by) Jha et al. WWW'15."
#define PATH_CENTERED_SAMPLING_BY_WEDGE "(local sampling) Centered Path Sampling ordered by wedge" 
#define PATH_CENTERED_SAMPLING_BY_DEGENERACY "(local sampling) Centered Path Sampling ordered by degeneracy"
#define EXACT_INSERTION_ONLY "Exact Algorithm when Stream is Insertion Only"
#define EXACT_FULLY_DYNAMC "Exact Algorithm for Fully Dynamic Graph Stream (with deletion)"
#define FLEET3 "FLEET3: Butterfly Estimation from a Graph Stream (insertion only)"
#define FLEET3_FULLY_DYNAMIC "FLEET3: Butterfly Estimation from a Fully Dynamic Graph Stream (with deletion)"
#define THINKD_FAST "THINKD Fast: Fast, Accurate and Provable Triangle Counting in Fully Dynamic Graph Streams -- K. Shin et al. TKDD"
#define THINKD_ACC "THINKD Acc: Fast, Accurate and Provable Triangle Counting in Fully Dynamic Graph Streams -- K. Shin et al. TKDD"
#define TRIEST_FD "TRIEST FD: Counting local and global triangles in Fully Dynamic Streams with fixed memory size -- Stefani et al. TKDD"
#define RS_WITH_DELETION "Reservoir Sampling with Deletion -- our algorithm"
#define IMPR_RS_WITH_DELETION "Improved Reservoir Sampling with Deletion -- our algorithm"


namespace constants {
	extern std::vector<std::string> algorithm_names;
	extern std::unordered_map<std::string, std::string> suffix_output_address;
	extern int ebfc_iterations;
}

namespace settings {
	extern int snapshots;
	extern long double max_time;
	extern int exp_repeatition;
	extern double min_p, max_p, p;
	extern int min_clr, max_clr, n_colors;
	extern double min_gamma, max_gamma, gamma;
	extern int min_res_size, max_res_size, res_size;
	extern bool is_static;
	extern std::string chosen_algo;
	extern std::vector<std::string>::iterator itr;
	void is_static_or_streaming();
	void get_settings();
	bool is_exact_algorithm();
	bool has_fixed_probability();
	bool has_fixed_gamma();
	bool has_fixed_reservoir();
	bool is_one_shot_algorithm();
	bool is_local_sampling_algorithm();
	bool is_streaming();
	bool is_only_insertion_algo();
	bool is_fully_dynamic_streaming_algo();
	void clear_settings();
	bool continue_run();
	int next_parameter();
}

namespace helper_functions {
	std::string to_str(int x);
	int to_int(const std::string& str);
	long long to_ll(const std::string& str);
	double to_double(const std::string& str);
	bool is_int_num(const std::string& str);
	bool is_equal(double a, double b);
	long long choose2(int x);
	bool yesno_query();
}