#include "algorithms.h"

/* Static graph: Approximation by one shot sampling */

namespace exact {
	void algorithms::exact_algorithm(Graph& G) {
		clock_t start_time = clock();
		this->unnormalized_butterfly_count = (double)exact::global::kai_exact_count(G);
		this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
	}
}

namespace static_processing {
	namespace one_shot {
		void algorithms::doulion(Graph& G) {
			/*
				Tsourakakis, Charalampos E., U.Kang, Gary L.Miller, and Christos Faloutsos.
				"Doulion: counting triangles in massive graphs with a coin."
				In Proceedings of the 15th ACM SIGKDD international conference on Knowledge discovery and data mining, pp. 837 - 846. ACM, 2009.
			*/
			clock_t start_time = clock();
			Graph sampled_graph = Graph();
			for (auto& edge : G.get_edges()) {
				if (sampler::generate_random_dbl(0.0, 1.0) <= settings::p) {
					int left_vertex = sampled_graph.get_vertex_index(edge.first);
					int right_vertex = sampled_graph.get_vertex_index(edge.second);
					sampled_graph.add_new_edge(left_vertex, right_vertex, true);
				}
			}
			this->graph_construction_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
			start_time = clock();
			this->unnormalized_butterfly_count = (double)exact::global::kai_exact_count(sampled_graph);
			this->counting_on_sampled_graph_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
			this->runtime = this->graph_construction_time + this->counting_on_sampled_graph_time;
			return;
		}
		void algorithms::colorful_counting(Graph& G) {
			/*
				Pagh, Rasmus, and Charalampos E. Tsourakakis.
				"Colorful triangle counting and a mapreduce implementation."
				Information Processing Letters 112, no. 7 (2012): 277-281.
			*/
			clock_t start_time = clock();
			std::vector <int> vertex_color;
			int n = G.get_n_vertices();
			for (int i = 0; i < n; i++) {
				vertex_color.push_back((int)sampler::generate_random_int(0, settings::n_colors - 1));
			}
			Graph sampled_graph = Graph();
			for (auto& edge : G.get_edges()) {
				if (vertex_color[edge.first] == vertex_color[edge.second]) {
					int left_vertex = sampled_graph.get_vertex_index(edge.first);
					int right_vertex = sampled_graph.get_vertex_index(edge.second);
					sampled_graph.add_new_edge(left_vertex, right_vertex, true);
				}
			}
			this->graph_construction_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
			start_time = clock();
			this->unnormalized_butterfly_count = (double)exact::global::kai_exact_count(sampled_graph);
			this->counting_on_sampled_graph_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
			this->runtime = this->graph_construction_time + this->counting_on_sampled_graph_time;
			return;
		}


		void algorithms::run(Graph& G) {
			if (settings::chosen_algo == ONE_SHOT_DOULION) {
				this->doulion(G);
			}
			else if (settings::chosen_algo == ONE_SHOT_COLORFUL) {
				this->colorful_counting(G);
			}
		}
		std::vector<double> algorithms::get_results(const int& exp_iter) {
			double unnormalized = this->get_unnormalized_count();
			double global_butterfly_count = 0;
			if (settings::chosen_algo == ONE_SHOT_DOULION) {
				global_butterfly_count = utility::normalized(unnormalized, 1. / (settings::p * settings::p * settings::p * settings::p));
			}
			else if (settings::chosen_algo == ONE_SHOT_COLORFUL) {
				global_butterfly_count = utility::normalized(unnormalized, (settings::n_colors * settings::n_colors * settings::n_colors));
			}
			return{ (double)exp_iter, global_butterfly_count, this->get_construction_time(), this->get_counting_on_sampled_graph_time(), this->get_runtime() };
		}
	}

	/* Static graph: Approximation by local sampling */
	namespace local_sampling {
		void algorithms::fast_edge_sampling(Graph& G) {
			/*
			
			*/
			clock_t start_time = clock();
			this->n_sampled++;
			int random_edge_index = (int)sampler::generate_random_int(0, G.get_n_edges() - 1);
			std::pair<int, int>& edge = G.get_ith_edge(random_edge_index);
			int& vertex_left = edge.first;
			int& vertex_right = edge.second;
			this->unnormalized_butterfly_count += randomized::edge::randomized_ebfc(G, vertex_left, vertex_right);
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			return;
		}

		void algorithms::fast_centered_edge_sampling(Graph& G) {
			/*

			*/
			clock_t start_time = clock();
			this->n_sampled++;
			int random_edge_index = (int)sampler::generate_random_int(0, G.get_n_edges() - 1);
			std::pair<int, int>& edge = G.get_ith_edge(random_edge_index);
			int& vertex_left = edge.first;
			int& vertex_right = edge.second;
			this->unnormalized_butterfly_count += randomized::edge::randomized_centered_ebfc(G, vertex_left, vertex_right);
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			return;
		}

		void algorithms::wedge_sampling(Graph& G) {
			/*

			*/
			clock_t start_time = clock();
			this->n_sampled++;
			auto pair = sampler::weighted_sampling(G.get_cum_w());
			auto wedge = G.get_wedge(pair.second, pair.first, G.get_adj_vec(pair.first));
			this->unnormalized_butterfly_count += exact::wedge::bfly_in_wedge(G, wedge);
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			return;
		}

		void algorithms::path_sampling(Graph& G) {
			/*

			*/
			clock_t start_time = clock();
			this->n_sampled++;
			auto pair = sampler::weighted_sampling(G.get_cum_z());
			auto& edge = G.get_ith_edge(pair.first);
			int& vertex_left = edge.first;
			int& vertex_right = edge.second;
			this->unnormalized_butterfly_count += randomized::path::z_forms_bfly(G, vertex_left, vertex_right);
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			return;
		}

		void algorithms::path_centered_sampling(Graph& G) {
			/*

			*/
			clock_t start_time = clock();
			this->n_sampled++;
			auto pair = sampler::weighted_sampling(G.get_cum_centered_z());
			auto& edge = G.get_ith_edge(pair.first);
			int& vertex_left = edge.first;
			int& vertex_right = edge.second;
			this->unnormalized_butterfly_count += randomized::path::centered_z_forms_bfly(G, vertex_left, vertex_right);
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
			return;
		}

		void algorithms::path_centered_sampling_by_wedge(Graph& G) {
			/*

			*/
			this->path_centered_sampling(G);
		}

		void algorithms::path_centered_sampling_by_degeneracy(Graph& G) {
			/*

			*/
			this->path_centered_sampling(G);
		}

		void algorithms::run(Graph& G) {
			if (settings::chosen_algo == FAST_EDGE_SAMPLING) {
				this->fast_edge_sampling(G);
			}
			else if (settings::chosen_algo == FAST_CENTERED_EDGE_SAMPLING) {
				this->fast_centered_edge_sampling(G);
			}
			else if (settings::chosen_algo == WEDGE_SAMPLING) {
				this->wedge_sampling(G);
			}
			else if (settings::chosen_algo == PATH_SAMPLING) {
				this->path_sampling(G);
			}
			else if (settings::chosen_algo == PATH_CENTERED_SAMPLING) {
				this->path_centered_sampling(G);
			}
			else if (settings::chosen_algo == PATH_CENTERED_SAMPLING_BY_WEDGE) {
				this->path_centered_sampling_by_wedge(G);
			}
			else if (settings::chosen_algo == PATH_CENTERED_SAMPLING_BY_DEGENERACY) {
				this->path_centered_sampling_by_degeneracy(G);
			}
		}
		std::vector<double> algorithms::get_results(const int& iter_exp, Graph& G) {
			double unnormalized = this->get_unnormalized_count();
			double global_butterfly_count = 0;
			if (settings::chosen_algo == FAST_EDGE_SAMPLING) {
				global_butterfly_count = utility::normalized(unnormalized, (double)G.get_n_edges() / (this->get_n_sampled() * 4.0));
			}
			else if (settings::chosen_algo == FAST_CENTERED_EDGE_SAMPLING) {
				global_butterfly_count = utility::normalized(unnormalized, (double)G.get_n_edges() / (this->get_n_sampled()));
			}
			else if (settings::chosen_algo == WEDGE_SAMPLING) {
				global_butterfly_count = utility::normalized(unnormalized, (double)G.get_n_w() / (this->get_n_sampled() * 4.0));
			}
			else if (settings::chosen_algo == PATH_SAMPLING) {
				global_butterfly_count = utility::normalized(unnormalized, (double)G.get_n_z() / (this->get_n_sampled() * 4.0));
			}
			else if (settings::chosen_algo == PATH_CENTERED_SAMPLING) {
				global_butterfly_count = utility::normalized(unnormalized, (double)G.get_n_centered_z() / (this->get_n_sampled()));
			}
			else if (settings::chosen_algo == PATH_CENTERED_SAMPLING_BY_WEDGE) {
				global_butterfly_count = utility::normalized(unnormalized, (double)G.get_n_centered_z() / (this->get_n_sampled()));
			}
			else if (settings::chosen_algo == PATH_CENTERED_SAMPLING_BY_DEGENERACY) {
				global_butterfly_count = utility::normalized(unnormalized, (double)G.get_n_centered_z() / (this->get_n_sampled()));
			}
			return { (double)iter_exp, this->get_preprocessing_time(), (double)this->get_n_sampled(), global_butterfly_count, this->get_runtime() };;
		}
	}
}

namespace streaming {
	void algorithms::fleet3_ins_only(const int& A, const int& B) {
		this->time_step++;
		double inv_p = 1. / this->prob;
		long long bfly = exact::edge::ebfc(R, A, B);
		this->seen_bfly += bfly;
		this->unnormalized_butterfly_count += bfly * inv_p * inv_p * inv_p;
		if ((this->R.get_current_size() >= this->R.get_maximum_size())) {
			while (this->R.get_current_size() >= this->R.get_maximum_size()) {
				reservoir *new_R = new reservoir();
				new_R->set_max_size(this->R.get_maximum_size());
				for (auto& e : this->R.edge_pool) {
					std::pair<int, int> p = R.to_edge(e);
					int x = p.first, y = p.second;
					if (sampler::generate_random_dbl(0.0, 1.0) <= this->gamma) {
						new_R->add_edge_to_reservoir(x, y);
					}
				}
				this->R = *new_R;
				delete new_R;
			}
			this->n_full_reservoir++;
			this->prob *= this->gamma;
		}
		if (sampler::generate_random_dbl(0.0, 1.0) <= this->prob) {
			this->n_sampled_edges++;
			this->R.add_edge_to_reservoir(A, B);
		}
	}
	void algorithms::fleet3_fully_dynamic(const int& A, const int& B, const bool op) {
		this->time_step++;
		double inv_p = 1. / this->prob;
		long long bfly = exact::edge::ebfc(R, A, B);
		this->seen_bfly += bfly;
		if (op == true)
			this->unnormalized_butterfly_count += bfly * inv_p * inv_p * inv_p;
		else
			this->unnormalized_butterfly_count -= bfly * inv_p * inv_p * inv_p;
		if ((this->R.get_current_size() >= this->R.get_maximum_size())) {
			while (this->R.get_current_size() >= this->R.get_maximum_size()) {
				reservoir *new_R = new reservoir();
				new_R->set_max_size(this->R.get_maximum_size());
				for (auto& e : this->R.edge_pool) {
					std::pair<int, int> p = R.to_edge(e);
					int x = p.first, y = p.second;
					if (sampler::generate_random_dbl(0.0, 1.0) <= this->gamma) {
						new_R->add_edge_to_reservoir(x, y);
					}
				}
				this->R = *new_R;
				delete new_R;
			}
			this->n_full_reservoir++;
			this->prob *= this->gamma;
		}
		if (op == true) {
			if (sampler::generate_random_dbl(0.0, 1.0) <= this->prob) {
				this->n_sampled_edges++;
				this->R.add_edge_to_reservoir(A, B);
			}
		}
		else {
			this->R.remove_edge_from_reservoir(A, B);
		}
	}
	void algorithms::exact_dynamic_algorithm(const int& A, const int& B, const bool op) {
		if (settings::chosen_algo == EXACT_INSERTION_ONLY) {
			if (op == true) {
				this->R.add_edge_to_reservoir(A, B);
				this->time_step++;
			}
		}
		else if (settings::chosen_algo == EXACT_FULLY_DYNAMC) {
			this->time_step++;
			if (op == true) {
				this->R.add_edge_to_reservoir(A, B);
			}
			else {
				this->R.remove_edge_from_reservoir(A, B);
			}
		}
		if (this->should_print()) {
			double start_time = (double)clock();
			this->unnormalized_butterfly_count = (double)exact::global::Kai_BFC_in_reservoir(this->R);
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
		}
	}
	void algorithms::thinkd_fast(const int& A, const int& B, const bool op) {
		this->time_step++;
		double inv_p = 1. / settings::p;
		long long bfly = exact::edge::ebfc(R, A, B);
		if (op == true) {
			if (sampler::generate_random_dbl(0.0, 1.0) <= this->prob)
				this->R.add_edge_to_reservoir(A, B);
			this->unnormalized_butterfly_count += bfly * inv_p * inv_p * inv_p;
		}
		else {
			this->R.remove_edge_from_reservoir(A, B);
			this->unnormalized_butterfly_count -= bfly * inv_p * inv_p * inv_p;
		}
	}
	void algorithms::thinkd_accurate(const int& A, const int& B, const bool op) {
		this->time_step++;
		long double y = (long double)mmin(this->R.get_current_size(), this->current_n_edges + this->n_b + this->n_g);
		this->prob = 1.0;
		if (this->current_n_edges >= 3) {
			for (int z = 0; z < 3; z++) {
				this->prob *= ((y - z) / (this->current_n_edges + this->n_b + this->n_g - z)); // ** make sure if this is true
			}
		}
		else this->prob = 0.0;
		double inv_p = this->current_n_edges >= 3 ? 1. / this->prob : 0;
		long long bfly = exact::edge::ebfc(R, A, B);
		if (this->current_n_edges >= 3) {
			if (op == true)
				this->unnormalized_butterfly_count += bfly * inv_p;
			else
				this->unnormalized_butterfly_count -= bfly * inv_p;
		}
		else this->unnormalized_butterfly_count = 0.0;

		if (op == true) {
			double tossed_prob = sampler::generate_random_dbl(0.0, 1.0);
			this->current_n_edges++;
			if (this->n_b + this->n_g == 0) {
				if (this->R.get_current_size() < settings::res_size) this->R.add_edge_to_reservoir(A, B);
				else if (tossed_prob * this->current_n_edges <= settings::res_size) {
					this->R.replace_in_reservoir(A, B);
				}
			}
			else if (tossed_prob <= ((double)this->n_b) / ((double)this->n_b + this->n_g)) {
				this->R.add_edge_to_reservoir(A, B);
				this->n_b--;
			}
			else {
				this->n_g--;
			}
		}
		else {
			this->current_n_edges--;
			if (this->R.remove_edge_from_reservoir(A, B) == true)
				this->n_b++; // bad uncompensated deletion as the edge appears in the stream
			else
				this->n_g++; // good uncompensated deletion
		}
	}
	void algorithms::rs_with_deletion(const int& A, const int& B, const bool op) {
		this->time_step++;
		double M = this->R.get_maximum_size();
		long long bfly = exact::edge::ebfc(R, A, B);
		long double factor = mmax(1.0, (this->c / M) * (this->c - 1) / (M - 1.0) * (this->c - 2.0) / (M - 2.0));
		if (op == false) {
			this->unnormalized_butterfly_count -= factor * bfly;
			this->R.remove_edge_from_reservoir(A, B);
			// The following is an optimization when reservoir gets emptied. 
			// Check if this operation is true!
			if (R.get_current_size() == 0) {
				this->c = 0;
			}
		}
		else {
			this->c = this->c + 1;
			this->unnormalized_butterfly_count += factor * bfly;
			if (sampler::generate_random_dbl(0.0, 1.0) <= M / this->c) {
				this->R.replace_in_reservoir_with_fixed_size(A, B);
			}
		}
	}
	void algorithms::improved_rs_with_deletion(const int& A, const int& B, const bool op) {
		this->time_step++;
		double M = this->R.get_maximum_size();
		long long bfly = exact::edge::ebfc(R, A, B);
		long double factor = mmax(1, (this->n_inserted / M) * ((this->n_inserted - 1.0) / (M - 1.0)) * ((this->n_inserted - 2.0) / (M - 2.0)));
		//long double factor = mmax(1, this->n_inserted / M * (this->n_inserted - 1.0) / (M - 1.0) * (this->n_inserted - 2.0) / (M - 2.0));
		if (op == false) { // a deletion occurs
			this->unnormalized_butterfly_count -= factor * bfly;
			this->R.remove_edge_from_reservoir(A, B);
			// Check if this operation is true!
			if (R.get_current_size() == 0) {
				this->c = this->R.get_maximum_size();
				this->n_inserted = 0;
			}
		}
		else { // an insertion 
			this->n_inserted++;
			this->unnormalized_butterfly_count += factor * bfly;
			if (this->R.get_current_size() == this->R.get_maximum_size()) {
				this->c = this->c + 1;
				if (sampler::generate_random_dbl(0.0, 1.0) <= M / this->c) {
					this->R.replace_in_reservoir(A, B);
				}
			}
			else if (sampler::generate_random_dbl(0.0, 1.0) <= M / this->c) { // here, reservoir is not full.
				this->R.add_edge_to_reservoir(A, B);
			}
		}
	}
	long long algorithms::total_work() {
		long long ret = 0;
		if (settings::has_fixed_gamma() == true) {
			for (int v = settings::min_res_size; v <= settings::max_res_size; v <<= 1) {
				ret += (long long)((settings::max_gamma - settings::min_gamma) / 0.1 + 1);
			}
		}
		else if (settings::has_fixed_probability() == true) {
			for (double v = settings::min_p; v <= settings::max_p; v *= 2) {
				ret++;
			}
		}
		else if (settings::has_fixed_reservoir() == true && settings::has_fixed_gamma() == false) {
			for (int v = settings::min_res_size; v <= settings::max_res_size; v <<= 1) {
				ret++;
			}
		}
		return ret;
	}
	void algorithms::run(const int& A, const int& B, const bool op) {
		if (settings::chosen_algo == FLEET3) {
			double start_time = (double)clock();
			this->fleet3_ins_only(A, B);
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
		}
		else if (settings::chosen_algo == EXACT_INSERTION_ONLY) {
			this->exact_dynamic_algorithm(A, B, true);
		}
		else if (settings::chosen_algo == FLEET3_FULLY_DYNAMIC) {
			double start_time = (double)clock();
			this->fleet3_fully_dynamic(A, B, op);
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
		}
		else if (settings::chosen_algo == EXACT_FULLY_DYNAMC) {
			this->exact_dynamic_algorithm(A, B, op);
		}
		else if (settings::chosen_algo == THINKD_FAST) {
			double start_time = (double)clock();
			this->thinkd_fast(A, B, op);
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
		}
		else if (settings::chosen_algo == THINKD_ACC) {
			double start_time = (double)clock();
			this->thinkd_accurate(A, B, op);
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
		}
		else if (settings::chosen_algo == RS_WITH_DELETION) {
			double start_time = (double)clock();
			this->rs_with_deletion(A, B, op);
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
		}
		else if (settings::chosen_algo == IMPR_RS_WITH_DELETION) {
			double start_time = (double)clock();
			this->improved_rs_with_deletion(A, B, op);
			this->runtime += ((double)clock() - start_time) / CLOCKS_PER_SEC;
		}
		if (settings::is_exact_algorithm() == false) {
			this->reservoir_use += this->R.get_current_size();
		}
	}
	std::vector<double> algorithms::get_results(const int& exp_iter) {
		double unnormalized = this->get_unnormalized_count();
		double global_butterfly_count = 0;
		if (settings::is_exact_algorithm() == true) {
			global_butterfly_count = unnormalized;
			return{ global_butterfly_count, this->get_runtime(), (double)this->time_step };
		}
		else {
			global_butterfly_count = unnormalized;
			return{ (double)exp_iter, global_butterfly_count, this->get_runtime(), (double)this->time_step, (double)this->reservoir_use };
		}
	}
}
