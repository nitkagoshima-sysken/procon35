#include <iostream>
#include<pybind11/embed.h>

#include <nlohmann/json.hpp>

#include "game.hpp"
#include "solver.hpp"

namespace py = pybind11;

int main(int argc, char **argv){
    py::scoped_interpreter guard{};

    procon35::game::Game game;
    procon35::game::Problem problem;
    procon35::game::Answer answer;
    procon35::solver::Solver_AStar solver_aster;

    
    // Pythonモジュールをインポート
    py::module get_post_module = py::module::import("get_and_post_function");

    // Pythonのget関数を呼び出す
    std::string url = "localhost:8080";  // 適当なURL("/problem")に置き換えてください
    py::object problem_board = get_post_module.attr("get")(url);

    //problem = game.loadProblem("./json/problem.json");

    std::cout << "log: problem loaded." << std::endl;

    answer = solver_aster.solve(game, problem);

    std::cout << "log: solver finished." << std::endl;
    std::cout << "log: answer: " << answer.operations.size() << " operations." << std::endl;

    // 回答を表示する falseにすれば非表示となる
    bool print_answer = true;
    if(print_answer) {
        std::cout << "log: answer operations:" << std::endl;
        struct procon35::game::Board board = problem.start_board;
        for(int i = 0; i < board.height; i++) {
            for(int j = 0; j < board.width; j++) {
                std::cout << board.board.at(i).at(j) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        for(auto& op: answer.operations) {
            std::cout  << "{p:"<< op.p << ", x:" << op.x << ", y:" << op.y << ", s:" << op.s << "}" << std::endl; 
            board = game.operate(board, op, problem.patterns.at(op.p));
            for(int i = 0; i < board.height; i++) {
                for(int j = 0; j < board.width; j++) {
                    std::cout << board.board.at(i).at(j) << " ";
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
    }

    //game.writeAnswer(answer, "./json/answer.json");

    // Pythonのpost関数を呼び出す
    py::object answer_python = py::cast(answer);  // Jsonを文字列に変換
    get_post_module.attr("post")(url, answer_python);


    return 0;
}
