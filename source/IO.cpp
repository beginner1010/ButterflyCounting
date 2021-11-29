#include "IO.h"

namespace IO {
	std::string input_address;
	std::string output_address;
	std::string input_file_name;
	FILE* fout;
	FILE* fin;

	void IO_addresses() {
		int mode = 3;
		do {
			std::cerr << " Insert the location of the input network:" << std::endl;
			std::cerr << " >>> "; std::cin >> IO::input_file_name;

			IO::input_address = "/work/snt-free/bfly/in." + IO::input_file_name;
			if (IO::check_if_file_exists(false)) {
				mode = 1; // to make it easy to get input file address easier on Nova Cluster
				break;
			}
			IO::input_address = "input/in." + IO::input_file_name;
			if (IO::check_if_file_exists(false)) { // to make it easy to get input file address easier on PC Office
				mode = 2;
				break;
			}
			IO::input_address = IO::input_file_name;
		} while (IO::check_if_file_exists(true) == false);

		if (mode == 3) { // the complete address is given
			IO::guess_input_file_name();
		}
	}

	double get_file_size() {
		std::ifstream in(input_address.c_str(), std::ifstream::ate | std::ifstream::binary);
		return (double)in.tellg();
	}

	void create_folder() {
		std::string folder_name = "";
		if (settings::is_exact_algorithm()) folder_name = "exact";
		else if (settings::is_one_shot_algorithm()) folder_name = "one_shot";
		else if (settings::is_local_sampling_algorithm()) folder_name = "local_sampling";
		else if (settings::is_streaming()) folder_name = "streaming";
#ifdef _WIN32
		::_mkdir("./output");
		::_mkdir(("./output/" + IO::input_file_name).c_str());
		::_mkdir(("./output/" + IO::input_file_name + "/" + folder_name).c_str());
#else
		::mkdir("./output", 0777);
		::mkdir(("./output/" + IO::input_file_name).c_str(), 0777);
		::mkdir(("./output/" + IO::input_file_name + "/" + folder_name).c_str(), 0777);
#endif
		char buffer[1024];
		std::string file_name = "";
		file_name += settings::exp_repeatition == -1 ? "" : "rep=" + helper_functions::to_str(settings::exp_repeatition);
		if (settings::chosen_algo == ONE_SHOT_DOULION) {
			sprintf(buffer, "_p=[%g,%g]", settings::min_p, settings::max_p);
			file_name += std::string(buffer);
		}
		else if (settings::chosen_algo == ONE_SHOT_COLORFUL) {
			sprintf(buffer, "_nclr=[%d,%d]", settings::min_clr, settings::max_clr);
			file_name += std::string(buffer);
		}
		else if (settings::is_local_sampling_algorithm() == true) {
			sprintf(buffer, "_mxtime=%d", (int)(settings::max_time + 1e-6));
			file_name += std::string(buffer);
		}
		else if (settings::is_streaming() == true) {
			if(settings::chosen_algo == EXACT_FULLY_DYNAMC || settings::chosen_algo == EXACT_INSERTION_ONLY) 
				sprintf(buffer, "_snapshot=%d", settings::snapshots);
			else if(settings::chosen_algo == FLEET3 || settings::chosen_algo == FLEET3_FULLY_DYNAMIC)
				sprintf(buffer, "_Rsize=[%d,%d]_gamma=[%g,%g]_snapshot=%d", (int)(settings::min_res_size + 1e-6), (int)(settings::max_res_size + 1e-6), settings::min_gamma, settings::max_gamma, settings::snapshots);
			else if(settings::chosen_algo == THINKD_FAST)
				sprintf(buffer, "_p=[%g,%g]_snapshot=%d", settings::min_p, settings::max_p, settings::snapshots);
			else if(settings::chosen_algo == THINKD_ACC || settings::chosen_algo == RS_WITH_DELETION || settings::chosen_algo == IMPR_RS_WITH_DELETION)
				sprintf(buffer, "_Rsize=[%d,%d]_snapshot=%d", settings::min_res_size, settings::max_res_size, settings::snapshots);
			file_name += std::string(buffer);
		}
		file_name = constants::suffix_output_address[settings::chosen_algo] + (file_name != "" ? "_" : "") + file_name;
		IO::output_address = "output/" + IO::input_file_name + "/" + folder_name + "/" + file_name + ".txt";
		return;
	}

	bool check_if_file_exists(bool log) {
		if (access(IO::input_address.c_str(), 0) != 0) {
			if (log == true) 
				std::cerr << " No such file exists!" << std::endl;
			return false;
		}
		return true;
	}

	void guess_input_file_name() {
		std::string file_name = "";
		for (int i = (int)IO::input_address.size() - 1; i >= 0; i--) {
			if (std::string("./\\").find(IO::input_address[i]) != std::string::npos) {
				break;
			}
			file_name += IO::input_address[i];
		}
		std::reverse(file_name.begin(), file_name.end());
		IO::input_file_name = file_name;
	}
 
}

