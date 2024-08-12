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
            double weight_heuristics = 1.0;
            // 使用するヒューリステック関数
            int mode = 3;

            std::cout << "log: solver started." << std::endl;

            start_node.heuristic = start_node.calculateHeuristics(&goal_node, mode) * weight_heuristics;
            open_queue.push(&start_node);
            auto neighbors = game.getAvailableOperations(start_node.board, problem.patterns);

            std::chrono::system_clock::time_point start1, end1, sum1, start2, end2, sum2, base;

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

                sum1 = std::chrono::system_clock::now();
                sum2 = std::chrono::system_clock::now();
                base = std::chrono::system_clock::now();
                int count2 = 0;
                for (struct procon35::game::Operation neighbor : neighbors) {
                    count2++;
                    if(count2 == 100000) {
                        std::chrono::milliseconds ms1 = std::chrono::duration_cast<std::chrono::milliseconds>(sum1 - base);
                        std::cout << "time of operate() : " << ms1.count() << "[ms]" << std::endl;
                        std::chrono::milliseconds ms2 = std::chrono::duration_cast<std::chrono::milliseconds>(sum2 - base);
                        std::cout << "time of calculateHeuristics() : " << ms2.count() << "[ms]" << std::endl;
                    }
                    start1 = std::chrono::system_clock::now();/////////
                    procon35::game::Board neighbor_board = game.operate(current_node->board, neighbor, problem.patterns.at(neighbor.p));
                    procon35::game::Board neighbor_board2 = game.operateBitBoard(current_node->board, neighbor, problem.patterns.at(neighbor.p));
                    if(!(neighbor_board == neighbor_board2)) {
                        std::cout << "operate diff at " << neighbor.p << " " << neighbor.x << " " << neighbor.y << " " << neighbor.s << std::endl;
                        // if(neighbor.s == 2) {
                        //     std::cout << "!!!operate diff at " << neighbor.p << " " << neighbor.x << " " << neighbor.y << " " << neighbor.s << std::endl;
                        // }
                    }else
                    {
                        // std::cout << "!!!operate same at " << neighbor.p << " " << neighbor.x << " " << neighbor.y << " " << neighbor.s << std::endl;
                    }
                    
                    end1 = std::chrono::system_clock::now();/////////
                    sum1 += end1 - start1;///////////
                    Node* neighbor_node = new Node(neighbor_board);
                    neighbor_node->operation = neighbor;

                    if (closed_set.find(neighbor_node) == closed_set.end()) {
                        neighbor_node->cost = current_node->cost + move_cost;
                    start2 = std::chrono::system_clock::now();/////////
                        neighbor_node->heuristic = neighbor_node->calculateHeuristics(&goal_node, mode) * weight_heuristics;
                    end2 = std::chrono::system_clock::now();/////////
                    sum2 += end2 - start2;///////////
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
