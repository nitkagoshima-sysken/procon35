#pragma once

#include <vector>
#include <filesystem>

using std::vector;

namespace procon35 {
    namespace game {
        enum Direction {
            UP = 0,
            DOWN,
            LEFT,
            RIGHT
        };

        struct Board {
            int width;
            int height;
            vector<vector<int> > board;
            Board();
            Board(int width, int height, vector<vector<int> > board);
            Board(const struct Board& other);
            struct Board& operator=(const struct Board& other);
            bool operator==(const struct Board& other);
        };

        struct Pattern {
            int p; // id of the pattern
            int width;
            int height;
            vector<vector<int> > cells;
            Pattern();
            Pattern(int p, int width, int height, vector<vector<int> > cells);
            Pattern(const struct Pattern& other);
            struct Pattern& operator=(const struct Pattern& other);
        };

        struct Operation {
            int p; // id of the pattern
            int x;
            int y;
            int s; // shift direction
            Operation();
            Operation(int p, int x, int y, int s);
            Operation(const struct Operation& other);
            struct Operation& operator=(const struct Operation& other);
        };

        class Problem {
        public:
            struct Board start_board;
            struct Board goal_board;
            vector<struct Pattern> patterns;
            Problem();
            ~Problem();
            Problem(const Problem& other);
            Problem& operator=(const Problem& other);
        };

        class Answer {
        public:
            bool solved;
            vector<struct Operation> operations;
            Answer();
            ~Answer();
            Answer(const Answer& other);
            Answer& operator=(const Answer& other);
        };

        class Game {
        public:
            Game();
            ~Game();
            Problem loadProblem(std::filesystem::path path);
            void writeAnswer(Answer answer, std::filesystem::path path);
            struct Board operate(struct Board board, struct Operation op, struct Pattern pattern);
            vector<struct Operation> getAvailableOperations(struct Board board, vector<struct Pattern> patterns);

        private:
            // 定型抜き型を返す
            vector<struct Pattern> getStandardPatterns();
            void swap(struct Board* board, int x1, int y1, int x2, int y2);
        };
    }
}
