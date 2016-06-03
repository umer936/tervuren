// Copyright 2015 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
// License for the specific language governing permissions and limitations under
// the License.

#include "test_graphs.h"

#include "dot_printer.h"
#include "type.h"
#include "util/logging.h"
#include "value.h"

namespace {

// Error messages.
const char kInitializationErr[] = "The graph is not initialized.";
const char kGraphNodeErr[] = "Invalid node identifier.";

// Tags and names for components of labels.
const char kNodeWeightTag[] = "Node-Weight";
const char kEdgeWeightTag[] = "Edge-Weight";
const char kWeightedGraphTag[] = "Weighted-Graph";

}  // namespace

namespace logle {
namespace test {

namespace type = ast::type;
namespace value = ast::value;

util::Status WeightedGraph::Initialize() {
  type::Types node_types;
  node_types.emplace(kNodeWeightTag,
                     type::MakeInt(kNodeWeightTag, /*Must not be null*/ false));
  type::Types edge_types;
  edge_types.emplace(kEdgeWeightTag,
                     type::MakeInt(kEdgeWeightTag, /*Must not be null*/ false));
  AST graph_type = type::MakeNull(kWeightedGraphTag);
  util::Status s =
      graph_.Initialize(node_types, {}, edge_types, {}, graph_type);
  if (s.ok()) {
    is_initialized_ = true;
    return s;
  }
  return util::Status(Code::INTERNAL, s.message());
}

NodeId WeightedGraph::AddNode(int node_weight) {
  TaggedAST label;
  label.set_tag(kNodeWeightTag);
  *label.mutable_ast() = value::MakeInt(node_weight);
  return graph_.FindOrAddNode(label);
}

EdgeId WeightedGraph::AddEdge(NodeId src, NodeId tgt, int edge_weight) {
  CHECK(src >= 0 && src < graph_.NumNodes(), kGraphNodeErr);
  CHECK(tgt >= 0 && tgt < graph_.NumNodes(), kGraphNodeErr);
  TaggedAST label;
  label.set_tag(kEdgeWeightTag);
  *label.mutable_ast() = value::MakeInt(edge_weight);
  return graph_.FindOrAddEdge(src, tgt, label);
}

set<NodeId> WeightedGraph::GetNodes(int node_weight) const {
  TaggedAST label;
  label.set_tag(kNodeWeightTag);
  *label.mutable_ast() = value::MakeInt(node_weight);
  return graph_.GetNodes(label);
}

const LabeledGraph* WeightedGraph::GetGraph() const {
  return &graph_;
}

int WeightedGraph::NumNodes() const {
  CHECK(is_initialized_, kInitializationErr);
  return graph_.NumNodes();
}

int WeightedGraph::NumEdges() const {
  CHECK(is_initialized_, kInitializationErr);
  return graph_.NumEdges();
}

string WeightedGraph::ToDot() const {
  CHECK(is_initialized_, kInitializationErr);
  return DotPrinter().DotGraph(graph_);
}

void GetPathGraph(int num_nodes, WeightedGraph* graph) {
  CHECK(graph != nullptr, "Pointer to graph is null");
  if (num_nodes < 1 || !graph->Initialize().ok()) {
    return;
  }
  NodeId zero = graph->AddNode(0);
  NodeId src = zero;
  NodeId tgt = src;
  for (int i = 1; i < num_nodes; ++i) {
    tgt = graph->AddNode(i);
    graph->AddEdge(src, tgt, i - 1);
    src = tgt;
  }
}

void GetCycleGraph(int num_nodes, WeightedGraph* graph) {
  // The call to GetPathGraph will crash if 'graph' is null, so subsequent
  // dereferences are well defined.
  GetPathGraph(num_nodes, graph);
  set<NodeId> first_nodes = graph->GetNodes(0);
  set<NodeId> last_nodes = graph->GetNodes(num_nodes - 1);
  graph->AddEdge(*last_nodes.begin(), *first_nodes.begin(), num_nodes - 1);
}

}  // namespace test
}  // namespace logle
