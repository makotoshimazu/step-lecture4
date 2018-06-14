#include <cstdio>
#include <map>
#include <string>
#include <vector>

class Node {
 public:
  Node() : Node(100.0) {}
  Node(double weight) : weight_(weight), next_weight_(0) {}

  void AddEdge(Node* other) { edges_.push_back(other); }

  void AddNextWeight(double weight) {
    next_weight_ += weight;
  }

  void NextStep() {
    weight_ = next_weight_;
    next_weight_ = 0;
  }

  double weight() const { return weight_; }
  const std::vector<Node*>& edges() const { return edges_; }

 private:
  double weight_;
  double next_weight_;
  std::vector<Node*> edges_;
};

using Nodes = std::map<std::string, Node>;

void PageRank(Nodes* nodes) {
  for (const auto& pair : *nodes) {
    double out_weight = pair.second.weight() / pair.second.edges().size();
    for (auto* other : pair.second.edges())
      other->AddNextWeight(out_weight);
  }

  for (auto& pair : *nodes)
    pair.second.NextStep();
}

void PrintNodes(int step, const Nodes& nodes) {
  printf("Step %2d: ", step);
  for (const auto& pair : nodes)
    printf("(%s, %.1f) ", pair.first.c_str(), pair.second.weight());
  printf("\n");
}

int main() {
  Nodes nodes;

  nodes.emplace("a", 100.0);
  nodes.emplace("b", 100.0);
  nodes.emplace("c", 100.0);
  nodes.emplace("d", 100.0);
  nodes.emplace("e", 100.0);
  nodes.emplace("f", 100.0);

  nodes["a"].AddEdge(&nodes["b"]);
  nodes["b"].AddEdge(&nodes["c"]);
  nodes["b"].AddEdge(&nodes["d"]);
  nodes["c"].AddEdge(&nodes["a"]);
  nodes["c"].AddEdge(&nodes["b"]);
  nodes["c"].AddEdge(&nodes["e"]);
  nodes["c"].AddEdge(&nodes["f"]);
  nodes["d"].AddEdge(&nodes["b"]);
  nodes["d"].AddEdge(&nodes["e"]);
  nodes["d"].AddEdge(&nodes["f"]);
  nodes["e"].AddEdge(&nodes["d"]);
  nodes["f"].AddEdge(&nodes["c"]);

  PrintNodes(0, nodes);
  for (int i = 1; i <= 20; i++) {
    PageRank(&nodes);
    PrintNodes(i, nodes);
  }
  return 0;
}
