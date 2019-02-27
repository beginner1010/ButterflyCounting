#include "algorithm.h"
#include "counting.h"

extern std::mt19937_64 gen;
extern std::uniform_int_distribution<int> int_ran;

int algorithm::ran_int(int A, int B) {
	decltype(int_ran.param()) new_range(A, B);
	int_ran.param(new_range);
	return int_ran(gen);
}

algorithm::algorithm() {}

void algorithm::exact(Graph& g) {
	std::clock_t start = std::clock();
	long long total_bfly = kai_exact_count(g);
	std::clock_t stop = std::clock();

	long double total_time = ((long double)stop - start) / CLOCKS_PER_SEC;
	printf("%lld, exact, %.2Lf\n", total_bfly, total_time);
}

void algorithm::fancy_text(int& iter_exp, int& total_exp, long double& max_time, long double& cur_runtime, bool done) {
	std::cerr << "\r";
	std::cerr << std::string(max_fancy_text_width, ' ');
	std::cerr << "\r";
	std::string color = "\033[1;32m";
	if (done == false) {
		fprintf(stderr, "%s [%3d%%] of experiment %d (out of %d) is done.", color.c_str(),
			((int)(cur_runtime / max_time * 100.0)), iter_exp, total_exp);
	}else {
		fprintf(stderr, "The experiment is finished!\n");
	}
	fprintf(stderr, "\033[0m");
	std::fflush(stderr);
}

void algorithm::fast_edge_sampling(Graph& g, int& iter_exp, int& total_exp, int& ebfc_iterations, long double& max_time, int& snapshot) {
	long double time_interval = (long double)max_time / snapshot;
	long double last_time = -time_interval;
	long double runtime = 0;

	long long n_iterations = 0;
	long double unbiased_estimate_bfly = 0;
	long double non_normalized_estimate = 0;

	std::random_device rd_edge;
	std::mt19937_64 gen_edge(rd_edge());
	std::uniform_int_distribution<int> edge_dis(0, g.get_n_edges() - 1);

	if (iter_exp == 1)
		printf("exp_rep, bfly, algo, time, iterations\n");

	while (runtime < max_time) {

		clock_t start = clock();
		int random_edge_index = edge_dis(gen_edge);
		std::pair<int, int>& edge = g.get_ith_edge(random_edge_index);
		int& vertex_left = edge.first;
		int& vertex_right = edge.second;
		n_iterations++;
		non_normalized_estimate += randomized_ebfc(g, vertex_left, vertex_right, ebfc_iterations);
		clock_t stop = clock();
		runtime += ((long double)stop - start) / CLOCKS_PER_SEC;

		if (runtime - last_time > time_interval) {
			fancy_text(iter_exp, total_exp, max_time, runtime, false);
			unbiased_estimate_bfly = (non_normalized_estimate / n_iterations) * (long double)g.get_n_edges() / 4.0;
			printf("%d, fes, %.2Lf, %.2Lf, %lld\n", iter_exp, unbiased_estimate_bfly, runtime, n_iterations);
			fflush(stdout);
			last_time = runtime;
		}
	}
}

void algorithm::fast_centered_edge_sampling(Graph& g, int& iter_exp, int& total_exp, int& ebfc_iterations, long double& max_time, int& snapshot) {
	long double time_interval = (long double)max_time / snapshot;
	long double last_time = -time_interval;
	long double runtime = 0;

	long long n_iterations = 0;
	long double unbiased_estimate_bfly = 0;
	long double non_normalized_estimate = 0;

	clock_t start_reindex_by_degree = clock();
	if (iter_exp == 1)
		g.sort_vertices_by_degree();
	clock_t stop_reindex_by_degree = clock();

	long double runtime_reindex = ((long double)stop_reindex_by_degree - start_reindex_by_degree) / CLOCKS_PER_SEC;

	std::random_device rd_edge;
	std::mt19937_64 gen_edge(rd_edge());
	std::uniform_int_distribution<int> edge_dis(0, g.get_n_edges() - 1);

	if (iter_exp == 1)
		printf("exp_rep, bfly, algo, time, preprocess_time, iterations\n");

	while (runtime < max_time) {

		clock_t start = clock();
		int random_edge_index = edge_dis(gen_edge);
		std::pair<int, int>& edge = g.get_ith_edge(random_edge_index);
		int& vertex_left = edge.first;
		int& vertex_right = edge.second;
		n_iterations++;
		non_normalized_estimate += randomized_centered_ebfc(g, vertex_left, vertex_right, ebfc_iterations);
		clock_t stop = clock();
		runtime += ((long double)stop - start) / CLOCKS_PER_SEC;

		if (runtime - last_time > time_interval) {
			fancy_text(iter_exp, total_exp, max_time, runtime, false);
			unbiased_estimate_bfly = (non_normalized_estimate / n_iterations) * (long double)g.get_n_edges();
			printf("%d, fces, %.2Lf, %.2Lf, %.2Lf, %lld\n", iter_exp, unbiased_estimate_bfly, 
				runtime, runtime_reindex, n_iterations);
			fflush(stdout);
			last_time = runtime;
		}
	}
}


void algorithm::path_sampling(Graph& g, int& iter_exp, int& total_exp, long double& max_time, int& snapshot) {
	long double time_interval = (long double)max_time / snapshot;
	long double last_time = -time_interval;
	long double runtime = 0;

	long long n_iterations = 0;
	long double unbiased_estimate_bfly = 0;
	long double non_normalized_estimate = 0;

	clock_t start_nz = clock();
	long long n_z = g.get_nz();
	clock_t stop_nz = clock();
	long double runtime_nz = ((long double)stop_nz - start_nz) / CLOCKS_PER_SEC;

	std::random_device rd_edge;
	std::mt19937_64 gen_edge(rd_edge());
	std::uniform_int_distribution<long long> z_dis(0, n_z - 1);

	if (iter_exp == 1)
		printf("exp_rep, bfly, algo, time, preprocess_time, iterations\n");

	while (runtime < max_time) {

		clock_t start = clock();
		long long random_z_index = z_dis(gen_edge);
		int edge_index = g.get_edge_index_by_nz(random_z_index);
		std::pair<int, int>& edge = g.get_ith_edge(edge_index);
		int& vertex_left = edge.first;
		int& vertex_right = edge.second;
		n_iterations++;
		non_normalized_estimate += z_forms_bfly(g, vertex_left, vertex_right);
		clock_t stop = clock();
		runtime += ((long double)stop - start) / CLOCKS_PER_SEC;

		if (runtime - last_time > time_interval) {
			fancy_text(iter_exp, total_exp, max_time, runtime, false);
			unbiased_estimate_bfly = (non_normalized_estimate / n_iterations) * (long double)n_z / 4.0;
			printf("%d, ps, %.2Lf, %.2Lf, %.2Lf, %lld\n", iter_exp, unbiased_estimate_bfly, 
															runtime, runtime_nz, n_iterations);
			fflush(stdout);
			last_time = runtime;
		}
	}
}

void algorithm::path_centered_sampling(Graph& g, int& iter_exp, int& total_exp, long double& max_time, int& snapshot) {
	long double time_interval = (long double)max_time / snapshot;
	long double last_time = -time_interval;
	long double runtime = 0;

	long long n_iterations = 0;
	long double unbiased_estimate_bfly = 0;
	long double non_normalized_estimate = 0;

	clock_t start_centered_nz = clock();
	if (iter_exp == 1)
		g.sort_vertices_by_degree();
	long long centered_nz = g.get_n_centered_z();
	clock_t stop_centered_nz = clock();
	long double runtime_centered_nz = ((long double)stop_centered_nz - start_centered_nz) / CLOCKS_PER_SEC;

	std::random_device rd_edge;
	std::mt19937_64 gen_edge(rd_edge());
	std::uniform_int_distribution<long long> z_dis(0, centered_nz - 1);

	if (iter_exp == 1)
		printf("exp_rep, bfly, algo, time, preprocess_time, iterations\n");

	while (runtime < max_time) {

		clock_t start = clock();
		long long random_z_index = z_dis(gen_edge);
		int edge_index = g.get_edge_index_by_centered_nz(random_z_index);
		std::pair<int, int>& edge = g.get_ith_edge(edge_index);
		int& vertex_left = edge.first;
		int& vertex_right = edge.second;
		n_iterations++;
		non_normalized_estimate += centered_z_forms_bfly(g, vertex_left, vertex_right);
		clock_t stop = clock();
		runtime += ((long double)stop - start) / CLOCKS_PER_SEC;

		if (runtime - last_time > time_interval) {
			fancy_text(iter_exp, total_exp, max_time, runtime, false);
			unbiased_estimate_bfly = (non_normalized_estimate / n_iterations) * (long double)centered_nz;
			printf("%d, pcs, %.2Lf, %.2Lf, %.2Lf, %lld\n", iter_exp, unbiased_estimate_bfly,
				runtime, runtime_centered_nz, n_iterations);
			fflush(stdout);
			last_time = runtime;
		}
	}
}