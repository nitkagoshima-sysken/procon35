// NNの学習データセットを生成

#include <iostream>
#include <fstream>
#include <vector>
#include <random>

#include <nlohmann/json.hpp>

#include "lib/game.hpp"
#include "lib/solver.hpp"

using std::vector;

int get_rand_range(int min, int max) {
    static std::mt19937 mt(0);
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

int main(int argc, char **argv) {
    procon35::game::Game game;
    procon35::game::Board board;
    vector<struct procon35::game::Pattern> patterns;
    std::filesystem::path path = "dataset/dataset.json";
    std::ofstream ofs(path);
    nlohmann::json data;
    // 生成する学習データセットのデータ数
    int dataset_size = 10000;
    // 縦横の大きさはそれぞれ32から256
    int board_width = 32, board_height = 32;
    int operation_num_min = 1, operation_num_max = 20;

    patterns = game.getStandardPatterns();

    for(int i = 0; i < dataset_size; i++) {
        // progress
        if(i % (dataset_size / 100) == 0) {
            std::cout << "progress: " << 100 * i / dataset_size << "%" << std::endl;
        }

        // start_boardからgoal_boardまでに何手必要か，というデータセットを生成
        board = generateRandomBoard(board_width, board_height);
        procon35::game::Board start_board = board;
        int operation_num = get_rand_range(operation_num_min, operation_num_max);
        for(int j = 0; j < operation_num; j++) {
            vector<struct procon35::game::Operation> operations = game.getAvailableOperations(board, patterns);
            struct procon35::game::Operation op = operations.at(get_rand_range(0, operations.size() - 1));
            board = game.operate(board, op, patterns.at(op.p));
        }
        procon35::game::Board goal_board = board;

        // jsonファイルに出力
        nlohmann::json problem;
        problem["id"] = i;
        problem["input"]["width"] = board_width;
        problem["input"]["height"] = board_height;
        vector<vector<int> > start_board_vector(board.height, vector<int>(board.width));
        vector<vector<int> > goal_board_vector(board.height, vector<int>(board.width));
        for(int i = 0; i < board.height; i++) {
            for(int j = 0; j < board.width; j++) {
                start_board_vector.at(i).at(j) = start_board.getValue(j, i);
                goal_board_vector.at(i).at(j) = goal_board.getValue(j, i);
            }
        }
        problem["input"]["start"] = start_board_vector;
        problem["input"]["goal"] = goal_board_vector;
        problem["output"]["n"] = operation_num;
        data.push_back(problem);
    }

    std::cout << "progress: done" << std::endl;

    ofs << data.dump();
    ofs.close();

    return 0;
}
