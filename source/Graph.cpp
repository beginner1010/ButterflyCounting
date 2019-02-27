#define _CRT_SECURE_NO_WARNINGS 
#include "Graph.h"

Graph::Graph() {}
Graph::~Graph() {}

void Graph::read_from_file() {
	this->preprocessing();

	int width = (5 * 2 - 1) + 12 + 15 + 15 + 12;
	std::string hline = " " + std::string(width, '-');
	std::cerr << std::endl;
	std::cerr << hline << std::endl;
	std::cerr << " |" << std::setw(12) << "#edges"	<< " |" << std::setw(15) << "#left vertices"<< " |" << std::setw(15) << "#right vertices"	<< " |" << std::setw(12) << "max degree" << " |" << std::endl;
	std::cerr << hline << std::endl;
	std::cerr << " |" << std::setw(12) << this->m	<< " |" << std::setw(15) << this->n_left	<< " |" << std::setw(15)	<< this->n_right	<< " |" << std::setw(12) << this->maximum_degree << " |" << std::endl;
	std::cerr << hline << std::endl;
}

int Graph::str_to_int(std::string& line) { std::stringstream aux; aux << line; int res; aux >> res; return res; }

bool Graph::all_num(std::string &line) {
	for (int i = 0; i < (int)line.size(); i++) if ((line[i] >= '0' && line[i] <= '9') == false) return false;
	return true;
}

int Graph::get_vertex_index(long long vertex) {
	if (this->vertex_index.find(vertex) == this->vertex_index.end()) {
		// vertex hasn't been seen already. Welcome home!
		this->vertex_index[vertex] = (int)this->vertices.size();
		if ((vertex & 1) == 0)
			this->vertices_left.push_back(this->vertex_index[vertex]);
		else
			this->vertices_right.push_back(this->vertex_index[vertex]);
		vertices.push_back(this->vertex_index[vertex]);
		this->adj_set.push_back(std::unordered_set<int>());
		this->adj_vec.push_back(std::vector<int>());
	}
	return this->vertex_index[vertex];
}

void Graph::add_new_edge(int& vertex_left, int& vertex_right) {
	this->edges.push_back(std::make_pair(vertex_left, vertex_right));
	this->adj_set[vertex_left].insert(vertex_right);
	this->adj_set[vertex_right].insert(vertex_left);
	this->adj_vec[vertex_left].push_back(vertex_right);
	this->adj_vec[vertex_right].push_back(vertex_left);
}

void Graph::fancy_text(bool done) {
	if (done == false) {
		if (line_number % text_interval == 0) {
			std::cerr << "\r";
			std::cerr << std::string(max_fancy_text_width, ' ');
			std::cerr << "\r";
			std::cerr << " Processing the input graph";
			for (int dots = 0; dots < n_dots; dots++)
				std::cerr << ".";
			n_dots = (n_dots + 1) % 4;
			std::cerr.flush();
		}
	}
	else {
		std::cerr << "\r";
		std::cerr << std::string(max_fancy_text_width, ' ');
		std::cerr << "\r";
		std::cerr << " The input graph is processed. See the statistics below:";
		std::cerr.flush();
	}
}

void Graph::preprocessing() {
	this->clear();
	std::ios::sync_with_stdio(false);
	std::string line;
	std::unordered_set < long long > seen_edges;

	while (std::getline(std::cin, line)) {
		fancy_text(false);
		line_number++;

		std::stringstream ss; ss << line;
		std::vector <std::string> vec_str;
		for (std::string z; ss >> z; vec_str.push_back(z));
		if (((int)vec_str.size()) >= 2) {
			bool is_all_num = true;
			for (int i = 0; i < 2 ; i++) is_all_num &= this->all_num(vec_str[i]);
			if (is_all_num) {
				int vertex_left = str_to_int(vec_str[0]);
				int vertex_right = str_to_int(vec_str[1]);
				vertex_left = this->get_vertex_index(vertex_left * 10LL);
				vertex_right = this->get_vertex_index(vertex_right * 10LL + 1);

				if (seen_edges.find(this->encode_edge(vertex_left, vertex_right)) != seen_edges.end())
					continue;

				seen_edges.insert(this->encode_edge(vertex_left, vertex_right));
				this->add_new_edge(vertex_left, vertex_right);
				this->maximum_degree = mmax(this->maximum_degree, 
					mmax((int)this->adj_vec[vertex_left].size(), (int)this->adj_vec[vertex_right].size()));
			}
		}
	}
	fancy_text(true);
	this->m = (int)this->edges.size();
	this->n = (int)this->vertices.size();
	this->n_left = (int)this->vertices_left.size();
	this->n_right = (int)this->vertices_right.size();
}

void Graph::compute_n_z() {
	this->n_z = 0;
	this->cum_z.clear();
	for (auto& e : this->edges) {
		this->n_z += ((long long)this->adj_vec[e.first].size() - 1) 
				* ((long long)this->adj_vec[e.second].size() - 1);
		this->cum_z.push_back(n_z);
	}
}

void Graph::compute_n_centred_z() {
	this->n_centered_z = 0;
	this->cum_centered_z.clear();
	for (auto& e : this->edges) {
		int& a = e.first;
		int& b = e.second;
		std::vector <int>& adj_a = this->get_adj_vec(a);
		std::vector <int>& adj_b = this->get_adj_vec(b);
		int index_b_in_adj_a = (int)(std::lower_bound(adj_a.begin(), adj_a.end(), b) - adj_a.begin());
		int index_a_in_adj_b = (int)(std::lower_bound(adj_b.begin(), adj_b.end(), a) - adj_b.begin());
		int degree_a = this->degree(a) - index_b_in_adj_a - 1;
		int degree_b = this->degree(b) - index_a_in_adj_b - 1;
		//for (auto& neighbor_a : adj_a)
		//	if (this->degree(neighbor_a) > this->degree(b) || (this->degree(neighbor_a) == this->degree(b) && neighbor_a > b))
		//		degree_a++;
		//for (auto& neighbor_b : adj_b)
		//	if (this->degree(neighbor_b) > this->degree(a) || (this->degree(neighbor_b) == this->degree(a) && neighbor_b > a))
		//		degree_b++;
		this->n_centered_z += ((long long)degree_a * (long long)degree_b);
		this->cum_centered_z.push_back(n_centered_z);
	}
}

void Graph::reindex() {
	std::vector <int> aux_rank = std::vector <int>(this->n);
	for (int i = 0; i < this->n; i++)
		aux_rank[this->degree_vertex_vec[i].second] = i;
	std::vector < std::pair<int, int> > aux_edges = this->edges;

	this->resize();
	// we do not update left_vertices, right_vertices, maximum_degree (we don't use them. Should we update them?)
	for (auto& aux_edge : aux_edges) {
		int& a = aux_rank[aux_edge.first];
		int& b = aux_rank[aux_edge.second];
		this->add_new_edge(a, b);
	}
	for (auto& v : this->vertices) {
		std::vector<int>& adj = this->get_adj_vec(v);
		std::sort(adj.begin(), adj.end());
	}
	this->n = (int)this->vertices.size();
	this->m = (int)this->edges.size();
}

void Graph::resize() {
	this->adj_vec.clear();
	this->adj_vec.resize(this->n);
	this->adj_set.clear();
	this->adj_set.resize(this->n);
	this->edges.clear();
}

void Graph::sort_vertices_by_degree() {
	this->degree_vertex_vec.clear();
	for (auto& vertex : this->vertices) {
		this->degree_vertex_vec.push_back(std::make_pair(this->degree(vertex), vertex));
	}
	std::sort(this->degree_vertex_vec.begin(), this->degree_vertex_vec.end());
	this->reindex();
}

int Graph::get_edge_index_by_nz(long long& ran_nz) {
	int lo = 0, hi = this->m; 
	while (lo < hi) {
		int mid = (lo + hi) >> 1;
		if (cum_z[mid] < ran_nz)
			lo = mid + 1;
		else
			hi = mid;
	}
	return lo;
}

int Graph::get_edge_index_by_centered_nz(long long& ran_centered_nz) {
	int lo = 0, hi = this->m;
	while (lo < hi) {
		int mid = (lo + hi) >> 1;
		if (cum_centered_z[mid] < ran_centered_nz)
			lo = mid + 1;
		else
			hi = mid;
	}
	return lo;
}

void Graph::clear() {
	this->edges.clear();
	this->vertices.clear();
	this->vertices_left.clear();
	this->vertices_right.clear();
	this->adj_set.clear();
	this->adj_vec.clear();
	this->vertex_index.clear();
	this->m = 0;
	this->n = 0;
	this->n_z = -1;
	this->n_centered_z = -1;
	this->n_left = 0;
	this->n_right = 0;
	this->maximum_degree = 0;
	this->n_dots = 0;
	this->line_number = 0;
}
