#include <iostream>

#include "game.hpp"
#include "solver.hpp"

int main(int argc, char **argv){
    procon35::game::Game game;
    procon35::game::Problem problem;
    procon35::game::Answer answer;
    procon35::solver::Solver_AStar solver_astar;

    problem = game.loadProblem("./json/problem.json");

    std::cout << "log: problem loaded." << std::endl;

    answer = solver_astar.solve(game, problem);

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

    game.writeAnswer(answer, "./json/answer.json");

    return 0;
}
