#include <sys/time.h>

#include <algorithm>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <queue>
#include <vector>

const char* LINKS_TXT_PATH = "links.txt";
const char* PAGES_TXT_PATH = "pages.txt";
const double DEFAULT_PAGE_RANK = 100;

class Vertex {
 public:
  Vertex(std::vector<int> edges) :
      edges_(std::move(edges)), weight_(DEFAULT_PAGE_RANK), next_weight_(0) {}

  void PrintEdges() const {
    std::cout << "{";
    bool is_first = true;
    for (auto& e : edges_) {
      std::cout << (is_first ? "" : ", ") << e;
      is_first = false;
    }
    std::cout << "}" << std::endl;
  }

  void NextStep() {
    weight_ = next_weight_;
    next_weight_ = 0;
  }

  void AddNextWeight(double weight) {
    next_weight_ += weight;
  }

  const std::vector<int>& edges() const { return edges_; }
  double weight() const { return weight_; }

 private:
  std::vector<int> edges_;
  double weight_;
  double next_weight_;
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

  std::vector<std::pair<double, std::string>> Search(std::string query) const {
    std::vector<std::pair<double, std::string>> answers;
    for (size_t i = 0; i < names_.size(); ++i) {
      const std::string& name = names_[i];
      if (name.find(query) == std::string::npos)
        continue;
      answers.emplace_back(vertexes_[i].weight(), name);
    }
    return answers;
  }

  void UpdatePageRank() {
    for (const auto& v : vertexes_) {
      double out_weight = v.weight() / v.edges().size();
      for (int idx : v.edges())
        vertexes_[idx].AddNextWeight(out_weight);
    }

    for (auto& v : vertexes_)
      v.NextStep();
  }



  static std::unique_ptr<Graph> Create(const char* pages_path,
                                       const char* links_path) {
    std::unique_ptr<Graph> graph = std::make_unique<Graph>();

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
    std::cout << "==== " << tag_ << " ====" << std::endl;
  }

  ~Timer() {
    timeval tv;
    gettimeofday(&tv, nullptr);
    double end = tv.tv_sec + tv.tv_usec / 1E6;

    std::cout << "Elapsed: " << std::setprecision(3)
              << end - begin_ << " sec" << std::endl;
    std::cout << std::endl;
    // std::cout << "==== End: " << tag_ << " ====" << std::endl;
  }

 private:
  double begin_;
  std::string tag_;
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


  // 457783: Google
  // 17821: ディズニーランド
  {
    Timer t("Google -> Disney");
    graph->PrintShortestPath(457783, 17821);
  }
  {
    Timer t("Disney -> Google");
    graph->PrintShortestPath(17821, 457783);
  }

  for (int i = 0; i < 20; i++) {
    Timer t("Update page rank");
    graph->UpdatePageRank();
  }

  while (true) {
    std::cout << "Input query (Press Ctrl+D to quit): ";
    std::string query;
    std::cin >> query;
    if (std::cin.eof()) {
      std::cout << std::endl;
      break;
    }
    if (query.empty())
      continue;

    std::cout << "searching..." << std::endl;
    std::vector<std::pair<double, std::string>> answers;
    {
      Timer t("query");
      answers = graph->Search(query);
    }
    std::cout << "We have " << answers.size() << " answers" << std::endl;
    {
      Timer t("sort");
      std::sort(answers.begin(), answers.end());
    }

    for (size_t i = 0; i < 5; ++i) {
      int idx = answers.size() - 1 - i;
      if (idx < 0)
        break;
      const auto& ans = answers[idx];
      std::cout << ans.second << " score: " << ans.first << std::endl;
    }
    std::cout << std::endl;
  }

  return 0;
}
