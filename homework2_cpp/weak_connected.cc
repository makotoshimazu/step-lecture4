#include <sys/time.h>

#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <queue>
#include <set>
#include <vector>

const char* LINKS_TXT_PATH = "links.txt";
const char* PAGES_TXT_PATH = "pages.txt";


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

class Vertex {
 public:
  Vertex() {}
  Vertex(std::set<int> edges) : edges_(std::move(edges)), out_edges_(edges_) {}

  void PrintEdges() const {
    std::cout << "{";
    bool is_first = true;
    for (auto& e : edges_) {
      std::cout << (is_first ? "" : ", ") << e;
      is_first = false;
    }
    std::cout << "}" << std::endl;
  }

  void ConnectTo(int dst) { edges_.insert(dst); }

  const std::set<int>& edges() const { return edges_; }
  const std::set<int>& out_edges() const { return out_edges_; }

 private:
  std::set<int> edges_;
  std::set<int> out_edges_;
};

class Graph {
 public:
  Graph() {}
  void AddVertex(Vertex vertex) {
    vertexes_.emplace_back(std::move(vertex));
  }

  Vertex& vertex(int i) {
    return vertexes_[i];
  }

  const std::vector<Vertex>& vertexes() const {
    return vertexes_;
  }

  static std::unique_ptr<Graph> Create(const char* pages_path,
                                       const char* links_path) {
    std::unique_ptr<Graph> graph(new Graph());

    std::fstream links_stream(links_path);
    if (links_stream.fail()) {
      std::cerr << "file not found: " << links_path << std::endl;
      return nullptr;
    }

    {
      Timer t("Read links.txt");
      std::set<int> edges;
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
        edges.insert(out);
      }
      // Add the last vertex
      if (edges.size() != 0)
        graph->AddVertex(std::move(edges));
    }

    {
      Timer t("Create reversed edges");
      // Add reversed direction
      for (size_t i = 0; i < graph->vertexes().size(); i++) {
        const auto& v = graph->vertexes()[i];
        for (int e : v.edges())
          graph->vertex(e).ConnectTo(i);
      }
    }

    {
      Timer t("Read pages.txt");
      // Read names
      std::vector<std::string> names;
      std::fstream pages_stream(pages_path);
      if (pages_stream.fail()) {
        std::cerr << "file not found: " << pages_path <<  std::endl;
        return nullptr;
      }

      while (true) {
        size_t id;
        std::string name;
        pages_stream >> id >> name;
        if (pages_stream.eof())
          break;
        if (names.size() != id) {
          std::cerr << "unmatch id" << std::endl;
          return nullptr;
        }
        names.push_back(name);
      }
      graph->names_ = std::move(names);
    }
    return graph;
  }

  void WriteReachable(int start) {
    std::vector<bool> visited(vertexes().size(), false);
    visited[start] = true;
    dfs_mark_visited(&visited, start);

    std::ofstream pages("out_pages.txt");
    std::ofstream links("out_links.txt");
    if (pages.fail() || links.fail()) {
      std::cerr << "something happened" << std::endl;
    }
    for (size_t i = 0; i < visited.size(); i++) {
      if (visited[i]) {
        pages << i << "\t" << names_[i] << std::endl;
        for (int e : vertexes_[i].out_edges())
          links << i << "\t" << e << std::endl;
      }
    }
  }

 private:
  void dfs_mark_visited(std::vector<bool>* visited, int index) {
    for (int e : vertexes_[index].edges()) {
      if (!(*visited)[e]) {
        (*visited)[e] = true;
        dfs_mark_visited(visited, e);
      }
    }
  }

  std::vector<Vertex> vertexes_;
  std::vector<std::string> names_;
};

int main() {
  std::unique_ptr<Graph> graph;
  {
    Timer t("Create graph");
    graph = Graph::Create(PAGES_TXT_PATH, LINKS_TXT_PATH);
    if (!graph)
      return -1;

    int n_edges = 0;
    for (const auto& v : graph->vertexes())
      n_edges += v.edges().size();
    std::cout << "num vertexes: " << graph->vertexes().size() << " "
              << "num edges: " << n_edges << std::endl;
  }

  {
    Timer t("Write graph");
    graph->WriteReachable(0);
  }

  return 0;
}
