#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>

#include "game.hpp"
#include "solver.hpp"

namespace procon35 {
    namespace solver {
        Node::Node() = default;
        Node::Node(struct procon35::game::Board b) : board(b), operation({0, 0, 0, 0}), cost(0), heuristic(0), parent(nullptr) {}
        Node::~Node() {}
        Node::Node(const Node &other) {
            board = other.board;
            operation = other.operation;
            cost = other.cost;
            heuristic = other.heuristic;
            parent = other.parent;
        }
        bool Node::operator==(const Node &other) {
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
                        if(board.board.at(i).at(j) != end->board.board.at(i).at(j)) {
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
                        int target = end->board.board.at(i).at(j);
                        for(auto& n : neighborhood) {
                            x = j + n.x;
                            y = i + n.y;
                            if(x < 0 || x >= board.width || y < 0 || y >= board.height) continue;
                            if(board.board.at(y).at(x) == target) {
                                distance_sum += std::max(abs(n.x), abs(n.y));
                                break;
                            }
                        }
                        distance_sum += distance_max + 1;
                    }
                }

                return (double)distance_sum;
            }
        }
        void Node::print() {
            std::cout << "board: " << std::endl;
            for(int i = 0; i < board.height; i++) {
                for(int j = 0; j < board.width; j++) {
                    int n = board.board.at(i).at(j);
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

        Solver_AStar::Solver_AStar() {}
        Solver_AStar::~Solver_AStar() {}
        procon35::game::Answer Solver_AStar::solve(procon35::game::Game game, procon35::game::Problem problem) {
            std::priority_queue<Node*, vector<Node*>, CompareNode> open_queue;
            vector<Node*> closed_set;
            procon35::game::Answer answer;
            Node start_node(problem.start_board), goal_node(problem.goal_board);
            // 一手のコスト
            double move_cost = 1.0;
            // コストに対するヒューリスティクスの重み
            double weight_heuristics = 2.0;
            // 使用するヒューリステック関数
            int mode = 1;
            
            std::cout << "log: solver started." << std::endl;

            start_node.heuristic = start_node.calculateHeuristics(&goal_node, mode) * weight_heuristics;
            open_queue.push(&start_node);

            // ログ出力用 探索したノード数をカウントする
            int count = 0;
            while(!open_queue.empty()) {
                count++;
                std::cout << "log: solver count: " << count << "  " << "cost: " << open_queue.top()->cost << "  " << "heuristic: " << open_queue.top()->heuristic;
                // std::cout << "  open queue: " << open_queue.size() << "  " << "closed set: " << closed_set.size();
                std::cout << std::endl;

                Node* current_node = open_queue.top();
                open_queue.pop();

                if(current_node->board == goal_node.board) {
                    std::cout << "log: solver finished; solved." << std::endl;
                    answer.solved = true;
                    answer.operations = reconstructPath(current_node);
                    return answer;
                }

                closed_set.push_back(current_node);

                for(struct procon35::game::Operation neighbor : game.getAvailableOperations(current_node->board, problem.patterns)) {
                    struct procon35::game::Board neighbor_board = game.operate(current_node->board, neighbor, problem.patterns.at(neighbor.p));
                    Node* neighbor_node = new Node(neighbor_board);

                    neighbor_node->operation = neighbor;

                    auto it = std::find(closed_set.begin(), closed_set.end(), neighbor_node);
                    if (it == closed_set.end()) {
                        neighbor_node->cost = current_node->cost + move_cost;
                        neighbor_node->heuristic = neighbor_node->calculateHeuristics(&goal_node, mode) * weight_heuristics;
                        neighbor_node->parent = current_node;
                        open_queue.push(neighbor_node);
                    }
                }
            }

            std::cout << "log: solver finished; unsolved." << std::endl;

            answer.solved = false;
            Node* current_node = open_queue.top();
            answer.operations = reconstructPath(current_node);

            return answer;
        }

        vector<struct procon35::game::Operation> Solver_AStar::reconstructPath(Node* current_node) {
            vector<struct procon35::game::Operation> path;
            while(current_node->parent != nullptr) {
                path.push_back(current_node->operation);
                current_node = current_node->parent;
            }
            std::reverse(path.begin(), path.end());
            return path;
        }

    }
}
