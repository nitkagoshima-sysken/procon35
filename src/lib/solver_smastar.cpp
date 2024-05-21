#include <algorithm>
#include <iostream>
#include <vector>
#include <queue>
#include <unordered_set>

#include "game.hpp"
#include "solver.hpp"

namespace procon35 {
    namespace solver {
        procon35::game::Answer Solver_SMAStar::solve(procon35::game::Game game, procon35::game::Problem problem, int memory_bound) {
            std::priority_queue<Node*, vector<Node*>, CompareNode> open_queue;
            std::unordered_set<Node*, NodeHash, NodeEqual> closed_set;
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
            while (!open_queue.empty()) {
                count++;
                std::cout << "log: solver count: " << count << " " << "cost: " << open_queue.top()->cost << " " << "heuristic: " << open_queue.top()->heuristic;
                // std::cout << "  open queue: " << open_queue.size() << "  " << "closed set: " << closed_set.size();
                std::cout << std::endl;

                Node* current_node = open_queue.top();
                open_queue.pop();

                if (current_node->board == goal_node.board) {
                    std::cout << "log: solver finished; solved." << std::endl;
                    answer.solved = true;
                    answer.operations = reconstructPath(current_node);
                    return answer;
                }

                closed_set.insert(current_node);

                if (open_queue.size() > memory_bound) {
                    // memory limit reached, prune the search tree
                    pruneSearchTree(open_queue, closed_set, memory_bound);
                }

                for (struct procon35::game::Operation neighbor : game.getAvailableOperations(current_node->board, problem.patterns)) {
                    procon35::game::Board neighbor_board = game.operate(current_node->board, neighbor, problem.patterns.at(neighbor.p));
                    Node* neighbor_node = new Node(neighbor_board);
                    neighbor_node->operation = neighbor;

                    if (closed_set.find(neighbor_node) == closed_set.end()) {
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

        void Solver_SMAStar::pruneSearchTree(std::priority_queue<Node*, vector<Node*>, CompareNode>& open_queue, std::unordered_set<Node*, NodeHash, NodeEqual>& closed_set, int memory_bound) {
            // メモリ許容値を超えるノードを削除する
            // 評価値が悪い順に並び替えて削除していく
            std::priority_queue<Node*, vector<Node*>, CompareNodeReverse> tmp_queue;
            while (!open_queue.empty()) {
                tmp_queue.push(open_queue.top());
                open_queue.pop();
            }
            while (tmp_queue.size() > memory_bound) {
                Node* node_to_remove = tmp_queue.top();
                tmp_queue.pop();
                closed_set.erase(node_to_remove);
                delete node_to_remove;
            }
            while(!tmp_queue.empty()) {
                open_queue.push(tmp_queue.top());
                tmp_queue.pop();
            }
        }

        vector<struct procon35::game::Operation> Solver_SMAStar::reconstructPath(Node* current_node) {
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
