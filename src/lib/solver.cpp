#include "game.hpp"
#include "solver.hpp"

namespace procon35 {
    namespace solver {
        Solver::Solver() {}
        Solver::~Solver() {}
        procon35::game::Answer Solver::solve(procon35::game::Game game, procon35::game::Problem problem) {
            return procon35::game::Answer{};
        }

        Solver_Example::Solver_Example() {}
        Solver_Example::~Solver_Example() {}
        procon35::game::Answer Solver_Example::solve(procon35::game::Game game, procon35::game::Problem problem) {
            return procon35::game::Answer{};
        }

    }
}
