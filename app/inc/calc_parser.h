#ifndef CALC_PARSER_H
#define CALC_PARSER_H

#include <string>

/*
 * 函数作用：计算器对外主接口，对输入表达式进行词法分析、解析和求值
 * 入参：input - 用户输入的表达式字符串
 * 出参：result - 计算结果；errorMsg - 错误信息
 * 返回值：计算成功返回true，失败返回false
 */
bool CALC_Evaluate(const std::string& input, double& result, std::string& errorMsg);

#endif
