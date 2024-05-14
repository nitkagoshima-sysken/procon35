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

        class ArrayBoard {
        public:
            int width;
            int height;
            vector<vector<int> > board;
            ArrayBoard();
            ArrayBoard(int width, int height, vector<vector<int> > board);
            ArrayBoard(const ArrayBoard& other);
            ArrayBoard& operator=(const ArrayBoard& other);
            bool operator==(ArrayBoard& other);
            int getValue(int x, int y);
            void setValue(int x, int y, int value);
            void appendLine();
            void appendValue(int value);
        };

        // 1マスの値を2bitで表現する
        class BitBoard {
        public:
            int width;
            int height;
            vector<vector<char> > board;
            BitBoard();
            BitBoard(int width, int height, vector<vector<char> > board);
            BitBoard(const BitBoard& other);
            BitBoard& operator=(const BitBoard& other);
            bool operator==(BitBoard& other);
            int getValue(int x, int y);
            void setValue(int x, int y, int value);
            void appendLine();
            void appendValue(int value);
        private:
            char width_end;
        };

        // define board type
        using Board = BitBoard;

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
            Board start_board;
            Board goal_board;
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
            Board operate(Board board, struct Operation op, struct Pattern pattern);
            Board inverseOperate(Board board, struct Operation op, struct Pattern pattern);
            vector<struct Operation> getAvailableOperations(Board board, vector<struct Pattern> patterns);
            // 定型抜き型を返す
            vector<struct Pattern> getStandardPatterns();

        private:
            void swap(Board* board, int x1, int y1, int x2, int y2);
        };
    }
}
