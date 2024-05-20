// 問題データを生成

#include <iostream>
#include <fstream>
#include <vector>
#include <random>

#include <nlohmann/json.hpp>

#include "lib/game.hpp"
#include "lib/solver.hpp"

using std::vector;

// メルセンヌツイスタ
std::mt19937 mt;

int get_rand_range(int min, int max) {
    std::uniform_int_distribution<int> dist(min, max);
    return dist(mt);
}

procon35::game::Board generateRandomBoard(int width, int height) {
    procon35::game::Board board;
    board.width = width;
    board.height = height;
    board.board.clear();
    for(int i = 0; i < height; i++) {
        board.appendLine();
        for(int j = 0; j < width; j++) {
            board.appendValue(get_rand_range(0, 3));
        }
    }
    return board;
}

vector<std::string> packVectorToString(vector<vector<int> > vec, int width, int height) {
    vector<std::string> vec_str;
    std::string s;
    for(int i = 0; i < height; i++) {
        s = "";
        for(int j = 0; j < width; j++) {
            s += std::to_string(vec.at(i).at(j));
        }
        vec_str.push_back(s);
    }
    return vec_str;
}

int main(int argc, char **argv) {
    procon35::game::Game game;
    procon35::game::Board board;
    vector<struct procon35::game::Pattern> patterns;
    std::filesystem::path path = "json/problem.json";
    std::ofstream ofs(path);
    // 縦横の大きさはそれぞれ32から256
    int board_width, board_height;
    int operation_num;

    std::cout << "please input problem configuration." << std::endl;
    std::cout << "width: ";
    std::cin >> board_width;
    std::cout << "height: ";
    std::cin >> board_height;
    std::cout << "operation_num: ";
    std::cin >> operation_num;
    char c;
    int seed;
    std::cout << "use random seed (y/n): ";
    std::cin >> c;
    if(c == 'y') {
        mt.seed(std::chrono::system_clock::now().time_since_epoch().count());
    } else if(c == 'n') {
        std::cout << "seed: ";
        std::cin >> seed;
        mt.seed(seed);
    }

    std::cout << "generating problem..." << std::endl;

    patterns = game.getStandardPatterns();

    // start_boardからgoal_boardまでに何手必要か，というデータセットを生成
    board = generateRandomBoard(board_width, board_height);
    procon35::game::Board start_board = board;
    for(int j = 0; j < operation_num; j++) {
        vector<struct procon35::game::Operation> operations = game.getAvailableOperations(board, patterns);
        struct procon35::game::Operation op = operations.at(get_rand_range(0, operations.size() - 1));
        board = game.operate(board, op, patterns.at(op.p));
    }
    procon35::game::Board goal_board = board;

    // jsonファイルに出力
    nlohmann::json problem;
    problem["board"]["width"] = board_width;
    problem["board"]["height"] = board_height;
    vector<vector<int> > start_board_vector(board.height, vector<int>(board.width));
    vector<vector<int> > goal_board_vector(board.height, vector<int>(board.width));
    for(int i = 0; i < board.height; i++) {
        for(int j = 0; j < board.width; j++) {
            start_board_vector.at(i).at(j) = start_board.getValue(j, i);
            goal_board_vector.at(i).at(j) = goal_board.getValue(j, i);
        }
    }
    problem["board"]["start"] = packVectorToString(start_board_vector, board.width, board.height);
    problem["board"]["goal"] = packVectorToString(goal_board_vector, board.width, board.height);
    // TODO: setting patterns
    problem["general"]["n"] = 0;
    problem["general"]["patterns"] = {};

    ofs << problem.dump();
    ofs.close();

    std::cout << "problem generated." << std::endl;

    return 0;
}
