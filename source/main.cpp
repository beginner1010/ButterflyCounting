#define _CRT_SECURE_NO_WARNINGS

#include "Graph.h"
#include "algorithms.h"
#include "counting.h"
#include "IO.h"
#include "utills.h"
#include "printing.h"

int main() {
	std::ios::sync_with_stdio(false);

	IO::IO_addresses();
	IO::fin = fopen(IO::input_address.c_str(), "r");
	Graph graph = Graph();

	settings::is_static_or_streaming();
	graph.read_from_file();
	std::fclose(IO::fin);

	//ins / del
	//auto& cur_stream = graph.get_fully_dyn_stream();
	//int ins = 0;
	//int del = 0;
	//int stream_size = (int)cur_stream.size();
	//int interval_batch = mmax(1, ((int)stream_size) / 100);
	//int time_step = 0;
	//IO::fout = fopen((IO::input_address + "-dyn").c_str(), "w");
	//std::vector< std::pair<int,int> > dyn_edges;
	//for (auto& item : cur_stream) {
		//dyn_edges.push_back(item.first);
	//}
	//int len_pre = (int)dyn_edges.size();
	//bool yes = false;
	//for(int i = 0; i < (int)dyn_edges.size(); i ++) {
		//if(sampler::generate_random_dbl(0.0, 1.0) <= 0.2) {
			//if(i >= len_pre) yes = true;
			//dyn_edges.push_back(dyn_edges[i]);
			//del ++;
		//}
	//}
	//std::cerr << "Yes? " <<  yes << std::endl;
	//std::random_shuffle(dyn_edges.begin(), dyn_edges.end());
	//for(auto& edge: dyn_edges) {
		//fprintf(IO::fout, "%d %d\n", edge.first, edge.second);
	//}
	//std::cerr << del << " " << (int)cur_stream.size() << " " << (int)dyn_edges.size() << std::endl;
	//return 0;

	if(settings::is_static == true) 
		print::statistics_table(graph.get_n_edges(), graph.get_n_left_vertices(), graph.get_n_right_vertices(), graph.get_maximum_degree());

	bool run = true;
	while (run) {
		if(settings::is_static == true) 
			graph.get_original_order();
		sampler::reset();
		settings::get_settings();
		IO::create_folder();

		exact::algorithms* exact_algo = new exact::algorithms();
		static_processing::one_shot::algorithms* static_one_shot_algo = new static_processing::one_shot::algorithms();
		static_processing::local_sampling::algorithms* static_local_samp_algo = new static_processing::local_sampling::algorithms();
		streaming::algorithms* streaming_algo = new streaming::algorithms();

		streaming_algo->set_stream_size(graph);
		if (settings::is_static == false) {
			print::statistics_table(streaming_algo->get_stream_size());
		}

		IO::fout = fopen(IO::output_address.c_str(), "w");
		print::print_header();
		if (settings::is_exact_algorithm()) {
			if (settings::is_static == true) {
				exact_algo->reset();
				exact_algo->exact_algorithm(graph);
				auto global_butterfly_count = exact_algo->get_unnormalized_count(); // unnormalized count in this case is indeed a normalized one since the counter is exact.
				print::print_result({ global_butterfly_count, exact_algo->get_runtime() });
			} else {
				streaming_algo->set_interval(graph);
				streaming_algo->set_stream_size(graph);
				int done_so_far = 0;
				double total_work = streaming_algo->get_stream_size();
				if (settings::is_only_insertion_algo() == true) {
					auto& stream = graph.get_ins_only_stream();
					for (auto& edge : stream) {
						streaming_algo->run(edge.first, edge.second);
						if (streaming_algo->should_print()) {
							print::print_result(streaming_algo->get_results());
						}
						if (streaming_algo->should_screen_print() == true) {
							print::clear_line();
							print::done_work_percentage(done_so_far, total_work, "insertion-only streaming algorithm");
						}
						done_so_far++;
					}
				}
				else if (settings::is_fully_dynamic_streaming_algo() == true) {
					auto& stream = graph.get_fully_dyn_stream();
					for (auto& edge : stream) {
						streaming_algo->run(edge.first.first, edge.first.second, edge.second);
						if (streaming_algo->should_print()) {
							print::print_result(streaming_algo->get_results());
						}
						if (streaming_algo->should_screen_print() == true) {
							print::clear_line();
							print::done_work_percentage(done_so_far, total_work, "fully-dyanmic streaming algorithm");
						}
						done_so_far++;
					}
				}
			}
		}
		else {
			for (int iter_exp = 1; iter_exp <= settings::exp_repeatition; iter_exp++) {
				if (settings::is_one_shot_algorithm() == true) {
					while (true) {
						static_one_shot_algo->reset();
						static_one_shot_algo->run(graph);
						print::print_result(static_one_shot_algo->get_results(iter_exp));
						if (settings::next_parameter() == -1) // a parameter (prob or n_color) is changed here!
							break;
					}
					print::clear_line();
					print::done_experiments(iter_exp);
				} else if (settings::is_local_sampling_algorithm() == true) {
					static_local_samp_algo->setup(iter_exp, graph);
					double tolerance = settings::max_time / settings::snapshots - 1e-6;
					while (static_local_samp_algo->get_runtime() <= settings::max_time + tolerance) {
						static_local_samp_algo->run(graph);
						if (static_local_samp_algo->should_print(static_local_samp_algo->get_runtime())) {
							static_local_samp_algo->update_last_time_printed(static_local_samp_algo->get_runtime());
							print::print_result(static_local_samp_algo->get_results(iter_exp, graph));
						}
						if (static_local_samp_algo->should_screen_print()) {
							print::clear_line();
							print::done_experiments(iter_exp);
							print::done_work_percentage(static_local_samp_algo->get_runtime(), settings::max_time, "local sampling");
						}
					}
				} else if (settings::is_streaming() == true) {
					double total_work = (double)streaming_algo->total_work();
					total_work *= streaming_algo->get_stream_size();
					streaming_algo->set_interval(graph);
					streaming_algo->set_stream_size(graph);
					double done_so_far = 0;
					do {
						streaming_algo->reset();
						if (settings::is_only_insertion_algo()) {
							auto& stream = graph.get_ins_only_stream();
							for (auto& edge : stream) {
								streaming_algo->run(edge.first, edge.second);
								if (streaming_algo->should_print()) {
									print::print_result(streaming_algo->get_results(iter_exp));
								}
								if(streaming_algo->should_screen_print()) {
									print::clear_line();
									print::done_experiments(iter_exp);
									print::done_work_percentage(done_so_far, total_work, "insertion-only streaming algorithm");
								}
								done_so_far++;
							}
						}
						else {
							auto& stream = graph.get_fully_dyn_stream();
							for (auto& edge : stream) {
								streaming_algo->run(edge.first.first, edge.first.second, edge.second);
								if (streaming_algo->should_print()) {
									print::print_result(streaming_algo->get_results(iter_exp));
								}
								if (streaming_algo->should_screen_print()) {
									print::clear_line();
									print::done_experiments(iter_exp);
									print::done_work_percentage(done_so_far, total_work, "fully-dyanmic streaming algorithm");
								}
								done_so_far++;
							}
						}
					} while (settings::next_parameter() != -1);
				}
			}
		}
		print::clear_line();
		std::cerr << " The experiment is finished. Look at for results: " << IO::output_address << std::endl << std::endl;
		run = settings::continue_run();
	}
	std::fclose(IO::fout);
}
