#include "counting.h"

extern std::mt19937_64 gen;
extern std::uniform_int_distribution<int> int_ran;

int ran_int(int A, int B) {
	decltype(int_ran.param()) new_range(A, B);
	int_ran.param(new_range);
	return int_ran(gen);
}

long long kai_exact_count(Graph& g) {
	long long ans = 0;
	std::vector<int>& start_vertices = g.get_vertices();
	for (auto& start : start_vertices) {
		std::unordered_map <int, int> counter;
		std::vector<int>& neighbors_of_start = g.get_adj_vec(start);
		int degree_start = (int)neighbors_of_start.size();
		for (auto& middle : neighbors_of_start) {
			std::vector<int>& neighbors_of_middle = g.get_adj_vec(middle);
			int degree_middle = (int)neighbors_of_middle.size(); 
			if (degree_start > degree_middle || (degree_start == degree_middle && start > middle)) {
				for (auto& end : neighbors_of_middle) {
					int degree_end = (int)g.get_adj_vec(end).size() ;
					if (degree_start > degree_end || (degree_start == degree_end && start > end)) {
						ans += counter[end];
						counter[end] ++;
					}
				}
			}
		}
	}
	return ans;
}

long double randomized_ebfc(Graph& g, int& vertex_a, int& vertex_b, int& ebfc_iterations) {
	int degree_a = (int)g.get_adj_vec(vertex_a).size();
	int degree_b = (int)g.get_adj_vec(vertex_b).size();
	if (degree_a == 0 || degree_b == 0)
		return 0;
	std::uniform_int_distribution<int> dis_a(0, degree_a - 1);
	std::uniform_int_distribution<int> dis_b(0, degree_b - 1);
	long double res = 0;
	for (int iter = 0; iter < ebfc_iterations; iter++) {
		int& neighbor_a = g.get_adj_vec(vertex_a)[dis_a(gen)];
		int& neighbor_b = g.get_adj_vec(vertex_b)[dis_b(gen)];
		if (neighbor_a != vertex_b && neighbor_b != vertex_a && g.get_adj_set(neighbor_a).find(neighbor_b) != g.get_adj_set(neighbor_a).end()) {
			res++;
		}
	}
	res /= ebfc_iterations;
	res *= ((long double)degree_a) * (degree_b);
	return res;
}

long double randomized_centered_ebfc(Graph& g, int& vertex_a, int& vertex_b, int& ebfc_iterations) {
	/*std::vector <int>& unordered_adj_a = g.get_adj_vec(vertex_a);
	std::vector <int>& unordered_adj_b = g.get_adj_vec(vertex_b);
	std::vector <int> adj_a;
	std::vector <int> adj_b;
	for (auto& neighbor_a : unordered_adj_a) {
		if (g.degree(neighbor_a) > g.degree(vertex_b)
			|| (g.degree(neighbor_a) == g.degree(vertex_b) && neighbor_a > vertex_b))
			adj_a.push_back(neighbor_a);
	}
	for (auto& neighbor_b : unordered_adj_b) {
		if (g.degree(neighbor_b) > g.degree(vertex_a) 
			|| (g.degree(neighbor_b) == g.degree(vertex_a) && neighbor_b > vertex_a))
			adj_b.push_back(neighbor_b);
	}
	int degree_a = (int)adj_a.size();
	int degree_b = (int)adj_b.size();*/

	std::vector<int>& adj_a = g.get_adj_vec(vertex_a);
	std::vector<int>& adj_b = g.get_adj_vec(vertex_b);
	int index_b_in_adj_a = (int)(std::lower_bound(adj_a.begin(), adj_a.end(), vertex_b) - adj_a.begin());
	int index_a_in_adj_b = (int)(std::lower_bound(adj_b.begin(), adj_b.end(), vertex_a) - adj_b.begin());
	int degree_a = g.degree(vertex_a) - index_b_in_adj_a - 1;
	int degree_b = g.degree(vertex_b) - index_a_in_adj_b - 1;


	if (degree_a == 0 || degree_b == 0)
		return 0;

	std::uniform_int_distribution<int> dis_a(index_b_in_adj_a + 1, g.degree(vertex_a) - 1);
	std::uniform_int_distribution<int> dis_b(index_a_in_adj_b + 1, g.degree(vertex_b) - 1);
	long double res = 0;
	for (int iter = 0; iter < ebfc_iterations; iter++) {
		int& neighbor_a = adj_a[dis_a(gen)];
		int& neighbor_b = adj_b[dis_b(gen)];
		if (neighbor_a != vertex_b && neighbor_b != vertex_a && g.get_adj_set(neighbor_a).find(neighbor_b) != g.get_adj_set(neighbor_a).end()) {
			res++;
		}
	}
	res /= ebfc_iterations;
	res *= ((long double)degree_a) * (degree_b);
	return res;
}

bool z_forms_bfly(Graph& g, int& vertex_a, int& vertex_b) {
	int degree_a = g.degree(vertex_a);
	int degree_b = g.degree(vertex_b);
	int neighbor_a = g.get_adj_vec(vertex_a)[ran_int(0, degree_a - 1)];
	int neighbor_b = g.get_adj_vec(vertex_b)[ran_int(0, degree_b - 1)];
	if (neighbor_a != vertex_b && neighbor_b != vertex_a && g.get_adj_set(neighbor_a).find(neighbor_b) != g.get_adj_set(neighbor_a).end()) {
		return true;
	}
	return false;
}

bool centered_z_forms_bfly(Graph& g, int& vertex_a, int& vertex_b) {
	std::vector<int>& adj_a = g.get_adj_vec(vertex_a);
	std::vector<int>& adj_b = g.get_adj_vec(vertex_b);
	int index_b_in_adj_a = (int)(std::lower_bound(adj_a.begin(), adj_a.end(), vertex_b) - adj_a.begin());
	int index_a_in_adj_b = (int)(std::lower_bound(adj_b.begin(), adj_b.end(), vertex_a) - adj_b.begin());
	int degree_a = g.degree(vertex_a) - index_b_in_adj_a - 1;
	int degree_b = g.degree(vertex_b) - index_a_in_adj_b - 1;

	if (degree_a == 0 || degree_b == 0)
		return false;

	int neighbor_a = adj_a[ran_int(index_b_in_adj_a + 1, g.degree(vertex_a) - 1)];
	int neighbor_b = adj_b[ran_int(index_a_in_adj_b + 1, g.degree(vertex_b) - 1)];

	if (neighbor_a != vertex_b && neighbor_b != vertex_a && g.get_adj_set(neighbor_a).find(neighbor_b) != g.get_adj_set(neighbor_a).end()) {
		return true;
	}
	return false;
}