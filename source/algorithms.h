#pragma once

#include <iomanip>

#include "Graph.h"
#include "counting.h"
#include "sampler.h"
#include "reservoir.h"

namespace base_algorithm_class {
	class algorithms {
	protected:
		double unnormalized_butterfly_count;
		double runtime;
		double last_time_print;
	public:
		algorithms() {
			this->reset();
		}
		inline double get_runtime() {
			return this->runtime;
		}
		virtual void reset() {
			this->unnormalized_butterfly_count = 0;
			this->runtime = 0;
			this->last_time_print = 0;
		}
		double get_unnormalized_count() {
			return this->unnormalized_butterfly_count;
		}
		inline bool should_screen_print() {
			if (((double)clock() - this->last_time_print) > 3.0 * CLOCKS_PER_SEC) {
				this->last_time_print = (double)clock();
				return true;
			}
			return false;
		}
	};
}

namespace exact {
	class algorithms : public base_algorithm_class::algorithms {
	public:
		void exact_algorithm(Graph& G);
	};
}

namespace static_processing {
	namespace one_shot {
		class algorithms : public base_algorithm_class::algorithms {
		private:
			/* Static graph: Approximation by one shot sampling */
			void doulion(Graph& G);
			void colorful_counting(Graph& G);
			double graph_construction_time;
			double counting_on_sampled_graph_time;
		public:
			void run(Graph& G);
			void reset() override {
				base_algorithm_class::algorithms::reset();
				this->graph_construction_time = 0;
				this->counting_on_sampled_graph_time = 0;
			}
			inline double get_construction_time() {
				return this->graph_construction_time;
			}
			inline double get_counting_on_sampled_graph_time() {
				return this->counting_on_sampled_graph_time;
			}
			std::vector<double> get_results(const int& iter_exp);
		};
	}

	namespace local_sampling {
		class algorithms : public base_algorithm_class::algorithms {
		private:
			double preprocessing_time;
			double interval_time;
			double last_time_printed;
			long long n_sampled;
			/* Static graph: Approximation by local sampling */
			void fast_edge_sampling(Graph& G);
			void fast_centered_edge_sampling(Graph& G);
			void wedge_sampling(Graph& G);
			void path_sampling(Graph& G);
			void path_centered_sampling(Graph& G);
			void path_centered_sampling_by_wedge(Graph& G);
			void path_centered_sampling_by_degeneracy(Graph& G);
		public:
			void setup(const int& iter_exp, Graph& G) {
				this->reset();
				this->n_sampled = 0;
				this->last_time_printed = 0;
				this->interval_time = mmax(1, settings::max_time / settings::snapshots);
				
				if (iter_exp == 1) {
					if (settings::chosen_algo == FAST_EDGE_SAMPLING) {
						this->preprocessing_time = 0;
					}
					else if (settings::chosen_algo == FAST_CENTERED_EDGE_SAMPLING) {
						clock_t start_time = clock();
						G.sort_vertices_by_degree(); // Question: why edges are kept the same as vanilla fast edge sampling?
						this->preprocessing_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
					}
					else if (settings::chosen_algo == WEDGE_SAMPLING) {
						clock_t start_time = clock();
						G.get_n_w();
						this->preprocessing_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
					}
					else if (settings::chosen_algo == PATH_SAMPLING) {
						clock_t start_time = clock();
						G.get_n_z();
						this->preprocessing_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
					}
					else if (settings::chosen_algo == PATH_CENTERED_SAMPLING) {
						clock_t start_time = clock();
						G.sort_vertices_by_degree();
						G.get_n_centered_z();
						this->preprocessing_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
					}
					else if (settings::chosen_algo == PATH_CENTERED_SAMPLING_BY_WEDGE) {
						clock_t start_time = clock();
						G.get_n_z();
						G.sort_vertices_by_wedges();
						this->preprocessing_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
					}
					else if (settings::chosen_algo == PATH_CENTERED_SAMPLING_BY_DEGENERACY) {
						clock_t start_time = clock();
						G.sort_vertices_by_degeneracy();
						G.get_n_centered_z();
						this->preprocessing_time = ((double)clock() - start_time) / CLOCKS_PER_SEC;
					}
				}
			}
			inline void update_last_time_printed(const double& current_time) {
				this->last_time_printed = current_time;
			}
			inline bool should_print(const double& current_time) {
				return current_time - this->last_time_printed >= this->interval_time;
			}
			inline double get_preprocessing_time() {
				return this->preprocessing_time;
			}
			inline long long get_n_sampled() {
				return this->n_sampled; 
			}
			void run(Graph& G);
			std::vector<double> get_results(const int& iter_exp, Graph& G);
		};
	}
}

namespace streaming {
	class algorithms : public base_algorithm_class::algorithms {
		private:
			/* Static graph: Approximation by one shot sampling */
			reservoir R;
			long long time_step;
			long long stream_size;
			long long seen_bfly;
			double prob;
			double gamma;
			long long n_full_reservoir;
			long long n_sampled_edges;
			int interval_batch;
			long long n_b, n_g;
			long long n_inserted;
			long long c;
			long long current_n_edges;
			long long reservoir_use;
			double last_time_print;
			void fleet3_ins_only(const int& A, const int& B);
			void fleet3_fully_dynamic(const int& A, const int& B, const bool op);
			void exact_dynamic_algorithm(const int& A, const int& B, const bool op);
			void thinkd_fast(const int& A, const int& B, const bool op);
			void thinkd_accurate(const int& A, const int& B, const bool op);
			void rs_with_deletion(const int& A, const int& B, const bool op);
			void improved_rs_with_deletion(const int& A, const int& B, const bool op);
		public:
			algorithms() {
				R = *new reservoir();
				R.set_max_size(settings::res_size);
				this->reset();
			}
			inline void reset() override {
				base_algorithm_class::algorithms::reset();
				this->time_step = 0;
				this->seen_bfly = 0;
				this->prob = 1.0;
				this->n_full_reservoir = 0;
				this->n_sampled_edges = 0;
				this->n_b = 0;
				this->n_g = 0;
				this->n_inserted = 0;
				if(settings::chosen_algo == RS_WITH_DELETION) this->c = 0;
				else if(settings::chosen_algo == IMPR_RS_WITH_DELETION) this->c = settings::res_size;
				this->current_n_edges = 0;
				this->gamma = settings::gamma;
				this->last_time_print = 0;
				this->reservoir_use = 0LL;
				this->R.clear();
				if (settings::has_fixed_reservoir() == true) {
					this->R.set_max_size(settings::res_size);
				}
			}
			inline void set_interval(Graph& g) {
				if (settings::is_only_insertion_algo())
					this->interval_batch = mmax(1, ((int)g.get_ins_only_stream().size()) / settings::snapshots);
				else if (settings::is_fully_dynamic_streaming_algo())
					this->interval_batch = mmax(1, ((int)g.get_fully_dyn_stream().size()) / settings::snapshots);
			}
			inline bool should_print() {
				return this->time_step == this->stream_size || this->interval_batch == 1 || (this->time_step > 0 && this->time_step % this->interval_batch == 0);
			}
			void set_stream_size(Graph& G) {
				if (settings::is_only_insertion_algo())
					this->stream_size = G.get_ins_only_stream().size();
				else if (settings::is_fully_dynamic_streaming_algo()) {
					this->stream_size = G.get_fully_dyn_stream().size();
				}
			}
			long long total_work();
			inline long long get_stream_size() { return this->stream_size;  }
			std::vector<double> get_results(const int& iter_exp = -1);
			void run(const int& A, const int& B, const bool op = true);
	};
}
