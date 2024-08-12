#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>

#include "game.hpp"
#include "solver.hpp"

namespace procon35 {
    namespace solver {
        Node::Node() = default;
        Node::Node(procon35::game::Board b) : board(b), operation({0, 0, 0, 0}), cost(0), heuristic(0), parent(nullptr) {}
        Node::~Node() {}
        Node::Node(const Node &other) {
            board = other.board;
            operation = other.operation;
            cost = other.cost;
            heuristic = other.heuristic;
            parent = other.parent;
        }
        bool Node::operator==(Node& other) {
            return board == other.board;
        }
        struct Coordinate {
            int x;
            int y;
        };
        double Node::calculateHeuristics(Node *end, int mode) {
            if(mode == 1) {
                // 今のboardとgoalのboard間で，値が違うマスが何個あるかを数える
                int not_same_value_num = 0;
        
                for(int i = 0; i < board.height; i++) {
                    for(int j = 0; j < board.width; j++) {
                        if(board.getValue(j, i) != end->board.getValue(j, i)) {
                            not_same_value_num++;
                        }
                    }
                }
            
                return (double)not_same_value_num;
            } else if(mode == 2) {
                // 各マスについて，goalのboardの値がどのくらい近傍にあるかを数える
                int distance_sum = 0;
                int distance_max = 2;
                vector<struct Coordinate> neighborhood{
                    // distance = 0
                    {0, 0},
                    // distance = 1
                    {0, 1}, {0, -1}, {1, 0}, {-1, 0},
                    {1, 1}, {1, -1}, {-1, 1}, {-1, -1},
                    // distance = 2
                    {2, 0}, {-2, 0}, {0, 2}, {0, -2},
                    {2, 2}, {2, -2}, {-2, 2}, {-2, -2},
                    {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
                    {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
                };
                
                int x, y;
                for(int i = 0; i < board.height; i++) {
                    for(int j = 0; j < board.width; j++) {
                        int target = end->board.getValue(j, i);
                        for(auto& n : neighborhood) {
                            x = j + n.x;
                            y = i + n.y;
                            if(x < 0 || x >= board.width || y < 0 || y >= board.height) continue;
                            if(board.getValue(x, y) == target) {
                                distance_sum += std::max(abs(n.x), abs(n.y));
                                break;
                            }
                        }
                        distance_sum += distance_max + 1;
                    }
                }

                return (double)distance_sum;
            } else if(mode == 3) {
                // BitBoardでしか使えないが高速
                int not_same_value_num = 0;

                bitboard_value_type diff;
                for(int i = 0; i < board.height; i++) {
                    // 切り上げ
                    for(int j = 0; j < ((board.width + (bitboard_value_type_size / 2)) - 1) / (bitboard_value_type_size / 2); j++) {
                        diff = board.board.at(i).at(j) ^ end->board.board.at(i).at(j);
                        while(diff != 0) {
                            if((diff & 0b11) != 0) {
                                not_same_value_num++;
                            }
                            diff = diff >> 2;
                        }
                    }
                }

                return (double)not_same_value_num;
            }
        }
        void Node::print() {
            std::cout << "board: " << std::endl;
            for(int i = 0; i < board.height; i++) {
                for(int j = 0; j < board.width; j++) {
                    int n = board.getValue(j, i);
                    if(n / 10 == 0) {
                        std::cout << "  " << n;
                    } else {
                        std::cout << " " << n;
                    }
                }
                std::cout << std::endl;
            }
            std::cout << "operation; p:" << operation.p << " x:" << operation.x << " y:" << operation.y << " s:" << operation.s << std::endl;
            std::cout << "cost: " << cost << std::endl;
            std::cout << "heuristic: " << heuristic << std::endl;
            std::cout << "parent: " << parent << std::endl;
        }

        bool CompareNode::operator()(Node* a, Node* b) {
            return (a->cost + a->heuristic) > (b->cost + b->heuristic);
        }

        size_t NodeHash::operator()(const Node* node) const {
            size_t seed = node->board.board.size() * node->board.board.at(0).size();
            for (auto y : node->board.board) {
                for(auto x : y) {
                    x = ((x >> 16) ^ x) * 0x45d9f3b;
                    x = ((x >> 16) ^ x) * 0x45d9f3b;
                    x = (x >> 16) ^ x;
                    seed ^= x + 0x9e3779b9 + (seed << 6) + (seed >> 2);
                }
            }
            return seed;
        }

        bool NodeEqual::operator()(const Node* a, const Node* b) const {
            return a->board.board == b->board.board;
        }

        bool CompareNodeReverse::operator()(Node* a, Node* b) {
            return (a->cost + a->heuristic) < (b->cost + b->heuristic);
        }
    }
}
