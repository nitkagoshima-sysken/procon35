#include "procon2024.hpp"
#include <string>
#include <memory>

void Board_information::Board_show(const std::vector<std::vector<int>>& board) 
{
    char color_clear[] = "\033[39m";

    for (auto& column : board) {
        for (auto value : column) {
            const char* output_color = nullptr;

            switch (value) {
            case 0:
                output_color = "\033[31m";
                break;
            case 1:
                output_color = "\033[32m";
                break;
            case 2:
                output_color = "\033[33m";
                break;
            case 3:
                output_color = "\033[34m";
                break;
            default:
                break;
            }

            if (output_color) std::cout << output_color;

            std::cout << value << color_clear;
        }

        std::cout << std::endl;
    }
}