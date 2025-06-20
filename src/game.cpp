#include <vector>
#include <filesystem>
#include <fstream>

#include <nlohmann/json.hpp>

#include "game.hpp"

using std::vector;
using json = nlohmann::json;

namespace procon35 {
    namespace game {
        Board::Board() = default;
        Board::Board(int width, int height, vector<vector<int> > board) : width(width), height(height), board(board) {}
        Board::Board(const struct Board& other) {
            width = other.width;
            height = other.height;
            board = other.board;
        }
        struct Board& Board::operator=(const struct Board& other) {
            width = other.width;
            height = other.height;
            board = other.board;
            return *this;
        }
        bool Board::operator==(const struct Board& other) {
            if(width != other.width) return false;
            if(height != other.height) return false;
            for(int i = 0; i < height; i++) {
                for(int j = 0; j < width; j++) {
                    if(board.at(i).at(j) != other.board.at(i).at(j)) return false;
                }
            }
            return true;
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
                problem.start_board.board.push_back(vector<int>());
                // change row string to int; "220103" -> [2,2,0,1,0,3]
                for(auto& x: row) {
                    problem.start_board.board.back().push_back((int)x - '0');
                }
            }
            problem.goal_board.width = data["board"]["width"];
            problem.goal_board.height = data["board"]["height"];
            problem.goal_board.board.clear();
            for(auto& _row: data["board"]["goal"]) {
                std::string row = _row;
                problem.goal_board.board.push_back(vector<int>());
                for(auto& x: row) {
                    problem.goal_board.board.back().push_back((int)x - '0');
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

        struct Board Game::operate(struct Board board, struct Operation op, struct Pattern pattern) {
            int board_x, board_y;

            // 型抜きの対象ピースの数値xは(x-10)に置き換える
            for(int i = 0; i < pattern.height; i++) {
                for(int j = 0; j < pattern.width; j++) {
                    if(pattern.cells.at(i).at(j) == 1) {
                        board_x = op.x + j;
                        board_y = op.y + i;
                        if((0 <= board_y && board_y < board.board.size()) && (0 <= board_x && board_x < board.board.at(0).size())) {
                            board.board.at(board_y).at(board_x) -= 10;
                        }
                    }
                }
            }

            // 値が負のピースを寄せる
            if(op.s == UP) {
                for(int j = 0; j < pattern.width; j++) {
                    int shifted_piece_count = 0;
                    for(int i = 0; i < pattern.height; i++) {
                        board_x = op.x + j;
                        board_y = op.y + i;
                        if((0 <= board_y && board_y < board.board.size()) && (0 <= board_x && board_x < board.board.at(0).size())) {
                            if(board.board.at(board_y).at(board_x) < 0) {
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
                    int shifted_piece_count = 0;
                    for(int i = pattern.height - 1; i >= 0; i--) {
                        board_x = op.x + j;
                        board_y = op.y + i;
                        if((0 <= board_y && board_y < board.board.size()) && (0 <= board_x && board_x < board.board.at(0).size())) {
                            if(board.board.at(board_y).at(board_x) < 0) {
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
                    int shifted_piece_count = 0;
                    for(int j = 0; j < pattern.width; j++) {
                        board_x = op.x + j;
                        board_y = op.y + i;
                        if((0 <= board_y && board_y < board.board.size()) && (0 <= board_x && board_x < board.board.at(0).size())) {
                            if(board.board.at(board_y).at(board_x) < 0) {
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
                    int shifted_piece_count = 0;
                    for(int j = pattern.width - 1; j >= 0; j--) {
                        board_x = op.x + j;
                        board_y = op.y + i;
                        if((0 <= board_y && board_y < board.board.size()) && (0 <= board_x && board_x < board.board.at(0).size())) {
                            if(board.board.at(board_y).at(board_x) < 0) {
                                for(int k = board_x; k < board.width - 1 - shifted_piece_count; k++) {
                                    swap(&board, k, board_y, k + 1, board_y);
                                }
                                shifted_piece_count++;
                            }
                        }
                    }
                }
            }

            // 型抜きの対象ピースの数値(x-10)はxに置き換える
            for(int i = 0; i < board.height; i++) {
                for(int j = 0; j < board.width; j++) {
                    if(board.board.at(i).at(j) < 0) {
                        board.board.at(i).at(j) += 10;
                    }
                }
            }

            return board;
        }

        vector<struct Operation> Game::getAvailableOperations(struct Board board, vector<struct Pattern> patterns) {
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
        
        void Game::swap(struct Board* board, int x1, int y1, int x2, int y2) {
            int tmp = board->board.at(y1).at(x1);
            board->board.at(y1).at(x1) = board->board.at(y2).at(x2);
            board->board.at(y2).at(x2) = tmp;
        }
    }
}
