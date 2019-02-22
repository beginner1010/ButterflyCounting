#include "Graph.h"

Graph::Graph() {}
Graph::~Graph() {}

void Graph::read_from_file() {
	std::cerr << " Insert the input (bipartite graph stream) file location" << std::endl;
	std::cerr << " >>> "; std::cin >> this->input_address;
	this->preprocessing();
	int width = 5 + 12 + 10 + 10 + 10;
	std::cerr << " |" << std::setw(12) << "#edges"	<< " |" << std::setw(10) << "#left vertices"	<< " |" << std::setw(10) << "#right vertices"		<< " |" << std::setw(10) << "max degree" << " |" << std::endl;
	std::cerr << std::string('-', width) << std::endl;
	std::cerr << " |" << std::setw(12) << this->m	<< " |" << std::setw(10) << this->n_left		<< " |" << std::setw(10)	<< this->right_vertices	<< " |" << std::setw(10) << this->maximum_degree << " |" << std::endl;
	std::cerr << std::string('-', width) << std::endl;
}

int Graph::to_string(std::string& line) { std::stringstream aux; aux << line; int res; aux >> res; return res; }

bool Graph::all_num(std::string &line) {
	for (int i = 0; i < (int)line.size(); i++) if ((line[i] >= '0' && line[i] <= '9') == false) return false;
	return true;
}

int Graph::get_vertex_left_index(int& vertex_left) {
	if (this->vertex_left_index.find(vertex_left) == this->vertex_left_index.end()) {
		// vertex_left hasn't been seen already. Say hello to her!
		this->vertex_left_index[vertex_left] = (int)this->vertices_left.size();
		this->vertices_left.push_back(this->vertex_left_index[vertex_left]);
		this->adj_left.push_back(std::unordered_set<int>());
	}
	return this->vertex_left_index[vertex_left];
}

int Graph::get_vertex_right_index(int& vertex_right) {
	if (this->vertex_right_index.find(vertex_right) == this->vertex_right_index.end()) {
		// vertex_right hasn't been seen already. Welcome home!
		this->vertex_right_index[vertex_right] = (int)this->vertices_right.size();
		this->vertices_right.push_back(this->vertex_right_index[vertex_right]);
		this->adj_right.push_back(std::unordered_set<int>());
	}
	return this->vertex_right_index[vertex_right];
}

void Graph::preprocessing() {

	this->clear();
	std::ios::sync_with_stdio(false);
	FILE *f_in;
	f_in = freopen(this->input_address, "r", stdin);
	std::string line;
	std::unordered_set < long long > seen_edges;

	while (std::getline(std::cin, line)) {
		std::stringstream ss; ss << line;
		std::vector <std::string> vec_str;
		for (std::string z; ss >> z; vec_str.push_back(z));
		if (((int)vec_str.size()) >= 2) {
			bool is_all_num = true;
			for (int i = 0; i < 2 ; i++) is_all_num &= this->all_num(vec_str[i]);
			if (is_all_num) {
				int vertex_left = to_string(vec_str[0]);
				int vertex_right = to_string(vec_str[1]);
				vertex_left = this->get_vertex_left_index(vertex_left);
				vertex_right = this->get_vertex_right_index(vertex_right);

				if (seen_edges.find(this->encode_edge(vertex_left, vertex_right)) != seen_edges.end())
					continue;

				seen_edges.insert(this->encode_edge(vertex_left, vertex_right));
				this->edges.push_back(std::make_pair(vertex_left, vertex_right));
				this->adj_left[vertex_left].insert(vertex_right);
				this->adj_right[vertex_right].insert(vertex_left);
				this->maximum_degree = mmax(this->maximum_degree, 
					mmax((int)this->adj_left[vertex_left].size(), (int)this->adj_right[vertex_right].size()));
			}
		}
	}

	fclose(f_in);
	this->m = (int)this->edges.size();
	this->n_left = (int)this->vertices_left.size();
	this->n_right = (int)this->vertices_right.size();
}

void Graph::clear() {
	this->edges.clear();
	this->vertices_left.clear();
	this->vertices_right.clear();
	this->adj_left.clear();
	this->adj_right.clear();
	this->vertex_left_index.clear();
	this->vertex_right_index.clear();
	this->m = 0;
	this->n_left = 0;
	this->n_right = 0;
	this->maximum_degree = 0;
}
