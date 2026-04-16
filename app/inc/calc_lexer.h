#ifndef CALC_LEXER_H
#define CALC_LEXER_H

#include <string>
#include <vector>
#include "calc_token.h"

/*
 * 函数作用：将输入字符串拆分为Token序列
 * 入参：input - 用户输入的表达式字符串
 * 出参：tokens - 解析得到的Token序列；errorMsg - 错误信息
 * 返回值：解析成功返回true，失败返回false
 */
bool CalcTokenize(const std::string& input, std::vector<Token>& tokens, std::string& errorMsg);

#endif
