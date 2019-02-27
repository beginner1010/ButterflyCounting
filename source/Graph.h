#pragma once
#include <iostream>
#include <vector>
#include <unordered_set>
#include <cassert>
#include <string>
#include <sstream>
#include <unordered_map>
#include <iomanip>
#include <algorithm>
#include <sys/stat.h>

#define mmin(x,y) ((x)>(y)?(y):(x))
#define mmax(x,y) ((x)<(y)?(y):(x))
const long long SHIFT = 1000 * 1000 * 1000; 

class Graph {
public:
	Graph();
	~Graph();
	Graph& operator=(Graph& other) {
		this->edges = other.edges;
		this->vertices = other.vertices;
		this->vertices_left = other.vertices_left;
		this->vertices_right = other.vertices_right;
		this->adj_set = other.adj_set;
		this->adj_vec = other.adj_vec;
		this->cum_z = other.cum_z;
		this->cum_centered_z = other.cum_centered_z;
		this->m = other.m; 
		this->n = other.n;
		this->n_z = other.n_z; 
		this->n_centered_z = other.n_centered_z;
		this->n_left = other.n_left;
		this->n_right = other.n_right;
		this->maximum_degree = other.maximum_degree;
		return *this;
	}

	void clear();
	void read_from_file();
	inline std::unordered_set<int>& get_adj_set(int& vertex) {
		assert(vertex < this->n);
		return this->adj_set[vertex];
	}
	inline std::vector<int>& get_adj_vec(int& vertex) {
		assert(vertex < this->n);
		return this->adj_vec[vertex];
	}
	inline int get_n_edges() {
		return this->m;
	}
	inline int get_n_vertices() {
		return this->n;
	}
	inline long long get_nz() {
		if (this->n_z == -1)
			this->compute_n_z();
		return this->n_z;
	}
	inline long long get_n_centered_z() {
		if (this->n_centered_z == -1)
			this->compute_n_centred_z();
		return this->n_centered_z;
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
	inline std::vector< int >& get_vertices() {
		return this->vertices;
	}
	inline std::vector<int>& get_left_vertices() {
		return this->vertices_left;
	}
	inline std::vector<int>& get_right_vertices() {
		return this->vertices_right;
	}
	inline std::pair<int,int>& get_ith_edge(int& idx) {
		return this->edges[idx];
	}
	inline int degree(int& vertex) {
		return (int)this->adj_vec[vertex].size();
	}
	inline int degree_const(const int& vertex) {
		return (int)this->adj_vec[vertex].size();
	}
	int get_vertex_index(long long vertex);
	void add_new_edge(int& vertex_left, int& vertex_right);
	int get_edge_index_by_nz(long long& n_z);
	int get_edge_index_by_centered_nz(long long& n_z);
	void sort_vertices_by_degree();
	bool cmp_by_degree(const int& a, const int& b) {
		if (adj_vec[a].size() != adj_vec[b].size()) {
			return adj_vec[a].size() > adj_vec[b].size();
		}
		return a > b;
	}
private:
	std::vector < std::pair<int, int> > edges;
	std::vector < int > vertices;
	std::vector < std::pair< long, int > > degree_vertex_vec;
	std::vector < long long > cum_z;
	std::vector < long long > cum_centered_z;
	std::vector < int > vertices_left;
	std::vector < int > vertices_right;
	std::vector < std::unordered_set<int> > adj_set;
	std::vector < std::vector<int> > adj_vec;
	std::unordered_map <long long, int> vertex_index;
	int m;
	int n;
	int n_left;
	int n_right;
	int maximum_degree;
	const int text_interval = 100000;
	const int max_fancy_text_width = 50;
	int n_dots;
	int line_number;
	long long n_z; 
	long long n_centered_z;

	void compute_n_z();
	void compute_n_centred_z();

	void reindex();
	void resize();

	void fancy_text(bool done);
	void preprocessing();
	int str_to_int(std::string& line);
	bool all_num(std::string &line);
	inline long long encode_edge(int& vertex_left, int& vertex_right) {
		return vertex_left * SHIFT + vertex_right;
	}
	inline std::pair<int, int> decode_edge(long long& hash_value) {
		return std::make_pair(hash_value / SHIFT, hash_value % SHIFT);
	}
};