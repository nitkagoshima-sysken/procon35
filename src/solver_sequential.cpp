#include <algorithm>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <vector>
#include <queue>

#include "game.hpp"
#include "solver.hpp"

namespace procon35 {
    namespace solver {
        struct Coordinate {
            int x;
            int y;
        };

        Solver_Sequential::Solver_Sequential() {}
        Solver_Sequential::~Solver_Sequential() {}

        struct Coordinate value_search(struct procon35::game::Board board, struct Coordinate aligning_coordinate, int target_value, int aligned_y_min) {
            struct Coordinate coordinate = aligning_coordinate;

            // 手数の少ない真下から探索 揃えたい数字と一致する数字を探索する
            for(int y = aligning_coordinate.y; y < board.height && y < aligned_y_min; y++) {
                if(board.board.at(y).at(aligning_coordinate.x) == target_value) {
                    coordinate.x = aligning_coordinate.x;
                    coordinate.y = y;
                    return coordinate;
                }
            }

            for(int x = aligning_coordinate.x; x < board.width; x++) {
                if(board.board.at(aligning_coordinate.y).at(x) == target_value) {
                    coordinate.x = x;
                    coordinate.y = aligning_coordinate.y;
                    return coordinate;
                }
            }

            for(int y = aligning_coordinate.y + 1; y < board.height && y < aligned_y_min; y++) {
                for(int x = 0; x < board.width; x++) {
                    if(board.board.at(y).at(x) == target_value) {
                        coordinate.x = x;
                        coordinate.y = y;
                        return coordinate;
                    }
                }
            }

            std::cout << "error: value not found in value_search" << std::endl;
            return coordinate;
        }

        void print(struct procon35::game::Board board) {
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
        }

        procon35::game::Answer Solver_Sequential::solve(procon35::game::Game game, procon35::game::Problem problem) {
            procon35::game::Answer answer;
            struct procon35::game::Board board = problem.start_board;
            struct procon35::game::Operation op = procon35::game::Operation(0, 0, 0, 0); // 1マスの抜き型
            struct Coordinate aligning_coordinate, target_coordinate;
            int target_value;
            int aligned_y_min = problem.goal_board.height;
            
            std::cout << "log: solver started." << std::endl;
            // print(board);

            for(int row = 0; row < board.height; row++) {
                // row行を0行目に揃えて一番下の行に移動
                for(int col = 0; col < board.width; col++) {
                    // std::cout << "log: solver row: " << row << " col: " << col << std::endl;

                    aligning_coordinate.x = col;
                    aligning_coordinate.y = 0;
                    target_value = problem.goal_board.board.at(row).at(col);

                    // print(board);
                    // target_valueを持つマスを(x:col, y:0)に移動
                    target_coordinate = value_search(board, aligning_coordinate, target_value, aligned_y_min);
                    // std::cout << "log: value target row: " << target_coordinate.y << " col: " << target_coordinate.x << std::endl;
                    // std::cout << std::endl;

                    // xを揃える
                    if(target_coordinate.x < aligning_coordinate.x) {
                        int diff = aligning_coordinate.x - target_coordinate.x;
                        op.p = 0;
                        op.s = procon35::game::LEFT;
                        op.x = board.width - 1;
                        op.y = target_coordinate.y;
                        for(int i = 0; i < diff; i++) {
                            board = game.operate(board, op, problem.patterns.at(op.p));
                            answer.operations.push_back(op);
                        }
                    } else if(target_coordinate.x > aligning_coordinate.x) {
                        int diff = target_coordinate.x - aligning_coordinate.x;
                        op.p = 0;
                        op.s = procon35::game::RIGHT;
                        if(target_coordinate.y == aligning_coordinate.y) {
                            op.x = aligning_coordinate.x;
                            op.y = aligning_coordinate.y;
                        } else {
                            op.x = 0;
                            op.y = target_coordinate.y;
                        }
                        for(int i = 0; i < diff; i++) {
                            board = game.operate(board, op, problem.patterns.at(op.p));
                            answer.operations.push_back(op);
                        }
                    }

                    // yを揃える
                    if(target_coordinate.y != aligning_coordinate.y) {
                        op.p = 1;
                        op.s = procon35::game::UP;
                        op.x = aligning_coordinate.x;
                        op.y = target_coordinate.y;
                        board = game.operate(board, op, problem.patterns.at(op.p));
                        answer.operations.push_back(op);
                    }
                }
                // 揃った0行目を下に移動
                op.p = 22;
                op.s = procon35::game::DOWN;
                op.x = 0;
                op.y = -255;
                board = game.operate(board, op, problem.patterns.at(op.p));
                answer.operations.push_back(op);
                aligned_y_min--;

                game.writeAnswer(answer, "./json/answer.json");
                std::cout << "log: progress: " << row + 1 << "/" << board.height << std::endl;
            }

            std::cout << "log: solver finished; solved." << std::endl;
            answer.solved = true;

            bool write_answer = true;
            if(write_answer) {
                std::filesystem::path path("./json/answer_board.txt");
                std::ofstream ofs(path);
                ofs << "board: " << std::endl;
                for(int i = 0; i < board.height; i++) {
                    for(int j = 0; j < board.width; j++) {
                        ofs << board.board.at(i).at(j);
                    }
                    ofs << std::endl;
                }
                ofs.close();
            }

            return answer;
        }
    }
}
