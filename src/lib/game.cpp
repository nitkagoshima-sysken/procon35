#include <vector>
#include <filesystem>
#include <fstream>
#include <iostream>

#include <nlohmann/json.hpp>

#include "game.hpp"

using std::vector;
using json = nlohmann::json;

namespace procon35 {
    namespace game {
        ArrayBoard::ArrayBoard() = default;
        ArrayBoard::ArrayBoard(int width, int height, vector<vector<int> > board) : width(width), height(height), board(board) {}
        ArrayBoard::ArrayBoard(const ArrayBoard& other) {
            width = other.width;
            height = other.height;
            board = other.board;
        }
        ArrayBoard& ArrayBoard::operator=(const ArrayBoard& other) {
            width = other.width;
            height = other.height;
            board = other.board;
            return *this;
        }
        bool ArrayBoard::operator==(ArrayBoard& other) {
            if(width != other.width) return false;
            if(height != other.height) return false;
            for(int i = 0; i < height; i++) {
                for(int j = 0; j < width; j++) {
                    if(getValue(j, i) != other.getValue(j, i)) return false;
                }
            }
            return true;
        }
        int ArrayBoard::getValue(int x, int y) {
            return board.at(y).at(x);
        }
        void ArrayBoard::setValue(int x, int y, int value) {
            board.at(y).at(x) = value;
        }
        void ArrayBoard::appendLine() {
            board.push_back(vector<int>());
        }
        void ArrayBoard::appendValue(int value) {
            board.back().push_back(value);
        }

        BitBoard::BitBoard() = default;
        BitBoard::BitBoard(int width, int height, vector<vector<bitboard_value_type> > board) : width(width), height(height), board(board) {}
        BitBoard::BitBoard(const BitBoard& other) {
            width = other.width;
            height = other.height;
            board = other.board;
            width_end = other.width_end;
        }
        BitBoard& BitBoard::operator=(const BitBoard& other) {
            width = other.width;
            height = other.height;
            board = other.board;
            width_end = other.width_end;
            return *this;
        }
        bool BitBoard::operator==(BitBoard& other) {
            if(width != other.width) return false;
            if(height != other.height) return false;
            for(int i = 0; i < height; i++) {
                for(int j = 0; j < width; j++) {
                    if(getValue(j, i) != other.getValue(j, i)) return false;
                }
            }
            return true;
        }
        int BitBoard::getValue(int x, int y) {
            if(x < 0 || x >= width || y < 0 || y >= height) {
                std::cout << __FILE__ << ":" << __LINE__ << ": " << "error: invalid x or y. x:" << x << " y:" << y << std::endl;
            }
            int index = x / (bitboard_value_type_size / 2);
            int shift = ((bitboard_value_type_size / 2) - 1) - (x % (bitboard_value_type_size / 2));
            int value = board.at(y).at(index) >> (shift * 2);
            return value & 0b11;
        }
        void BitBoard::setValue(int x, int y, int value) {
            if(x < 0 || x >= width || y < 0 || y >= height) {
                std::cout << __FILE__ << ":" << __LINE__ << ": " << "error: invalid x or y. x:" << x << " y:" << y << std::endl;
            }
            if(value < 0 || value > 3) {
                std::cout << __FILE__ << ":" << __LINE__ << ": " << "error: invalid value. value:" << value << std::endl;
            }
            int index = x / (bitboard_value_type_size / 2);
            int shift = ((bitboard_value_type_size / 2) - 1) - (x % (bitboard_value_type_size / 2));
            board.at(y).at(index) = (board.at(y).at(index) & ~(0b11 << (shift * 2))) | ((value & 0b11) << (shift * 2));
        }
        void BitBoard::appendLine() {
            board.push_back(vector<bitboard_value_type>());
            width_end = 0;
        }
        void BitBoard::appendValue(int value) {
            if(value < 0 || value > 3) {
                std::cout << __FILE__ << ":" << __LINE__ << ": " << "error: invalid value. value:" << value << std::endl;
            }
            if(width_end == 0) {
                board.back().push_back(0);
            }
            setValue(((board.back().size() - 1) * (bitboard_value_type_size / 2)) + width_end, board.size() - 1, value);
            width_end++;
            if(width_end >= (bitboard_value_type_size / 2)) {
                width_end = 0;
            }
        }

        Pattern::Pattern() = default;
        Pattern::Pattern(int p, int width, int height, vector<vector<int> > cells) : p(p), width(width), height(height), cells(cells) {}
        Pattern::Pattern(const struct Pattern& other) {
            p = other.p;
            width = other.width;
            height = other.height;
            cells = other.cells;
        }
        struct Pattern& Pattern::operator=(const struct Pattern& other) {
            p = other.p;
            width = other.width;
            height = other.height;
            cells = other.cells;
            return *this;
        }

        Operation::Operation() = default;
        Operation::Operation(int p, int x, int y, int s) : p(p), x(x), y(y), s(s) {}
        Operation::Operation(const struct Operation& other) {
            p = other.p;
            x = other.x;
            y = other.y;
            s = other.s;
        }
        struct Operation& Operation::operator=(const struct Operation& other) {
            p = other.p;
            x = other.x;
            y = other.y;
            s = other.s;
            return *this;
        }

        Problem::Problem() {
            start_board = Board();
            goal_board = Board();
            patterns.clear();
        }
        Problem::~Problem() {}
        Problem::Problem(const Problem& other) {
            start_board = other.start_board;
            goal_board = other.goal_board;
            patterns = other.patterns;
        }
        Problem& Problem::operator=(const Problem& other) {
            start_board = other.start_board;
            goal_board = other.goal_board;
            patterns = other.patterns;
            return *this;
        }

        Answer::Answer() {
            solved = false;
            operations.clear();
        }
        Answer::~Answer() {}
        Answer::Answer(const Answer& other) {
            solved = other.solved;
            operations = other.operations;
        }
        Answer& Answer::operator=(const Answer& other) {
            solved = other.solved;
            operations = other.operations;
            return *this;
        }

        Game::Game() {}
        Game::~Game() {}

        Problem Game::loadProblem(std::filesystem::path path) {
            Problem problem;
            std::ifstream ifs(path);
            json data = json::parse(ifs);

            problem.start_board.width = data["board"]["width"];
            problem.start_board.height = data["board"]["height"];
            problem.start_board.board.clear();
            for(auto& _row: data["board"]["start"]) {
                std::string row = _row;
                problem.start_board.appendLine();
                // change row string to int; "220103" -> [2,2,0,1,0,3]
                for(auto& x: row) {
                    problem.start_board.appendValue((int)x - '0');
                }
            }
            problem.goal_board.width = data["board"]["width"];
            problem.goal_board.height = data["board"]["height"];
            problem.goal_board.board.clear();
            for(auto& _row: data["board"]["goal"]) {
                std::string row = _row;
                problem.goal_board.appendLine();
                for(auto& x: row) {
                    problem.goal_board.appendValue((int)x - '0');
                }
            }

            problem.patterns.clear();
            for(auto& pattern: getStandardPatterns()) {
                problem.patterns.push_back(pattern);
            }
            for(auto& pattern: data["general"]["patterns"]) {
                problem.patterns.push_back(Pattern{});
                problem.patterns.back().p = pattern["p"];
                problem.patterns.back().width = pattern["width"];
                problem.patterns.back().height = pattern["height"];
                problem.patterns.back().cells.clear();
                for(auto& _row: pattern["cells"]) {
                    std::string row = _row;
                    problem.patterns.back().cells.push_back(vector<int>());
                    for(auto& x: row) {
                        problem.patterns.back().cells.back().push_back((int)x - '0');
                    }
                }
            }

            ifs.close();

            return problem;
        }

        void Game::writeAnswer(Answer answer, std::filesystem::path path) {
            std::ofstream ofs(path);
            json data;
            data["n"] = answer.operations.size();
            for(auto& op: answer.operations) {
                json operation;
                operation["p"] = op.p;
                operation["x"] = op.x;
                operation["y"] = op.y;
                operation["s"] = op.s;
                data["ops"].push_back(operation);
            }
            ofs << data.dump(4);
            ofs.close();
        }

        Board Game::operate(Board board, struct Operation op, struct Pattern pattern) {
            int board_x, board_y;
            
            if(op.s == UP) {
                for(int j = 0; j < pattern.width; j++) {
                    board_x = op.x + j;
                    if(board_x < 0 || board_x >= board.width) {
                        continue;
                    }
                    int shifted_piece_count = 0;
                    for(int i = 0; i < pattern.height; i++) {
                        board_y = op.y + i;
                        if(0 <= board_y && board_y < board.height) {
                            if(pattern.cells.at(i).at(j) == 1) {
                                for(int k = board_y; k > shifted_piece_count; k--) {
                                    swap(&board, board_x, k, board_x, k - 1);
                                }
                                shifted_piece_count++;
                            }
                        }
                    }
                }
            } else if(op.s == DOWN) {
                for(int j = 0; j < pattern.width; j++) {
                    board_x = op.x + j;
                    if(board_x < 0 || board_x >= board.width) {
                        continue;
                    }
                    int shifted_piece_count = 0;
                    for(int i = pattern.height - 1; i >= 0; i--) {
                        board_y = op.y + i;
                        if(0 <= board_y && board_y < board.height) {
                            if(pattern.cells.at(i).at(j) == 1) {
                                for(int k = board_y; k < board.height - 1 - shifted_piece_count; k++) {
                                    swap(&board, board_x, k, board_x, k + 1);
                                }
                                shifted_piece_count++;
                            }
                        }
                    }
                }
            } else if(op.s == LEFT) {
                for(int i = 0; i < pattern.height; i++) {
                    board_y = op.y + i;
                    if(board_y < 0 || board_y >= board.height) {
                        continue;
                    }
                    int shifted_piece_count = 0;
                    for(int j = 0; j < pattern.width; j++) {
                        board_x = op.x + j;
                        if(0 <= board_x && board_x < board.width) {
                            if(pattern.cells.at(i).at(j) == 1) {
                                for(int k = board_x; k > shifted_piece_count; k--) {
                                    swap(&board, k, board_y, k - 1, board_y);
                                }
                                shifted_piece_count++;
                            }
                        }
                    }
                }
            } else if(op.s == RIGHT) {
                for(int i = 0; i < pattern.height; i++) {
                    board_y = op.y + i;
                    if(board_y < 0 || board_y >= board.height) {
                        continue;
                    }
                    int shifted_piece_count = 0;
                    for(int j = pattern.width - 1; j >= 0; j--) {
                        board_x = op.x + j;
                        if(0 <= board_x && board_x < board.width) {
                            if(pattern.cells.at(i).at(j) == 1) {
                                for(int k = board_x; k < board.width - 1 - shifted_piece_count; k++) {
                                    swap(&board, k, board_y, k + 1, board_y);
                                }
                                shifted_piece_count++;
                            }
                        }
                    }
                }
            }

            return board;
        }

        // inline void _swap(Board* board, int x1, int y1, int x2, int y2) {
        //     int index1 = x1 / (bitboard_value_type_size / 2);
        //     int shift1 = ((bitboard_value_type_size / 2) - 1) - (x1 % (bitboard_value_type_size / 2));
        //     int tmp1 = (board->board.at(y1).at(index1) >> (shift1 * 2)) & 0b11;
        //     int index2 = x2 / (bitboard_value_type_size / 2);
        //     int shift2 = ((bitboard_value_type_size / 2) - 1) - (x2 % (bitboard_value_type_size / 2));
        //     int tmp2 = (board->board.at(y2).at(index2) >> (shift2 * 2)) & 0b11;
        //     board->board.at(y1).at(index1) = (board->board.at(y1).at(index1) & ~(0b11 << (shift1 * 2))) | (tmp2 << (shift1 * 2));
        //     board->board.at(y2).at(index2) = (board->board.at(y2).at(index2) & ~(0b11 << (shift2 * 2))) | (tmp1 << (shift2 * 2));
        // }

        // debug
        void print(Board board) {
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
        }

        Board Game::operateBitBoard(Board board, struct Operation op, struct Pattern pattern) {
            // BitBoard用の高速なoperate()
            // 逐次交換ではなく，循環シフトのように操作する
            int board_x, board_y;
            int shifted_piece_count;
            int index, shift;
            int target_index, target_shift;
            bitboard_value_type move_value;
            // bitboard_value_type mask;

            // TODO: 不動のときどうなる？ 0->0にup,left, max->maxにdown,right
            // 左シフトが溢れたら循環してしまう

            // std::cout << "in operateBitBoard" << std::endl;
            
            if(op.s == UP) {
                for(int j = 0; j < pattern.width; j++) {
                    board_x = op.x + j;
                    if(board_x < 0 || board_x >= board.width) {
                        continue;
                    }
                    index = board_x / (bitboard_value_type_size / 2);
                    shift = ((bitboard_value_type_size / 2) - 1) - (board_x % (bitboard_value_type_size / 2));
                    // mask = 0b11 << (shift * 2);
                    shifted_piece_count = 0;
                    for(int i = 0; i < pattern.height; i++) {
                        board_y = op.y + i;
                        if(0 <= board_y && board_y < board.height) {
                            if(pattern.cells.at(i).at(j) == 1) {
                                // TODO: maskに置き換えると速くなる？
                                move_value = board.board.at(board_y).at(index) & (0b11 << (shift * 2));
                                for(int k = board_y; k > shifted_piece_count; k--) {
                                    board.board.at(k).at(index) = 
                                        (board.board.at(k).at(index) & ~(0b11 << (shift * 2)))
                                        | ((board.board.at(k - 1).at(index) & (0b11 << (shift * 2))));
                                }
                                board.board.at(shifted_piece_count).at(index) = (board.board.at(shifted_piece_count).at(index) & ~(0b11 << (shift * 2))) | move_value;

                                shifted_piece_count++;
                            }
                        }
                    }
                }
            } else if(op.s == DOWN) {
                for(int j = 0; j < pattern.width; j++) {
                    board_x = op.x + j;
                    if(board_x < 0 || board_x >= board.width) {
                        continue;
                    }
                    index = board_x / (bitboard_value_type_size / 2);
                    shift = ((bitboard_value_type_size / 2) - 1) - (board_x % (bitboard_value_type_size / 2));
                    // mask = 0b11 << (shift * 2);
                    shifted_piece_count = 0;
                    for(int i = pattern.height - 1; i >= 0; i--) {
                        board_y = op.y + i;
                        if(0 <= board_y && board_y < board.height) {
                            if(pattern.cells.at(i).at(j) == 1) {
                                move_value = board.board.at(board_y).at(index) & (0b11 << (shift * 2));
                                for(int k = board_y; k < board.height - 1 - shifted_piece_count; k++) {
                                    board.board.at(k).at(index) = 
                                        (board.board.at(k).at(index) & ~(0b11 << (shift * 2)))
                                        | ((board.board.at(k + 1).at(index) & (0b11 << (shift * 2))));
                                }
                                board.board.at(board.height - 1 - shifted_piece_count).at(index) = (board.board.at(board.height - 1 - shifted_piece_count).at(index) & ~(0b11 << (shift * 2))) | move_value;

                                shifted_piece_count++;
                            }
                        }
                    }
                }
            } else if(op.s == LEFT) {
                // std::cout << "LEFT" << std::endl;
                for(int i = 0; i < pattern.height; i++) {
                    board_y = op.y + i;
                    if(board_y < 0 || board_y >= board.height) {
                        continue;
                    }
                    shifted_piece_count = 0;
                    for(int j = 0; j < pattern.width; j++) {
                        board_x = op.x + j;
                        if(0 <= board_x && board_x < board.width) {
                            if(pattern.cells.at(i).at(j) == 1) {
                                // std::cout << "move" << std::endl;
                                std::cout << "before:" << std::endl;
                                print(board);
                                index = board_x / (bitboard_value_type_size / 2);
                                shift = ((bitboard_value_type_size / 2) - 1) - (board_x % (bitboard_value_type_size / 2));
                                move_value = (board.board.at(board_y).at(index) >> (shift * 2)) & 0b11;

                                // std::cout << "log: line" << __LINE__ << std::endl;
                                
                                target_index = shifted_piece_count / (bitboard_value_type_size / 2);
                                target_shift = ((bitboard_value_type_size / 2) - 1) - (shifted_piece_count % (bitboard_value_type_size / 2));

                                // std::cout << "log: line" << __LINE__ << std::endl;

                                std::cout << "index: " << index << std::endl;
                                std::cout << "shift: " << shift << std::endl;
                                std::cout << "target_index: " << target_index << std::endl;
                                std::cout << "target_shift: " << target_shift << std::endl;
                                std::cout << "move_value: " << move_value << std::endl;

                                // board.board.at(board_y).at(index) = board.board.at(board_y).at(index) & ~(0b11 << (shift * 2));
                                // print(board);

                                // debug
                                // (board.board.at(board_y).at(index) & ((0b1 << (shift * 2)) - 1));
                                // // std::cout << "log: line" << __LINE__ << std::endl;
                                // ((board.board.at(board_y).at(index) & ~((0b1 << ((shift + 1) * 2)) - 1)) >> 2);
                                // // std::cout << "log: line" << __LINE__ << std::endl;
                                // ((board.board.at(board_y).at(index - 1) & 0b11) << (bitboard_value_type_size - 2));

                                // std::cout << "log: line" << __LINE__ << std::endl;

                                if(index != target_index) {
                                    /*
                                    [a  b] [c  d,  e  f  g  h,  i] [j] [k  l]
                                    -> j move
                                    [a  b] [j] [c,  d  e  f  g,  h  i] [k  l]
                                    */
                                    board.board.at(board_y).at(index) = 
                                        (board.board.at(board_y).at(index) & ((0b1 << (shift * 2)) - 1))
                                        | ((board.board.at(board_y).at(index) & ~((0b1 << ((shift + 1) * 2)) - 1)) >> 2)// shiftに値で分岐が必要////////////////////////////
                                        | ((board.board.at(board_y).at(index - 1) & 0b11) << (bitboard_value_type_size - 2));
                                    // std::cout << "log: line" << __LINE__ << std::endl;
                                    for(int k = index - 1; k > target_index; k--) {
                                        board.board.at(board_y).at(k) = 
                                            (board.board.at(board_y).at(k) >> 2)
                                            | ((board.board.at(board_y).at(k - 1) & 0b11) << (bitboard_value_type_size - 2));
                                    }
                                    // std::cout << "log: line" << __LINE__ << std::endl;
                                    if((target_shift + 1) * 2 < bitboard_value_type_size) {
                                        board.board.at(board_y).at(target_index) = 
                                            (board.board.at(board_y).at(target_index) & ~((0b1 << ((target_shift + 1) * 2)) - 1))
                                            | ((board.board.at(board_y).at(target_index) >> 2) & ((0b1 << (target_shift * 2)) - 1))
                                            | (move_value << (target_shift * 2));
                                    } else {
                                        board.board.at(board_y).at(target_index) = 
                                            ((board.board.at(board_y).at(target_index) >> 2) & ((0b1 << (target_shift * 2)) - 1))// &以降は不要っぽい？
                                            | (move_value << (target_shift * 2));
                                    }
                                } else {
                                    /*
                                    [a  b] [c  d  e] [f] [g  h]
                                    -> f move
                                    [a  b] [f] [c  d  e] [g  h]
                                    */
                                    board.board.at(board_y).at(index) = 
                                        (board.board.at(board_y).at(index) & ((0b1 << (shift * 2)) - 1))
                                        | (board.board.at(board_y).at(index) & ~((0b1 << ((target_shift + 1) * 2)) - 1))
                                        | ((board.board.at(board_y).at(index) >> 2) & (~((0b1 << (shift * 2)) - 1) & ((0b1 << (target_shift * 2)) - 1)))
                                        | (move_value << (target_shift * 2));
                                }
                                // TODO: 32ビット変数で32ビット左シフトしてるからバグってる
                                // ~0をしたら全部1になる？ -> なる
                                // UINT32_MAXを使う？

                                shifted_piece_count++;
                                // std::cout << "move end" << std::endl;
                                std::cout << "after:" << std::endl;
                                print(board);
                            }
                        }
                    }
                }
            } else if(op.s == RIGHT) {
                // std::cout << "RIGHT" << std::endl;
                for(int i = 0; i < pattern.height; i++) {
                    board_y = op.y + i;
                    if(board_y < 0 || board_y >= board.height) {
                        continue;
                    }
                    shifted_piece_count = 0;
                    for(int j = pattern.width - 1; j >= 0; j--) {
                        board_x = op.x + j;
                        if(0 <= board_x && board_x < board.width) {
                            if(pattern.cells.at(i).at(j) == 1) {
                                // std::cout << "move" << std::endl;
                                index = board_x / (bitboard_value_type_size / 2);
                                shift = ((bitboard_value_type_size / 2) - 1) - (board_x % (bitboard_value_type_size / 2));
                                move_value = (board.board.at(board_y).at(index) >> (shift * 2)) & 0b11;
                                
                                target_index = (board.width - 1 - shifted_piece_count) / (bitboard_value_type_size / 2);
                                target_shift = ((bitboard_value_type_size / 2) - 1) - ((board.width - 1 - shifted_piece_count) % (bitboard_value_type_size / 2));

                                // std::cout << "index: " << index << std::endl;
                                // std::cout << "target_index: " << target_index << std::endl;

                                // std::cout << "log: line" << __LINE__ << std::endl;
                                if(index != target_index) {
                                    board.board.at(board_y).at(index) = 
                                        ((board.board.at(board_y).at(index) & ((0b1 << (shift * 2)) - 1)) << 2)
                                        | (board.board.at(board_y).at(index) & ~((0b1 << ((shift + 1) * 2)) - 1))
                                        | ((board.board.at(board_y).at(index + 1) >> (bitboard_value_type_size - 2)) & 0b11);
                                    // std::cout << "log: line" << __LINE__ << std::endl;
                                    for(int k = index + 1; k < target_index; k++) {
                                        board.board.at(board_y).at(k) = 
                                            (board.board.at(board_y).at(k) << 2)
                                            | ((board.board.at(board_y).at(k + 1) >> (bitboard_value_type_size - 2)) & 0b11);
                                    }
                                }
                                // std::cout << "log: line" << __LINE__ << std::endl;
                                board.board.at(board_y).at(target_index) = 
                                    (board.board.at(board_y).at(target_index) & ((0b1 << (target_shift * 2)) - 1))
                                    | ((board.board.at(board_y).at(target_index) << 2) & ~((0b1 << ((target_shift + 1) * 2)) - 1))
                                    | (move_value << (target_shift * 2));

                                shifted_piece_count++;
                                // std::cout << "move end" << std::endl;
                            }
                        }
                    }
                }
            }

            // std::cout << "end operateBitBoard" << std::endl;

            return board;
        }

        // operate()の逆操作
        // 端のマスから，patternとshift方向に従って指定した座標に値を持ってくる
        // 指定する座標はpatternの左上隅の座標
        /*
        board:
            x 0 1 2 x
            x 3 0 x x
            x x x x x
            x x x x x
            x x x x x
        pattern:
            0 1 0
            1 0 1
            1 1 0
        のとき，x=1, y=2, shift=UPの操作後は
        board:
            x x x x x
            x x x x x
            x x 1 x x
            x 0 x 2 x
            x 3 0 x x
        になる
        */
        Board Game::inverseOperate(Board board, struct Operation op, struct Pattern pattern) {
            int board_x, board_y;

            if(op.s == UP) {
                for(int j = 0; j < pattern.width; j++) {
                    board_x = op.x + j;
                    if(board_x < 0 || board_x >= board.width) {
                        continue;
                    }
                    int unshifted_piece_count = 0;
                    for(int i = 0; i < pattern.height; i++) {
                        board_y = op.y + i;
                        if(0 <= board_y && board_y < board.height) {
                            if(pattern.cells.at(i).at(j) == 1) {
                                unshifted_piece_count++;
                            }
                        }
                    }
                    for(int i = pattern.height - 1; i >= 0; i--) {
                        board_y = op.y + i;
                        if(0 <= board_y && board_y < board.height) {
                            if(pattern.cells.at(i).at(j) == 1) {
                                for(int k = unshifted_piece_count - 1; k < board_y; k++) {
                                    swap(&board, board_x, k, board_x, k + 1);
                                }
                                unshifted_piece_count--;
                            }
                        }
                    }
                }
            } else if(op.s == DOWN) {
                for(int j = 0; j < pattern.width; j++) {
                    board_x = op.x + j;
                    if(board_x < 0 || board_x >= board.width) {
                        continue;
                    }
                    int unshifted_piece_count = 0;
                    for(int i = 0; i < pattern.height; i++) {
                        board_y = op.y + i;
                        if(0 <= board_y && board_y < board.height) {
                            if(pattern.cells.at(i).at(j) == 1) {
                                unshifted_piece_count++;
                            }
                        }
                    }
                    for(int i = 0; i < pattern.height; i++) {
                        board_y = op.y + i;
                        if(0 <= board_y && board_y < board.height) {
                            if(pattern.cells.at(i).at(j) == 1) {
                                for(int k = board.height - 1 - (unshifted_piece_count - 1); k > board_y; k--) {
                                    swap(&board, board_x, k, board_x, k - 1);
                                }
                                unshifted_piece_count--;
                            }
                        }
                    }
                }
            } else if(op.s == LEFT) {
                for(int i = 0; i < pattern.height; i++) {
                    board_y = op.y + i;
                    if(board_y < 0 || board_y >= board.height) {
                        continue;
                    }
                    int unshifted_piece_count = 0;
                    for(int j = 0; j < pattern.width; j++) {
                        board_x = op.x + j;
                        if(0 <= board_x && board_x < board.width) {
                            if(pattern.cells.at(i).at(j) == 1) {
                                unshifted_piece_count++;
                            }
                        }
                    }
                    for(int j = pattern.width - 1; j >= 0; j--) {
                        board_x = op.x + j;
                        if(0 <= board_x && board_x < board.width) {
                            if(pattern.cells.at(i).at(j) == 1) {
                                for(int k = unshifted_piece_count - 1; k < board_x; k++) {
                                    swap(&board, k, board_y, k + 1, board_y);
                                }
                                unshifted_piece_count--;
                            }
                        }
                    }
                }
            } else if(op.s == RIGHT) {
                for(int i = 0; i < pattern.height; i++) {
                    board_y = op.y + i;
                    if(board_y < 0 || board_y >= board.height) {
                        continue;
                    }
                    int unshifted_piece_count = 0;
                    for(int j = 0; j < pattern.width; j++) {
                        board_x = op.x + j;
                        if(0 <= board_x && board_x < board.width) {
                            if(pattern.cells.at(i).at(j) == 1) {
                                unshifted_piece_count++;
                            }
                        }
                    }
                    for(int j = 0; j < pattern.width; j++) {
                        board_x = op.x + j;
                        if(0 <= board_x && board_x < board.width) {
                            if(pattern.cells.at(i).at(j) == 1) {
                                for(int k = board.width - 1 - (unshifted_piece_count - 1); k > board_x; k--) {
                                    swap(&board, k, board_y, k - 1, board_y);
                                }
                                unshifted_piece_count--;
                            }
                        }
                    }
                }
            }

            return board;
        }

        vector<struct Operation> Game::getAvailableOperations(Board board, vector<struct Pattern> patterns) {
            vector<struct Operation> operations;

            for(int p = 0; p < patterns.size(); p++) {
                // TODO: ここで数を絞らないと，探索ノードの数が多くなりすぎるから制限している
                // 改良して制限を無くすべき
                if(9 < p && p < 25) {
                    continue;
                }
                
                for(int i = -patterns.at(p).height + 1; i < board.height + patterns.at(p).height - 1; i++) {
                    for(int j = -patterns.at(p).width + 1; j < board.width + patterns.at(p).width - 1; j++) {
                        // shift direction
                        for(int s = 0; s < 4; s++) {
                            struct Operation op(p, j, i, s);
                            operations.push_back(op);
                        }
                    }
                }
            }

            return operations;
        }

        vector<struct Pattern> Game::getStandardPatterns() {
            vector<struct Pattern> patterns;
            vector<vector<int> > cells;

            patterns.push_back(Pattern(0, 1, 1, vector<vector<int> >{{1}}));

            int p = 1;
            for(int size = 2; size <= 256; size *= 2) {
                // type 1: すべてのセルが1
                cells.clear();
                for(int i = 0; i < size; i++) {
                    vector<int> row;
                    for(int j = 0; j < size; j++) {
                        row.push_back(1);
                    }
                    cells.push_back(row);
                }
                patterns.push_back(Pattern(p, size, size, cells));
                p++;

                // type 2: 偶数行のセルが1，奇数行のセルが0
                cells.clear();
                for(int i = 0; i < size; i++) {
                    vector<int> row;
                    if(i % 2 == 0) {
                        for(int j = 0; j < size; j++) {
                            row.push_back(1);
                        }
                    } else {
                        for(int j = 0; j < size; j++) {
                            row.push_back(0);
                        }
                    }
                    cells.push_back(row);
                }
                patterns.push_back(Pattern(p, size, size, cells));
                p++;

                // type 3: 偶数列のセルが1，奇数列のセルが0
                cells.clear();
                for(int i = 0; i < size; i++) {
                    vector<int> row;
                    for(int j = 0; j < size; j++) {
                        if(j % 2 == 0) {
                            row.push_back(1);
                        } else {
                            row.push_back(0);
                        }
                    }
                    cells.push_back(row);
                }
                patterns.push_back(Pattern(p, size, size, cells));
                p++;
            }

            return patterns;
        }
        
        inline void Game::swap(Board* board, int x1, int y1, int x2, int y2) {
            int tmp = board->getValue(x1, y1);
            board->setValue(x1, y1, board->getValue(x2, y2));
            board->setValue(x2, y2, tmp);
        }
    }
}
