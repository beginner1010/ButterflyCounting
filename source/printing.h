#pragma once

#include <iostream>
#include <string>
#include <iomanip>

#include "utills.h"
#include "Graph.h"

namespace print {
	void statistics_table(const int& m, const int& n_l, const int& n_r, const int& max_deg);
	void statistics_table(const long long& m);
	void done_experiments(const int& iter_exp);
	void done_work_percentage(double done_work, double total_work, std::string text);
	void print_dots(int& n_dots);
	void message_with_dots(double& last_print_time, int& n_dots, double& cur_time, double total_work, double done_work, const std::string text);
	void finished_work(const double& cur_time, const std::string text);
	void clear_line();
	void print_header();
	void print_result(std::vector <double> res);
}