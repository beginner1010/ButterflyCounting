#include "utills.h"


namespace constants {
	std::vector<std::string> algorithm_names = 
	{ 
		EXACT, 
		ONE_SHOT_COLORFUL,
		ONE_SHOT_DOULION,
		FAST_EDGE_SAMPLING,
		WEDGE_SAMPLING,
		FAST_CENTERED_EDGE_SAMPLING, PATH_SAMPLING,
		PATH_CENTERED_SAMPLING, PATH_CENTERED_SAMPLING_BY_WEDGE,
		PATH_CENTERED_SAMPLING_BY_DEGENERACY,
		EXACT_INSERTION_ONLY,
		EXACT_FULLY_DYNAMC,
		FLEET3,
		FLEET3_FULLY_DYNAMIC,
		THINKD_FAST,
		THINKD_ACC,
		RS_WITH_DELETION,
		IMPR_RS_WITH_DELETION
	};
	std::unordered_map<std::string, std::string> suffix_output_address =
	{
		{ EXACT, "exact" },
		{ ONE_SHOT_DOULION, "doulion" },
		{ ONE_SHOT_COLORFUL, "colorful" },
		{ FAST_EDGE_SAMPLING, "fast_edge" },
		{ FAST_CENTERED_EDGE_SAMPLING, "c_fast_edge" },
		{ PATH_SAMPLING, "path" },
		{ PATH_CENTERED_SAMPLING , "c_path" },
		{ PATH_CENTERED_SAMPLING_BY_WEDGE , "c_path_ord_wedge" },
		{ WEDGE_SAMPLING, "wedge" },
		{ PATH_CENTERED_SAMPLING_BY_DEGENERACY, "c_path_ord_degen" },
		{ EXACT_INSERTION_ONLY, "exact_ins_only" },
		{ EXACT_FULLY_DYNAMC, "exact_full_dyn" },
		{ FLEET3, "fleet3_ins_only" },
		{ FLEET3_FULLY_DYNAMIC, "fleet3_full_dyn" },
		{ THINKD_FAST, "thinkd_fast" },
		{ THINKD_ACC, "thinkd_acc" },
		{ RS_WITH_DELETION, "rs_with_del" },
		{ IMPR_RS_WITH_DELETION, "impr_rs_with_del" }
	};
	int ebfc_iterations = 2000;
}

namespace settings {
	bool is_static;
	int snapshots;
	long double max_time = -1;
	int exp_repeatition = -1;
	int n_colors = -1;
	double p = -1;
	double min_p, max_p;
	int min_clr, max_clr;
	double min_gamma, max_gamma, gamma;
	int min_res_size, max_res_size, res_size;
	std::string chosen_algo;
	std::vector<std::string>::iterator itr;

	void is_static_or_streaming() {
		std::cerr << " Is this a static graph?(y/n) ** \"n\" as answer means the graph is streaming **" << std::endl;
		settings::is_static = helper_functions::yesno_query();
	}

	void get_settings() {
		settings::clear_settings();

		std::string fixed_chosen_algo = settings::chosen_algo;
		std::unordered_set<int> allowed_algo;
		std::cerr << " Insert a number corresponding to an algorithm, listed below:" << std::endl;
		for (int i = 0; i < (int)constants::algorithm_names.size(); i++) {
			settings::chosen_algo = constants::algorithm_names[i];
			if (is_static != settings::is_streaming()) {
				fprintf(stderr, "\t[%d] %s\n", i + 1, constants::algorithm_names[i].c_str());
				allowed_algo.insert(i + 1);
			}
			else {
				fprintf(stderr, "\t[x] %s\n", constants::algorithm_names[i].c_str());
			}
		}
		settings::chosen_algo = fixed_chosen_algo;
		while (true) {
			if (settings::chosen_algo != "") {
				std::cerr << " *** The algorithm is not found!" << std::flush;
				std::cerr << " Insert a number corresponding to an algorithm, listed above:" << std::endl;
			}			std::cerr << " >>> "; std::cin >> settings::chosen_algo;
			if (helper_functions::is_int_num(settings::chosen_algo)) {
				int idx = helper_functions::to_int(settings::chosen_algo);
				if (allowed_algo.find(idx) != allowed_algo.end()) {
					settings::chosen_algo = constants::algorithm_names[idx - 1];
					break;
				}
			}
		}

		if (settings::is_exact_algorithm() == false) {
			std::cerr << " Insert #repeatition of the experiments:" << std::endl;
			std::cerr << " >>> "; std::cin >> settings::exp_repeatition;
		}

		if (settings::has_fixed_probability() == true) {
			std::cerr << " Insert the sampling probability (0.0, 1.0):" << std::endl;
			std::string line;
			std::cerr << " >>> "; std::cin >> line;
			if (line.find(',') != std::string::npos) {
				line[line.find(',')] = ' ';
				std::stringstream ss; ss << line;
				ss >> settings::min_p >> settings::max_p;
				settings::p = settings::min_p;
			}else {
				settings::p = helper_functions::to_double(line);
				settings::min_p = settings::max_p = settings::p; 
			}
			assert(0.0 < settings::min_p && settings::max_p < 1.0);
		}

		if (settings::chosen_algo == ONE_SHOT_COLORFUL) {
			std::cerr << " Insert #colors for painting vertices:" << std::endl;
			std::string line;
			std::cerr << " >>> "; std::cin >> line;
			if (line.find(',') != std::string::npos) {
				line[line.find(',')] = ' ';
				std::stringstream ss; ss << line;
				ss >> settings::min_clr >> settings::max_clr;
				settings::n_colors = settings::max_clr;
			}
			else {
				settings::n_colors = helper_functions::to_int(line);
				settings::min_clr = settings::max_clr = settings::n_colors;
			}
			assert(settings::min_clr >= 1);
		}

		if (settings::is_local_sampling_algorithm() == true) {
			std::cerr << " Insert the execution time (in seconds):" << std::endl;
			std::cerr << " >>> "; std::cin >> settings::max_time;
			assert(settings::max_time >= 1.0);
		}

		if (settings::has_fixed_reservoir() == true) {
			std::cerr << " Insert range for reservoir size:" << std::endl;
			std::string line;
			std::cerr << " >>> "; std::cin >> line;
			if (line.find(',') != std::string::npos) {
				line[line.find(',')] = ' ';
				std::stringstream ss; ss << line;
				ss >> settings::min_res_size >> settings::max_res_size;
				settings::res_size = settings::min_res_size;
				assert(settings::min_res_size <= settings::max_res_size);
			}
			else {
				settings::res_size = helper_functions::to_int(line);
				settings::min_res_size = settings::max_res_size = settings::res_size;
			}
			assert(settings::res_size > 0);
		}

		if (settings::has_fixed_gamma() == true) {
			std::cerr << " Insert range for gamma of fleet3 (0.0, 1.0):" << std::endl;
			std::string line;
			std::cerr << " >>> "; std::cin >> line;
			if (line.find(',') != std::string::npos) {
				line[line.find(',')] = ' ';
				std::stringstream ss; ss << line;
				ss >> settings::min_gamma >> settings::max_gamma;
				settings::gamma = settings::min_gamma;
			}
			else {
				settings::gamma = helper_functions::to_int(line);
				settings::min_gamma = settings::max_gamma = settings::gamma;
			}
			assert(0.0 < settings::min_gamma && settings::max_gamma < 1.0);
		}

		if (settings::is_local_sampling_algorithm() == true) {
			//I removed this setting as I would like to print output files simply every second.
			settings::snapshots = (int)(settings::max_time + 1e-6);
		}
		if (settings::is_streaming() == true) {
			std::cerr << " Insert # snapshots (# lines in output per experiment):" << std::endl;
			std::cerr << " >>> "; std::cin >> settings::snapshots;
			assert(0 < settings::snapshots);
		}
	}

	bool is_exact_algorithm() {
		return std::unordered_set<std::string>({ EXACT, EXACT_FULLY_DYNAMC, EXACT_INSERTION_ONLY }).count(settings::chosen_algo) > 0;
	}

	bool has_fixed_probability() {
		return std::unordered_set<std::string>({ ONE_SHOT_DOULION, THINKD_FAST }).count(settings::chosen_algo) > 0;
	}

	bool has_fixed_gamma() {
		return std::unordered_set<std::string>({ FLEET3, FLEET3_FULLY_DYNAMIC }).count(settings::chosen_algo) > 0;
	}

	bool has_fixed_reservoir() {
		return std::unordered_set<std::string>({ FLEET3, FLEET3_FULLY_DYNAMIC, THINKD_ACC, RS_WITH_DELETION, IMPR_RS_WITH_DELETION }).count(settings::chosen_algo) > 0;
	}

	bool is_one_shot_algorithm() {
		return std::unordered_set<std::string>({
			ONE_SHOT_DOULION,
			ONE_SHOT_COLORFUL
		}).count(settings::chosen_algo) > 0;
	}

	bool is_only_insertion_algo() {
		return std::unordered_set<std::string>({
			EXACT_INSERTION_ONLY,
			FLEET3
		}).count(settings::chosen_algo) > 0;
	}

	bool is_fully_dynamic_streaming_algo() {
		return std::unordered_set<std::string>({
			EXACT_FULLY_DYNAMC,
			FLEET3_FULLY_DYNAMIC,
			THINKD_FAST,
			THINKD_ACC,
			RS_WITH_DELETION,
			IMPR_RS_WITH_DELETION
		}).count(settings::chosen_algo) > 0;
	}

	bool is_local_sampling_algorithm() {
		return std::unordered_set<std::string>({ 
			FAST_EDGE_SAMPLING, 
			FAST_CENTERED_EDGE_SAMPLING,
			WEDGE_SAMPLING,
			PATH_SAMPLING,
			PATH_CENTERED_SAMPLING,
			PATH_CENTERED_SAMPLING_BY_WEDGE,
			PATH_CENTERED_SAMPLING_BY_DEGENERACY
		}).count(settings::chosen_algo) > 0;
	}

	bool is_streaming() {
		return std::unordered_set<std::string>({
			EXACT_INSERTION_ONLY,
			EXACT_FULLY_DYNAMC,
			FLEET3,
			FLEET3_FULLY_DYNAMIC,
			THINKD_FAST,
			THINKD_ACC,
			RS_WITH_DELETION,
			IMPR_RS_WITH_DELETION
		}).count(settings::chosen_algo) > 0;
	}

	void clear_settings() {
		max_time = -1;
		exp_repeatition = -1;
		chosen_algo = "";
	}

	bool continue_run() {
		std::cerr << " Would you like to try experiments for the same graph but different algorithms/settings?(y/n)" << std::endl;
		return helper_functions::yesno_query();
	}
	int next_parameter() {
		if (settings::has_fixed_probability() == true) {
			if (helper_functions::is_equal(settings::p, settings::max_p)) {
				settings::p = settings::min_p;
				return -1;
			}
			else {
				settings::p = mmin(settings::p * 2, settings::max_p);
				return 0;
			}
		}
		else if (settings::chosen_algo == ONE_SHOT_COLORFUL) {
			if (settings::n_colors == settings::min_clr) {
				settings::n_colors = settings::max_clr; // return it back to the initial value
				return -1;
			}
			else {
				settings::n_colors = mmax(settings::min_clr, settings::n_colors >> 1);
				return 0;
			}
		}
		else if (settings::has_fixed_gamma() == true) { // should be a FLEET algorithm. It takes two parameters -- gamma and reservoir size
			if (helper_functions::is_equal(settings::gamma, settings::max_gamma) == true) {
				settings::gamma = settings::min_gamma;
				if (settings::res_size == settings::max_res_size) {
					settings::res_size = settings::min_res_size;
					return -1;
				}
				else {
					settings::res_size = mmin(settings::res_size * 2, settings::max_res_size);
					return 0;
				}
			}
			else {
				settings::gamma = mmin(settings::gamma + 0.1, settings::max_gamma);
				return 0;
			}
		}
		else if (settings::has_fixed_reservoir() == true && settings::has_fixed_gamma() == false) { // exlude algorithms FLEET
			if (settings::res_size >= settings::max_res_size) {
				settings::res_size = settings::min_res_size;
				return -1;
			}
			else {
				settings::res_size = mmin(settings::res_size * 2, settings::max_res_size);
				return 0;
			}
		}
		return -1;
	}
}

namespace helper_functions {
	std::string to_str(int x) {
		std::stringstream ss;
		ss << x;
		return ss.str();
	}
	int to_int(const std::string& str) {
		std::stringstream ss; ss << str;
		int x; ss >> x;
		return x;
	}
	long long to_ll(const std::string& str) {
		std::stringstream ss; ss << str;
		long long x; ss >> x;
		return x;
	}
	double to_double(const std::string& str) {
		std::stringstream ss; ss << str;
		double x; ss >> x;
		return x;
	}
	bool is_int_num(const std::string& str) {
		if ((int)str.size() > 9)
			return false;
		for (int i = 0; i < (int)str.size(); i++) {
			if ('0' > str[i] || str[i] > '9')
				return false;
		}
		return true;
	}
	bool is_equal(double a, double b) {
		return std::fabs(a - b) <= 1e-6;
	}
	long long choose2(int x) {
		return ((long long)x * (x - 1)) >> 1;
	}
	bool yesno_query() {
		std::string command;
		std::cerr << " >>> "; std::cin >> command;
		return command[0] == 'Y' || command[0] == 'y';
	}
}
