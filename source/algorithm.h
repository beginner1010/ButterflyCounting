#include <ctime>
#include <random>

#include "Graph.h"
#include "counting.h"

class algorithm {
public:
	algorithm();
	void exact(Graph& g);
	void fast_edge_sampling(Graph& g, int& iter_exp, int& total_exp, int& ebfc_iterations, long double& max_time, int& snapshots);
	void fast_centered_edge_sampling(Graph& g, int& iter_exp, int& total_exp, int& ebfc_iterations, long double& max_time, int& snapshot);
	void path_sampling(Graph& g, int& iter_exp, int& total_exp, long double& max_time, int& snapshots);
	void path_centered_sampling(Graph& g, int& iter_exp, int& total_exp, long double& max_time, int& snapshot);
private:
	int ran_int(int A, int B);
	void fancy_text(int& iter_exp, int& total_exp, long double& max_time, long double& cur_runtime, bool done);
	const int max_fancy_text_width = 60;
};