#include "calc_lexer.h"
#include "calc_token.h"
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>

const double EPSILON = 1e-9;

static int g_passCount = 0;
static int g_failCount = 0;

static void UtAssertBool(bool condition, const std::string& caseName, const std::string& detail) {
    if (condition) {
        g_passCount++;
    } else {
        g_failCount++;
        std::cout << "[FAIL] UT " << caseName << ": " << detail << std::endl;
    }
}

static void UtAssertTokenizeSuccess(const std::string& input, const std::string& caseName) {
    std::vector<Token> tokens;
    std::string errorMsg;
    bool ok = CalcTokenize(input, tokens, errorMsg);
    UtAssertBool(ok, caseName, "expected success but got error: " + errorMsg);
}

static void UtAssertTokenizeError(const std::string& input, const std::string& caseName) {
    std::vector<Token> tokens;
    std::string errorMsg;
    bool ok = CalcTokenize(input, tokens, errorMsg);
    UtAssertBool(!ok, caseName, "expected error but got success");
}

static void UtAssertTokenCount(const std::string& input, size_t expected, const std::string& caseName) {
    std::vector<Token> tokens;
    std::string errorMsg;
    CalcTokenize(input, tokens, errorMsg);
    UtAssertBool(tokens.size() == expected, caseName,
                 "expected " + std::to_string(expected) + " tokens but got " + std::to_string(tokens.size()));
}

static void UtAssertTokenType(const std::string& input, size_t index, TokenType expected,
                               const std::string& caseName) {
    std::vector<Token> tokens;
    std::string errorMsg;
    CalcTokenize(input, tokens, errorMsg);
    if (index >= tokens.size()) {
        g_failCount++;
        std::cout << "[FAIL] UT " << caseName << ": index " << index
                  << " out of range (size=" << tokens.size() << ")" << std::endl;
        return;
    }
    UtAssertBool(tokens[index].type == expected, caseName,
                 "token[" + std::to_string(index) + "] type mismatch");
}

static void UtAssertTokenValue(const std::string& input, size_t index, double expected,
                                const std::string& caseName) {
    std::vector<Token> tokens;
    std::string errorMsg;
    CalcTokenize(input, tokens, errorMsg);
    if (index >= tokens.size()) {
        g_failCount++;
        std::cout << "[FAIL] UT " << caseName << ": index " << index
                  << " out of range (size=" << tokens.size() << ")" << std::endl;
        return;
    }
    UtAssertBool(std::abs(tokens[index].value - expected) <= EPSILON, caseName,
                 "token[" + std::to_string(index) + "] value expected " + std::to_string(expected) +
                 " but got " + std::to_string(tokens[index].value));
}

static void TestNumberToken() {
    UtAssertTokenizeSuccess("42", "NumberBasic");
    UtAssertTokenCount("42", 1, "NumberCount");
    UtAssertTokenType("42", 0, TOKEN_TYPE_NUMBER, "NumberType");
    UtAssertTokenValue("42", 0, 42, "NumberValue");

    UtAssertTokenizeSuccess("3.14", "DecimalBasic");
    UtAssertTokenValue("3.14", 0, 3.14, "DecimalValue");

    UtAssertTokenizeSuccess("0.5", "DecimalWithLeadingZero");
    UtAssertTokenValue("0.5", 0, 0.5, "DecimalWithLeadingZeroValue");
}

static void TestOperatorToken() {
    UtAssertTokenCount("1 + 2", 3, "AddTokenCount");
    UtAssertTokenType("1 + 2", 1, TOKEN_TYPE_OPERATOR_ADD, "AddTokenType");

    UtAssertTokenType("5 - 3", 1, TOKEN_TYPE_OPERATOR_SUB, "SubTokenType");
    UtAssertTokenType("2 * 3", 1, TOKEN_TYPE_OPERATOR_MUL, "MulTokenType");
    UtAssertTokenType("6 / 2", 1, TOKEN_TYPE_OPERATOR_DIV, "DivTokenType");
}

static void TestUnaryOperator() {
    UtAssertTokenType("-5", 0, TOKEN_TYPE_OPERATOR_NEG, "UnaryNegType");
    UtAssertTokenType("-5", 1, TOKEN_TYPE_NUMBER, "UnaryNegNumberType");
    UtAssertTokenValue("-5", 1, 5, "UnaryNegNumberValue");

    UtAssertTokenCount("+5", 1, "UnaryPlusSkipped");
    UtAssertTokenType("+5", 0, TOKEN_TYPE_NUMBER, "UnaryPlusResultType");
    UtAssertTokenValue("+5", 0, 5, "UnaryPlusResultValue");

    UtAssertTokenType("(-3)", 1, TOKEN_TYPE_OPERATOR_NEG, "UnaryNegAfterBracket");
}

static void TestBracketToken() {
    UtAssertTokenType("()", 0, TOKEN_TYPE_BRACKET_LEFT_SMALL, "LeftSmallType");
    UtAssertTokenType("()", 1, TOKEN_TYPE_BRACKET_RIGHT_SMALL, "RightSmallType");
    UtAssertTokenType("[]", 0, TOKEN_TYPE_BRACKET_LEFT_MEDIUM, "LeftMediumType");
    UtAssertTokenType("[]", 1, TOKEN_TYPE_BRACKET_RIGHT_MEDIUM, "RightMediumType");
    UtAssertTokenType("{}", 0, TOKEN_TYPE_BRACKET_LEFT_LARGE, "LeftLargeType");
    UtAssertTokenType("{}", 1, TOKEN_TYPE_BRACKET_RIGHT_LARGE, "RightLargeType");
}

static void TestWhitespaceSkip() {
    UtAssertTokenCount("  1  +  2  ", 3, "WhitespaceSkipCount");
    UtAssertTokenType("  1  +  2  ", 0, TOKEN_TYPE_NUMBER, "WhitespaceSkipType");
}

static void TestInvalidInput() {
    UtAssertTokenizeError("3 @ 5", "InvalidChar");
    UtAssertTokenizeError("3.14.15", "MultipleDots");
    UtAssertTokenizeError(".", "SingleDot");
}

int main() {
    TestNumberToken();
    TestOperatorToken();
    TestUnaryOperator();
    TestBracketToken();
    TestWhitespaceSkip();
    TestInvalidInput();

    std::cout << "UT Results: " << g_passCount << " passed, "
              << g_failCount << " failed" << std::endl;

    std::cout << "Press any key to close..." << std::endl;
    std::system("pause >nul");

    return g_failCount > 0 ? 1 : 0;
}
