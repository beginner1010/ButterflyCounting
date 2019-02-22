#pragma once
#include <iostream>
#include <vector>
#include <unordered_set>
#include <cassert>
#include <string>
#include <sstream>
#include <unordered_map>
#include <iomanip>

#define mmin(x,y) ((x)>(y)?(y):(x))
#define mmax(x,y) ((x)<(y)?(y):(x))
const long long SHIFT = 1000 * 1000 * 1000; 

class Graph {
public:
	Graph();
	~Graph();
	void clear();
	void read_from_file();
	inline std::unordered_set<int>& get_adj_left(int& vertex_left) {
		assert(vertex_left < this->n_left);
		return this->adj_left[vertex_left];
	}
	inline std::unordered_set<int>& get_adj_right(int& vertex_right) {
		assert(vertex_right < this->n_right);
		return this->adj_right[vertex_right];
	}
	inline int get_n_edges() {
		return this->m;
	}
	inline int get_n_left_vertices() {
		return this->n_left;
	}
	inline int get_n_right_vertices() {
		return this->n_right;
	}
	inline std::vector< std::pair<int, int> >& get_edges() {
		return this->edges;
	}
	inline std::vector<int>& left_vertices() {
		return this->vertices_left;
	}
	inline std::vector<int>& right_vertices() {
		return this->vertices_right;
	}
private:
	std::vector < std::pair<int, int> > edges;
	std::vector < int > vertices_left;
	std::vector < int > vertices_right;
	std::vector < std::unordered_set<int> > adj_left;
	std::vector < std::unordered_set<int> > adj_right;
	std::unordered_map <int, int> vertex_left_index;
	std::unordered_map <int, int> vertex_right_index;
	char *input_address;
	int m;
	int n_left;
	int n_right;
	int maximum_degree;

	void preprocessing();
	int to_string(std::string& line);
	bool all_num(std::string &line);
	int get_vertex_left_index(int& left_vertex);
	int get_vertex_right_index(int& right_vertex);
	inline long long encode_edge(int& vertex_left, int& vertex_right) {
		return vertex_left * SHIFT + vertex_right;
	}
	inline std::pair<int, int> decode_edge(long long& hash_value) {
		return std::make_pair(hash_value / SHIFT, hash_value % SHIFT);
	}
};