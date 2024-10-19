#pragma once

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
            struct procon35::game::Board board;
            procon35::game::Operation operation; // parent's operation
            double cost;
            double heuristic;
            Node* parent;
        
            Node();
            Node(struct procon35::game::Board b);
            ~Node();
            Node(const Node& other);
            bool operator==(const Node &other);
            double calculateHeuristics(Node* end, int mode);
            void print();
        };

        class CompareNode {
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

        class Solver_Sequential : Solver {
        public:
            Solver_Sequential();
            ~Solver_Sequential();
            procon35::game::Answer solve(procon35::game::Game game, procon35::game::Problem problem);
        };

        class Solver_Example : Solver {
        public:
            Solver_Example();
            ~Solver_Example();
            procon35::game::Answer solve(procon35::game::Game game, procon35::game::Problem problem);
        };

    }
}
