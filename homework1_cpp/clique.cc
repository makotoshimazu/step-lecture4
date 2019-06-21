//! clang++ -std=c++14 -Wall -Wextra shortest.cc
#include <sys/time.h>

#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <queue>
#include <set>
#include <vector>

const char *LINKS_TXT_PATH = "links.txt";
const char *NICKNAMES_TXT_PATH = "nicknames.txt";

class Vertex {
 public:
  Vertex(std::vector<int> edges) : edges_(edges.begin(), edges.end()) {}
  Vertex(Vertex &&other) : edges_(std::move(other.edges_)) {}
  void operator=(Vertex &&other) { edges_ = std::move(other.edges_); }

  void PrintEdges() const {
    std::cout << "{";
    bool is_first = true;
    for (auto &e : edges_) {
      std::cout << (is_first ? "" : ", ") << e;
      is_first = false;
    }
    std::cout << "}" << std::endl;
  }

  const std::set<int> &edges() const { return edges_; }

 private:
  std::set<int> edges_;
};

class Graph {
 public:
  Graph() {}
  void AddVertex(Vertex vertex) { vertexes_.emplace_back(std::move(vertex)); }

  const std::vector<Vertex> &vertexes() const { return vertexes_; }

  static std::unique_ptr<Graph> Create(const char *nicknames_path,
                                       const char *links_path) {
    auto graph = std::make_unique<Graph>();

    std::fstream links_stream(links_path);
    if (links_stream.fail()) {
      std::cerr << "file not found: " << links_path << std::endl;
      return nullptr;
    }

    std::vector<int> edges;
    int vertex_index = 0;
    while (true) {
      int in = -1, out = -1;
      links_stream >> in >> out;
      if (links_stream.eof())
       break;
      if (in == -1 || out == -1) {
        std::cerr << "unexpected error: " << vertex_index << std::endl;
        return nullptr;
      }
      // Add the previous vertex and prepare for the next one.
      if (in != vertex_index) {
        ++vertex_index;
        graph->AddVertex(std::move(edges));
        for (int i = vertex_index; i < in; i++) {
          // Add empty nodes if some node doesn't have any edges.
          graph->AddVertex(std::vector<int>());
        }
        vertex_index = in;
      }
      edges.push_back(out);
    }
    // Add the last vertex
    if (edges.size() != 0) {
      graph->AddVertex(std::move(edges));
    }

    // Read names
    std::vector<std::string> names;
    std::fstream nicknames_stream(nicknames_path);
    if (nicknames_stream.fail()) {
      std::cerr << "file not found: " << nicknames_path << std::endl;
      return nullptr;
    }

    while (true) {
      size_t id;
      std::string name;
      nicknames_stream >> id >> name;
      if (nicknames_stream.eof()) break;
      if (names.size() != id) {
        std::cerr << "unmatch id" << std::endl;
        return nullptr;
      }
      names.push_back(name);
    }
    graph->names_ = std::move(names);

    return graph;
  }

  void PrintCliques() {
    for (size_t i = 0; i < vertexes_.size(); i++) {
      auto clique = findMaxClique(i);
      std::cout << "{ ";
      for (int e : clique) {
        std::cout << names_[e] << ", ";
      }
      std::cout << "}" << std::endl;
    }
  }

 private:
  bool isExistEdge(int from, int to) {
    const auto& edges = vertexes_[from].edges();
    return edges.find(to) != edges.end();
  }

  bool isBiDirectionalEdge(int a, int b) {
    bool a_b = isExistEdge(a, b);
    bool b_a = isExistEdge(b, a);
    return a_b && b_a;
  }

  std::set<int> findMaxClique(int src) {
    std::vector<std::vector<int>> cliques = {};
    const char* src_name = names_[src].c_str();
    for (int will_be_added : vertexes_[src].edges()) {
      const char* will_be_added_name = names_[will_be_added].c_str();
      if (!isBiDirectionalEdge(src, will_be_added)) {
        continue;
      }
      auto next_cliques = cliques;
      // Check if |will_be_add| can be merged into any of clique.
      for (auto& clique : cliques) {
        // If all existing nodes are bidirectional with |will_be_added|, it
        // means |will_be_added| is a part of the clique.
        bool in_this_clique = true;
        for (int existing : clique) {
          const char *existing_name = names_[existing].c_str();
          if (!isBiDirectionalEdge(will_be_added, existing)) {
            in_this_clique = false;
            break;
          }
        }
        if (in_this_clique) {
          std::vector<int> new_clique = clique;
          new_clique.push_back(will_be_added);
          next_cliques.emplace_back(std::move(new_clique));
        }
      }
      // Create a subset of a clique.
      next_cliques.push_back({src, will_be_added});
      cliques = std::move(next_cliques);
    }
    int max_id = -1;
    int max_size = -1;
    for (size_t i = 0; i < cliques.size(); i++) {
      if (static_cast<int>(cliques[i].size()) > max_size) {
        max_id = i;
        max_size = cliques[i].size();
      }
    }

    return (max_id < 0)
               ? std::set<int>({src})
               : std::set<int>(cliques[max_id].begin(), cliques[max_id].end());
  }

  std::vector<Vertex> vertexes_;
  std::vector<std::string> names_;
};

class Timer {
 public:
  Timer(const std::string &tag) : tag_(tag) {
    timeval tv;
    gettimeofday(&tv, nullptr);
    begin_ = tv.tv_sec + tv.tv_usec / 1E6;
    std::cout << "==== Begin: " << tag_ << " ====" << std::endl;
  }

  ~Timer() {
    timeval tv;
    gettimeofday(&tv, nullptr);
    double end = tv.tv_sec + tv.tv_usec / 1E6;

    std::cout << "Elapsed: " << std::setprecision(3) << end - begin_ << " sec"
              << std::endl;
    std::cout << "==== End: " << tag_ << " ====" << std::endl;
  }

 private:
  double begin_;
  std::string tag_;
};

int main() {
  auto graph = Graph::Create(NICKNAMES_TXT_PATH, LINKS_TXT_PATH);
  graph->PrintCliques();
  return 0;
}