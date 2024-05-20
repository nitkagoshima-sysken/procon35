#include <iostream>

#include "lib/game.hpp"
#include "lib/solver.hpp"

void print_answer(procon35::game::Game game, procon35::game::Problem problem, procon35::game::Answer answer) {
    std::cout << "log: answer operations:" << std::endl;
    procon35::game::Board board = problem.start_board;
    for(int i = 0; i < board.height; i++) {
        for(int j = 0; j < board.width; j++) {
            std::cout << board.getValue(j, i) << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
    for(auto& op: answer.operations) {
        std::cout  << "{p:"<< op.p << ", x:" << op.x << ", y:" << op.y << ", s:" << op.s << "}" << std::endl; 
        board = game.operate(board, op, problem.patterns.at(op.p));
        for(int i = 0; i < board.height; i++) {
            for(int j = 0; j < board.width; j++) {
                std::cout << board.getValue(j, i) << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
}

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

    // 回答を表示する
    print_answer(game, problem, answer);

    game.writeAnswer(answer, "./json/answer.json");

    return 0;
}
