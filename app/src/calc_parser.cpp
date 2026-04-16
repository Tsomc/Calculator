#include "calc_parser.h"
#include "calc_lexer.h"
#include "calc_token.h"
#include <stack>
#include <queue>
#include <vector>

const int PRECEDENCE_NEG = 3;
const int PRECEDENCE_MUL_DIV = 2;
const int PRECEDENCE_ADD_SUB = 1;
const double DIVISOR_ZERO = 0.0;

/*
 * 函数作用：获取运算符的优先级
 * 入参：type - Token类型
 * 出参：无
 * 返回值：优先级数值，越大优先级越高
 */
static int CalcGetPrecedence(TokenType type) {
    switch (type) {
    case TOKEN_TYPE_OPERATOR_NEG:
        return PRECEDENCE_NEG;
    case TOKEN_TYPE_OPERATOR_MUL:
    case TOKEN_TYPE_OPERATOR_DIV:
        return PRECEDENCE_MUL_DIV;
    case TOKEN_TYPE_OPERATOR_ADD:
    case TOKEN_TYPE_OPERATOR_SUB:
        return PRECEDENCE_ADD_SUB;
    default:
        return 0;
    }
}

/*
 * 函数作用：判断运算符是否为右结合
 * 入参：type - Token类型
 * 出参：无
 * 返回值：是右结合返回true，否则返回false
 */
static bool CalcIsRightAssociative(TokenType type) {
    return type == TOKEN_TYPE_OPERATOR_NEG;
}

/*
 * 函数作用：判断Token类型是否为左括号
 * 入参：type - Token类型
 * 出参：无
 * 返回值：是左括号返回true，否则返回false
 */
static bool CalcIsBracketLeft(TokenType type) {
    return type == TOKEN_TYPE_BRACKET_LEFT_SMALL ||
           type == TOKEN_TYPE_BRACKET_LEFT_MEDIUM ||
           type == TOKEN_TYPE_BRACKET_LEFT_LARGE;
}

/*
 * 函数作用：判断Token类型是否为右括号
 * 入参：type - Token类型
 * 出参：无
 * 返回值：是右括号返回true，否则返回false
 */
static bool CalcIsBracketRight(TokenType type) {
    return type == TOKEN_TYPE_BRACKET_RIGHT_SMALL ||
           type == TOKEN_TYPE_BRACKET_RIGHT_MEDIUM ||
           type == TOKEN_TYPE_BRACKET_RIGHT_LARGE;
}

/*
 * 函数作用：获取与右括号匹配的左括号类型
 * 入参：rightBracket - 右括号Token类型
 * 出参：无
 * 返回值：对应的左括号Token类型
 */
static TokenType CalcGetMatchingLeftBracket(TokenType rightBracket) {
    switch (rightBracket) {
    case TOKEN_TYPE_BRACKET_RIGHT_SMALL:
        return TOKEN_TYPE_BRACKET_LEFT_SMALL;
    case TOKEN_TYPE_BRACKET_RIGHT_MEDIUM:
        return TOKEN_TYPE_BRACKET_LEFT_MEDIUM;
    case TOKEN_TYPE_BRACKET_RIGHT_LARGE:
        return TOKEN_TYPE_BRACKET_LEFT_LARGE;
    default:
        return TOKEN_TYPE_NUMBER;
    }
}

/*
 * 函数作用：在调度场算法中处理右括号，弹出运算符栈直到匹配的左括号
 * 入参：rightBracket - 右括号Token；opStack - 运算符栈；output - 输出队列
 * 出参：errorMsg - 错误信息
 * 返回值：处理成功返回true，括号不匹配返回false
 */
static bool CalcHandleRightBracket(const Token& rightBracket, std::stack<Token>& opStack, std::queue<Token>& output,
    std::string& errorMsg) {
    TokenType matchingLeft = CalcGetMatchingLeftBracket(rightBracket.type);
    bool foundMatch = false;

    while (!opStack.empty()) {
        Token top = opStack.top();
        opStack.pop();
        if (CalcIsBracketLeft(top.type)) {
            if (top.type == matchingLeft) {
                foundMatch = true;
            } else {  // 左括号类型与右括号不对应，如(]
                errorMsg = "异常：括号不匹配";
                return false;
            }
            break;
        } else {
            output.push(top);
        }
    }

    if (!foundMatch) {  // 栈已空仍未找到匹配的左括号
        errorMsg = "异常：括号不匹配";
        return false;
    }

    return true;
}

/*
 * 函数作用：在调度场算法中处理运算符，根据优先级弹出栈顶运算符
 * 入参：token - 当前运算符Token；opStack - 运算符栈；output - 输出队列
 * 出参：无
 * 返回值：无
 */
static void CalcHandleOperator(const Token& token, std::stack<Token>& opStack, std::queue<Token>& output) {
    while (!opStack.empty()) {
        Token top = opStack.top();
        if (CalcIsBracketLeft(top.type)) {
            break;
        }
        int topPrec = CalcGetPrecedence(top.type);
        int curPrec = CalcGetPrecedence(token.type);
        if (CalcIsRightAssociative(token.type)) {
            if (topPrec > curPrec) {  // 右结合运算符仅弹出严格高优先级的，同优先级不弹出以保持右结合性
                output.push(top);
                opStack.pop();
            } else {
                break;
            }
        } else {
            if (topPrec >= curPrec) {  // 左结合运算符弹出大于等于优先级的，保证左结合性
                output.push(top);
                opStack.pop();
            } else {
                break;
            }
        }
    }
    opStack.push(token);
}

/*
 * 函数作用：使用调度场算法将中缀Token序列转换为后缀表达式（RPN）
 * 入参：tokens - 中缀Token序列
 * 出参：output - 转换得到的RPN队列；errorMsg - 错误信息
 * 返回值：转换成功返回true，失败返回false
 */
static bool CalcConvertToRpn(const std::vector<Token>& tokens, std::queue<Token>& output, std::string& errorMsg) {
    std::stack<Token> opStack;
    size_t tokenCount = tokens.size();

    if (tokenCount == 0) {
        errorMsg = "异常：表达式为空";
        return false;
    }

    for (size_t i = 0; i < tokenCount; i++) {
        const Token& token = tokens[i];

        if (token.type == TOKEN_TYPE_NUMBER) {
            output.push(token);
        } else if (CalcIsBracketLeft(token.type)) {
            opStack.push(token);
        } else if (CalcIsBracketRight(token.type)) {
            if (!CalcHandleRightBracket(token, opStack, output, errorMsg)) {
                return false;
            }
        } else {
            CalcHandleOperator(token, opStack, output);
        }
    }

    while (!opStack.empty()) {
        Token top = opStack.top();
        opStack.pop();
        if (CalcIsBracketLeft(top.type)) {  // 遍历结束后栈中仍残留左括号，说明缺少右括号
            errorMsg = "异常：括号不匹配";
            return false;
        }
        output.push(top);
    }

    return true;
}

/*
 * 函数作用：对两个操作数应用二元运算符
 * 入参：left - 左操作数；right - 右操作数；opType - 运算符类型
 * 出参：value - 运算结果；errorMsg - 错误信息
 * 返回值：运算成功返回true，除数为零返回false
 */
static bool CalcApplyBinaryOp(double left, double right, TokenType opType, double& value, std::string& errorMsg) {
    switch (opType) {
    case TOKEN_TYPE_OPERATOR_ADD:
        value = left + right;
        break;
    case TOKEN_TYPE_OPERATOR_SUB:
        value = left - right;
        break;
    case TOKEN_TYPE_OPERATOR_MUL:
        value = left * right;
        break;
    case TOKEN_TYPE_OPERATOR_DIV:
        if (right == DIVISOR_ZERO) {
            errorMsg = "异常：除数不能为零";
            return false;
        }
        value = left / right;
        break;
    default:
        errorMsg = "异常：未知的运算符";
        return false;
    }
    return true;
}

/*
 * 函数作用：对后缀表达式进行求值
 * 入参：rpn - 后缀表达式队列
 * 出参：result - 求值结果；errorMsg - 错误信息
 * 返回值：求值成功返回true，失败返回false
 */
static bool CalcEvaluateRpn(std::queue<Token>& rpn, double& result, std::string& errorMsg) {
    std::stack<double> evalStack;

    while (!rpn.empty()) {
        Token token = rpn.front();
        rpn.pop();

        if (token.type == TOKEN_TYPE_NUMBER) {
            evalStack.push(token.value);
        } else if (token.type == TOKEN_TYPE_OPERATOR_NEG) {
            if (evalStack.empty()) {
                errorMsg = "异常：表达式不完整";
                return false;
            }
            double operand = evalStack.top();
            evalStack.pop();
            evalStack.push(-operand);
        } else {
            if (evalStack.size() < 2) {  // 二元运算符需要两个操作数
                errorMsg = "异常：表达式不完整";
                return false;
            }
            double right = evalStack.top();
            evalStack.pop();
            double left = evalStack.top();
            evalStack.pop();

            double value = 0;
            if (!CalcApplyBinaryOp(left, right, token.type, value, errorMsg)) {
                return false;
            }
            evalStack.push(value);
        }
    }

    if (evalStack.size() != 1) {  // 合法表达式求值后栈中应仅剩一个结果
        errorMsg = "异常：表达式不完整";
        return false;
    }

    result = evalStack.top();
    return true;
}

/*
 * 函数作用：计算器对外主接口，对输入表达式进行词法分析、解析和求值
 * 入参：input - 用户输入的表达式字符串
 * 出参：result - 计算结果；errorMsg - 错误信息
 * 返回值：计算成功返回true，失败返回false
 */
bool CALC_Evaluate(const std::string& input, double& result, std::string& errorMsg) {
    if (input.empty()) {
        errorMsg = "异常：输入为空";
        return false;
    }

    std::vector<Token> tokens;
    if (!CalcTokenize(input, tokens, errorMsg)) {
        return false;
    }

    if (tokens.empty()) {  // 输入非空但全为空白字符时tokens为空
        errorMsg = "异常：输入为空";
        return false;
    }

    std::queue<Token> rpn;
    if (!CalcConvertToRpn(tokens, rpn, errorMsg)) {
        return false;
    }

    if (!CalcEvaluateRpn(rpn, result, errorMsg)) {
        return false;
    }

    return true;
}
