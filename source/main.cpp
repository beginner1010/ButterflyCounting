#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <random>
#include <cstdio>
#include <sstream>
#include <sys/stat.h>
#ifdef _WIN32
	#include <direct.h>
#endif
#ifdef _WIN32
	#include <io.h> 
	#define access    _access_s
#else
	#include <unistd.h>
#endif

#include "Graph.h"
#include "algorithm.h"

std::mt19937_64 gen;
std::uniform_real_distribution<double> dbl_ran;
std::uniform_int_distribution<int> int_ran;

#define EXACT "1"
#define FAST_EDGE_SAMPLING "2"
#define FAST_CENTERED_EDGE_SAMPLING "3"
#define PATH_SAMPLING "4" 
#define PATH_CENTERED_SAMPLING "5" 

std::string folder_algo_name[] = { "exact", "fes", "fces", "ps", "pcs" };

bool check_if_input_exists(std::string& input_address) {
	if (access(input_address.c_str(), 0) != 0) {
		std::cerr << " No such file exists!" << std::endl;	
		return false;
	}
	return true;
}

std::string to_str(int x) { std::stringstream ss; ss << x;  return ss.str();}

std::string create_folder(int max_time, std::string algo, std::string input_address, int ebfc_iteration, int exp_repeatition) {
	std::string algo_name = folder_algo_name[algo[0] - '1'];
#ifdef _WIN32
	::_mkdir("./output");
	::_mkdir(("./output/"+input_address).c_str());
	::_mkdir(("./output/" + input_address + "/" + algo_name).c_str());
#else
	::mkdir("./output", 0777);
	::mkdir(("./output/" + input_address).c_str(), 0777);
	::mkdir(("./output/" + input_address + "/" + algo_name).c_str(), 0777);
#endif
	std::string file_name = ebfc_iteration == -1 ? "" : "ebfc=" + to_str(ebfc_iteration);
	file_name += max_time == -1 ? "" : "t=" + to_str(max_time);
	file_name += "rep=" + to_str(exp_repeatition);
	file_name = file_name == "" ? "exact" : file_name;
	std::string output_address = "output/" + input_address + "/" + algo_name + "/" + file_name + ".txt";
	freopen(output_address.c_str(), "w", stdout);
	return output_address;
}

int main() {
	std::random_device rd;
	gen.seed(rd());

	std::ios::sync_with_stdio(false);

	std::unordered_set<std::string> algorithm_names({ EXACT, FAST_EDGE_SAMPLING, FAST_CENTERED_EDGE_SAMPLING,
		PATH_SAMPLING, PATH_CENTERED_SAMPLING});
	int ebfc_iterations = -1;
	int snapshots;
	long double max_time = -1;
	int exp_repeatition = -1;
	std::string chosen_algo;
	std::string input_address;
	std::string input_file_name;

	while (algorithm_names.find(chosen_algo) == algorithm_names.end()) {
		if (chosen_algo != "") {
			std::cerr << " The algorithm is not found!" << std::endl;
		}
		std::cerr << " Insert a number corresponding to an algorithm, shown below:" << std::endl;
		std::cerr << "\t[1] exact" << std::endl;
		std::cerr << "\t[2] fast edge sampling" << std::endl;
		std::cerr << "\t[3] fast centered edge sampling" << std::endl;
		std::cerr << "\t[4] path sampling" << std::endl;
		std::cerr << "\t[5] path centered sampling" << std::endl;
		std::cerr << " >>> "; std::cin >> chosen_algo;
	}

	if (chosen_algo == FAST_EDGE_SAMPLING || chosen_algo == FAST_CENTERED_EDGE_SAMPLING) {
		std::cerr << " Insert # iterations for randomized ebfc:" << std::endl;
		std::cerr << " >>> "; std::cin >> ebfc_iterations;
	}
	if (chosen_algo != EXACT) {
		std::cerr << " Insert #repeatition of the experiments:" << std::endl;
		std::cerr << " >>> "; std::cin >> exp_repeatition;

		std::cerr << " Insert the execution time (in seconds):" << std::endl;
		std::cerr << " >>> "; std::cin >> max_time;

		std::cerr << " Insert # output snapshots:" << std::endl;
		std::cerr << " >>> "; std::cin >> snapshots;
	}

	do {
		std::cerr << " Insert an input file (bipartite network) location:" << std::endl;
		std::cerr << " >>> "; std::cin >> input_file_name;
#ifdef OFFICE_PC
		input_address = "input/in." + input_file_name;
#else // on Cyence
		input_address = "/work/snt-free/bfly/in." + input_file_name;
#endif
	} while (check_if_input_exists(input_address) == false);
	
	
	std::string output_address = create_folder(max_time > 0 ? (int)(max_time + 1e-9): -1, chosen_algo, input_file_name, ebfc_iterations, exp_repeatition);
	
	FILE *f_in = freopen(input_address.c_str(), "r", stdin);
	Graph g = Graph();
	g.read_from_file();
	std::fclose(f_in);


	algorithm* algo = new algorithm();


	if (chosen_algo == EXACT) {
		printf("bfly, algo, time\n");
		algo->exact(g);
	}
	else {
		for (int iter_exp = 1; iter_exp <= exp_repeatition; iter_exp++) {
			if (chosen_algo == FAST_EDGE_SAMPLING) {
				algo->fast_edge_sampling(g, iter_exp, exp_repeatition, ebfc_iterations, max_time, snapshots);
			}
			else if (chosen_algo == FAST_CENTERED_EDGE_SAMPLING) {
				algo->fast_centered_edge_sampling(g, iter_exp, exp_repeatition, ebfc_iterations, max_time, snapshots);
			}
			else if (chosen_algo == PATH_SAMPLING) {
				algo->path_sampling(g, iter_exp, exp_repeatition, max_time, snapshots);
			}
			else if (chosen_algo == PATH_CENTERED_SAMPLING) {
				algo->path_centered_sampling(g, iter_exp, exp_repeatition, max_time, snapshots);
			}
		}
		std::cerr << "\r" << std::string(60, ' ') ;
		std::cerr << "\r The experiment is finished. Look at: " << output_address << std::endl;
	}
}