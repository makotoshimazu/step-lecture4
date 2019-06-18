//! clang++ -std=c++14 -Wall -Wextra shortest.cc
#include <sys/time.h>

#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>

const char* LINKS_TXT_PATH = "links.txt";
const char* NICKNAMES_TXT_PATH = "nicknames.txt";


class Vertex {
 public:
  Vertex(std::vector<int> edges) : edges_(std::move(edges)) {}
  Vertex(Vertex&& other) : edges_(std::move(other.edges_)) {}
  void operator=(Vertex&& other) {
    edges_ = std::move(other.edges_);
  }

  void PrintEdges() const {
    std::cout << "{";
    bool is_first = true;
    for (auto& e : edges_) {
      std::cout << (is_first ? "" : ", ") << e;
      is_first = false;
    }
    std::cout << "}" << std::endl;
  }

  const std::vector<int>& edges() const { return edges_; }

 private:
  std::vector<int> edges_;
};

class Graph {
 public:
  Graph() {}
  void AddVertex(Vertex vertex) {
    vertexes_.emplace_back(std::move(vertex));
  }

  const std::vector<Vertex>& vertexes() const {
    return vertexes_;
  }

  void PrintShortestPath(int from, int to) {
    std::cout << "From: " << names_[from]
              << ", To: " << names_[to] << std::endl;
    auto path = bfs(from, to);
    if (path.size() == 0) {
      std::cout << "Path was not found" << std::endl;
      return;
    }
    std::cout << path.size() - 1 << " steps" << std::endl;
    std::cout << "Path: {";
    bool is_first = true;
    for (auto& v : path) {
      std::cout << (is_first ? "" : ", ") << names_[v];
      is_first = false;
    }
    std::cout << "}" << std::endl;
  }

  static std::unique_ptr<Graph> Create(const char* nicknames_path,
                                       const char* links_path) {
    std::unique_ptr<Graph> graph(new Graph());

    std::fstream links_stream(links_path);
    if (links_stream.fail()) {
      std::cerr << "file not found: " << links_path << std::endl;
      return nullptr;
    }

    std::vector<int> edges;
    while (true) {
      static int vertex_index = 0;
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
        vertex_index++;
        graph->AddVertex(std::move(edges));
        if (edges.size() != 0)
          break;
      }
      edges.push_back(out);
    }
    // Add the last vertex
    if (edges.size() != 0)
      graph->AddVertex(std::move(edges));

    // Read names
    std::vector<std::string> names;
    std::fstream nicknames_stream(nicknames_path);
    if (nicknames_stream.fail()) {
      std::cerr << "file not found: " << nicknames_path <<  std::endl;
      return nullptr;
    }

    while (true) {
      size_t id;
      std::string name;
      nicknames_stream >> id >> name;
      if (nicknames_stream.eof())
        break;
      if (names.size() != id) {
        std::cerr << "unmatch id" << std::endl;
        return nullptr;
      }
      names.push_back(name);
    }
    graph->names_ = std::move(names);

    return graph;
  }

 private:
  std::vector<int> bfs(int from, int to) {
    std::vector<bool> visited(vertexes().size());
    std::queue<std::vector<int>> queue;
    queue.emplace(std::vector<int>({from}));
    visited[from] = true;

    while (!queue.empty()) {
      const auto& route = queue.front();
      int index = route.back();
      if (index == to)
        return std::move(route);
      // Push the outgoing nodes into the |queue|
      const Vertex& v = vertexes_[index];
      for (const auto& i : v.edges()) {
        if (!visited[i]) {
          visited[i] = true;
          auto new_route = route;
          new_route.push_back(i);
          queue.emplace(std::move(new_route));
        }
      }
      queue.pop();
    }
    return std::vector<int>();
  }

  std::vector<Vertex> vertexes_;
  std::vector<std::string> names_;
};


class Timer {
 public:
  Timer(const std::string& tag) : tag_(tag) {
    timeval tv;
    gettimeofday(&tv, nullptr);
    begin_ = tv.tv_sec + tv.tv_usec / 1E6;
    std::cout << "==== Begin: " << tag_ << " ====" << std::endl;
  }

  ~Timer() {
    timeval tv;
    gettimeofday(&tv, nullptr);
    double end = tv.tv_sec + tv.tv_usec / 1E6;

    std::cout << "Elapsed: " << std::setprecision(3)
              << end - begin_ << " sec" << std::endl;
    std::cout << "==== End: " << tag_ << " ====" << std::endl;
  }

 private:
  double begin_;
  std::string tag_;
};

int main() {
  std::unique_ptr<Graph> graph;
  {
    Timer t("Create graph");
    graph = Graph::Create(NICKNAMES_TXT_PATH, LINKS_TXT_PATH);
    if (!graph)
      return -1;

    int n_edges = 0;
    for (const auto& v : graph->vertexes())
      n_edges += v.edges().size();
    std::cout << "num vertexes: " << graph->vertexes().size() << " "
              << "num edges: " << n_edges << std::endl;
  }

  std::cout << "jacob's id: 23" << std::endl;

  while (true) {
    int from, to;
    std::cout << "Type the source's id: ";
    std::cin >> from;
    std::cout << "Type the destination's id: ";
    std::cin >> to;
    if (from < 0 || static_cast<int>(graph->vertexes().size()) <= from) {
      std::cout << "out of range (source)" << std::endl;
      continue;
    }
    if (to < 0 || static_cast<int>(graph->vertexes().size()) <= to) {
      std::cout << "out of range (to)" << std::endl;
      continue;
    }

    Timer t("BFS");
    graph->PrintShortestPath(from, to);
  }

  return 0;
}
