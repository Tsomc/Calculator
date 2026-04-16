#include "calc_lexer.h"
#include <cctype>

const char CHAR_SPACE = ' ';
const char CHAR_TAB = '\t';
const char CHAR_DOT = '.';
const char CHAR_PLUS = '+';
const char CHAR_MINUS = '-';
const char CHAR_ASTERISK = '*';
const char CHAR_SLASH = '/';
const char CHAR_LEFT_SMALL = '(';
const char CHAR_RIGHT_SMALL = ')';
const char CHAR_LEFT_MEDIUM = '[';
const char CHAR_RIGHT_MEDIUM = ']';
const char CHAR_LEFT_LARGE = '{';
const char CHAR_RIGHT_LARGE = '}';

const unsigned char UTF8_BOM_BYTE_0 = 0xEF;
const unsigned char UTF8_BOM_BYTE_1 = 0xBB;
const unsigned char UTF8_BOM_BYTE_2 = 0xBF;
const size_t UTF8_BOM_LENGTH = 3;

struct CharTokenEntry {
    char ch;
    TokenType type;
};

const CharTokenEntry CHAR_TOKEN_MAP[] = {
    { CHAR_ASTERISK, TOKEN_TYPE_OPERATOR_MUL },
    { CHAR_SLASH, TOKEN_TYPE_OPERATOR_DIV },
    { CHAR_LEFT_SMALL, TOKEN_TYPE_BRACKET_LEFT_SMALL },
    { CHAR_RIGHT_SMALL, TOKEN_TYPE_BRACKET_RIGHT_SMALL },
    { CHAR_LEFT_MEDIUM, TOKEN_TYPE_BRACKET_LEFT_MEDIUM },
    { CHAR_RIGHT_MEDIUM, TOKEN_TYPE_BRACKET_RIGHT_MEDIUM },
    { CHAR_LEFT_LARGE, TOKEN_TYPE_BRACKET_LEFT_LARGE },
    { CHAR_RIGHT_LARGE, TOKEN_TYPE_BRACKET_RIGHT_LARGE },
};

const size_t CHAR_TOKEN_MAP_SIZE = sizeof(CHAR_TOKEN_MAP) / sizeof(CHAR_TOKEN_MAP[0]);

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
 * 函数作用：判断Token类型是否为运算符
 * 入参：type - Token类型
 * 出参：无
 * 返回值：是运算符返回true，否则返回false
 */
static bool CalcIsOperator(TokenType type) {
    return type == TOKEN_TYPE_OPERATOR_ADD ||
           type == TOKEN_TYPE_OPERATOR_SUB ||
           type == TOKEN_TYPE_OPERATOR_MUL ||
           type == TOKEN_TYPE_OPERATOR_DIV ||
           type == TOKEN_TYPE_OPERATOR_NEG;
}

/*
 * 函数作用：跳过输入字符串开头的UTF-8 BOM标记
 * 入参：input - 输入字符串
 * 出参：无
 * 返回值：BOM后的起始位置
 */
static size_t CalcSkipBom(const std::string& input) {
    if (input.size() >= UTF8_BOM_LENGTH &&
        static_cast<unsigned char>(input[0]) == UTF8_BOM_BYTE_0 &&
        static_cast<unsigned char>(input[1]) == UTF8_BOM_BYTE_1 &&
        static_cast<unsigned char>(input[2]) == UTF8_BOM_BYTE_2) {
        return UTF8_BOM_LENGTH;
    }
    return 0;
}

/*
 * 函数作用：从输入字符串中解析一个数字Token
 * 入参：input - 输入字符串；pos - 当前解析位置（会被更新）
 * 出参：token - 解析得到的数字Token；errorMsg - 错误信息
 * 返回值：解析成功返回true，失败返回false
 */
static bool CalcParseNumber(const std::string& input, size_t& pos, Token& token, std::string& errorMsg) {
    size_t start = pos;
    size_t inputLen = input.size();
    bool hasDot = false;

    while (pos < inputLen) {
        char ch = input[pos];
        if (std::isdigit(ch)) {
            pos++;
        } else if (ch == CHAR_DOT) {
            if (hasDot) {
                errorMsg = "异常：数字格式错误，包含多个小数点";
                return false;
            }
            hasDot = true;
            pos++;
        } else {
            break;
        }
    }

    std::string numStr = input.substr(start, pos - start);
    if (numStr == std::string(1, CHAR_DOT)) {  // 仅输入了小数点而无数字
        errorMsg = "异常：数字格式错误，单独的小数点";
        return false;
    }

    token.type = TOKEN_TYPE_NUMBER;
    try {
        token.value = std::stod(numStr);
    } catch (...) {  // stod对超范围或格式异常的输入会抛出异常
        errorMsg = "异常：数字格式错误";
        return false;
    }

    return true;
}

/*
 * 函数作用：将单个字符分类为对应的Token
 * 入参：ch - 当前字符；tokens - 已解析的Token序列
 * 出参：token - 生成的Token；shouldSkip - 是否跳过该字符（如一元加号）；errorMsg - 错误信息
 * 返回值：分类成功返回true，遇到非法字符返回false
 */
static bool CalcClassifyChar(char ch, const std::vector<Token>& tokens, Token& token, bool& shouldSkip,
    std::string& errorMsg) {
    shouldSkip = false;
    token.value = 0;

    if (ch == CHAR_PLUS) {  // +需区分一元/二元，无法放入条件表
        if (tokens.empty() ||
            CalcIsBracketLeft(tokens.back().type) ||
            CalcIsOperator(tokens.back().type)) {  // 前面无操作数或紧跟左括号/运算符时为一元加号
            shouldSkip = true;  // 一元加号无运算意义，直接跳过
        } else {
            token.type = TOKEN_TYPE_OPERATOR_ADD;
        }
        return true;
    }

    if (ch == CHAR_MINUS) {  // -需区分一元/二元，无法放入条件表
        if (tokens.empty() ||
            CalcIsBracketLeft(tokens.back().type) ||
            CalcIsOperator(tokens.back().type)) {  // 同一元加号的判断逻辑，但一元减号需保留
            token.type = TOKEN_TYPE_OPERATOR_NEG;
        } else {
            token.type = TOKEN_TYPE_OPERATOR_SUB;
        }
        return true;
    }

    for (size_t i = 0; i < CHAR_TOKEN_MAP_SIZE; i++) {
        if (CHAR_TOKEN_MAP[i].ch == ch) {
            token.type = CHAR_TOKEN_MAP[i].type;
            return true;
        }
    }

    errorMsg = "异常：包含非法字符 '";
    errorMsg += ch;
    errorMsg += "'";
    return false;
}

/*
 * 函数作用：将输入字符串拆分为Token序列
 * 入参：input - 用户输入的表达式字符串
 * 出参：tokens - 解析得到的Token序列；errorMsg - 错误信息
 * 返回值：解析成功返回true，失败返回false
 */
bool CalcTokenize(const std::string& input, std::vector<Token>& tokens, std::string& errorMsg) {
    tokens.clear();
    size_t pos = CalcSkipBom(input);  // 部分编辑器会在文件开头插入BOM，需跳过
    size_t inputLen = input.size();

    while (pos < inputLen) {
        char ch = input[pos];

        if (ch == CHAR_SPACE || ch == CHAR_TAB) {
            pos++;
            continue;
        }

        if (std::isdigit(ch) || ch == CHAR_DOT) {
            Token token;
            if (!CalcParseNumber(input, pos, token, errorMsg)) {
                return false;
            }
            tokens.push_back(token);
            continue;
        }

        Token token;
        bool shouldSkip = false;
        if (!CalcClassifyChar(ch, tokens, token, shouldSkip, errorMsg)) {
            return false;
        }

        if (shouldSkip) {
            pos++;
            continue;
        }

        tokens.push_back(token);
        pos++;
    }

    return true;
}
