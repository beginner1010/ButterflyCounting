#pragma once
#include <random>
#include "Graph.h"

long long kai_exact_count(Graph& g);
long double randomized_ebfc(Graph& g, int& vertex_left, int& vertex_right, int& ebfc_iterations);
long double randomized_centered_ebfc(Graph& g, int& vertex_a, int& vertex_b, int& ebfc_iterations);
bool z_forms_bfly(Graph& g, int& vertex_a, int& vertex_b);
bool centered_z_forms_bfly(Graph& g, int& vertex_a, int& vertex_b);