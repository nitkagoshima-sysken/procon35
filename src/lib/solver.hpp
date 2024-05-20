#pragma once

#include <queue>
#include <unordered_set>

#include "game.hpp"

namespace procon35 {
    namespace solver {
        class Solver {
        public:
            Solver();
            ~Solver();
            procon35::game::Answer solve(procon35::game::Game game, procon35::game::Problem problem);
        };

        class Node {
        public:
            procon35::game::Board board;
            procon35::game::Operation operation; // parent's operation
            double cost;
            double heuristic;
            Node* parent;
        
            Node();
            Node(procon35::game::Board b);
            ~Node();
            Node(const Node& other);
            bool operator==(Node& other);
            double calculateHeuristics(Node* end, int mode);
            void print();
        };

        class CompareNode {
        public:
            bool operator()(Node* a, Node* b);
        };

        class CompareNodeReverse {
        public:
            bool operator()(Node* a, Node* b);
        };

        class Solver_AStar : Solver {
        public:
            Solver_AStar();
            ~Solver_AStar();
            procon35::game::Answer solve(procon35::game::Game game, procon35::game::Problem problem);
            vector<struct procon35::game::Operation> reconstructPath(Node* current_node);
        };

        class NodeHash {
        public:
            size_t operator()(const Node* node) const;
        };

        class NodeEqual {
        public:
            bool operator()(const Node* a, const Node* b) const;
        };

        class Solver_SMAStar : Solver {
        public:
            procon35::game::Answer solve(procon35::game::Game game, procon35::game::Problem problem, int memory_bound);
            void pruneSearchTree(std::priority_queue<Node*, vector<Node*>, CompareNode>& open_queue, std::unordered_set<Node*, NodeHash, NodeEqual>& closed_set, int memory_bound);
            vector<struct procon35::game::Operation> reconstructPath(Node* current_node);
        };

        class Solver_Example : Solver {
        public:
            Solver_Example();
            ~Solver_Example();
            procon35::game::Answer solve(procon35::game::Game game, procon35::game::Problem problem);
        };

    }
}
