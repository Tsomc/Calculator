#include "calc_parser.h"
#include <iostream>
#include <string>
#include <windows.h>

const char* const PROMPT = "> ";
const char* const CMD_EXIT = "exit";
const char* const CMD_QUIT = "quit";

const char* const MSG_WELCOME =
    "========================================\n"
    "       C++ 命令行计算器\n"
    "========================================\n"
    "支持运算：+  -  *  /  %\n"
    "支持括号：()  []  {}\n"
    "示例输入：2 * (3 + [4 - {5 - 1}])\n"
    "输入 exit 或 quit 退出程序\n"
    "========================================";

const char* const MSG_EXIT = "已退出计算器，再见！";

int main() {
    SetConsoleOutputCP(CP_UTF8);

    std::cout << MSG_WELCOME << std::endl;

    std::string input;

    while (true) {
        std::cout << PROMPT;
        if (!std::getline(std::cin, input)) {  // 输入流结束（如Ctrl+D/Z）时退出
            break;
        }

        if (input == CMD_EXIT || input == CMD_QUIT) {
            break;
        }

        double result = 0;
        std::string errorMsg;
        if (CALC_Evaluate(input, result, errorMsg)) {
            std::cout << result << std::endl;
        } else {
            std::cout << errorMsg << std::endl;
        }
    }

    std::cout << MSG_EXIT << std::endl;

    return 0;
}
