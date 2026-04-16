#include "calc_parser.h"
#include <iostream>
#include <string>
#include <cmath>
#include <cstdlib>

const double EPSILON = 1e-9;

static int g_passCount = 0;
static int g_failCount = 0;

static void ItAssertBool(bool condition, const std::string& caseName, const std::string& detail) {
    if (condition) {
        g_passCount++;
    } else {
        g_failCount++;
        std::cout << "[FAIL] IT " << caseName << ": " << detail << std::endl;
    }
}

static void ItAssertResult(const std::string& input, double expected, const std::string& caseName) {
    double result = 0;
    std::string errorMsg;
    bool ok = CALC_Evaluate(input, result, errorMsg);
    if (!ok) {
        g_failCount++;
        std::cout << "[FAIL] IT " << caseName << ": expected " << expected
                  << " but got error: " << errorMsg << std::endl;
    } else if (std::abs(result - expected) > EPSILON) {
        g_failCount++;
        std::cout << "[FAIL] IT " << caseName << ": expected " << expected
                  << " but got " << result << std::endl;
    } else {
        g_passCount++;
    }
}

static void ItAssertError(const std::string& input, const std::string& caseName) {
    double result = 0;
    std::string errorMsg;
    bool ok = CALC_Evaluate(input, result, errorMsg);
    ItAssertBool(!ok, caseName, "expected error but got result " + std::to_string(result));
}

static void TestBasicArithmetic() {
    ItAssertResult("1 + 2", 3, "Add");
    ItAssertResult("5 - 3", 2, "Sub");
    ItAssertResult("2 * 3", 6, "Mul");
    ItAssertResult("6 / 2", 3, "Div");
    ItAssertResult("10 / 3", 10.0 / 3.0, "DivFloat");
}

static void TestOperatorPrecedence() {
    ItAssertResult("2 + 3 * 4", 14, "MulBeforeAdd");
    ItAssertResult("2 * 3 + 4", 10, "MulBeforeAdd2");
    ItAssertResult("10 - 2 * 3", 4, "MulBeforeSub");
    ItAssertResult("8 / 2 + 3", 7, "DivBeforeAdd");
}

static void TestUnaryOperator() {
    ItAssertResult("-5 + 3", -2, "UnaryNeg");
    ItAssertResult("2 * -3", -6, "UnaryNegAfterMul");
    ItAssertResult("-(-3)", 3, "DoubleNeg");
    ItAssertResult("+5", 5, "UnaryPlus");
}

static void TestBrackets() {
    ItAssertResult("(2 + 3) * 4", 20, "SmallBracket");
    ItAssertResult("[2 + 3] * 4", 20, "MediumBracket");
    ItAssertResult("{2 + 3} * 4", 20, "LargeBracket");
    ItAssertResult("2 * (3 + [4 - {5 - 3}])", 10, "NestedBrackets");
    ItAssertResult("((1 + 2))", 3, "DoubleSmallBracket");
}

static void TestDecimal() {
    ItAssertResult("1.5 + 2.5", 4, "DecimalAdd");
    ItAssertResult("3.14 * 2", 6.28, "DecimalMul");
    ItAssertResult("0.5 - 0.3", 0.2, "DecimalSub");
}

static void TestErrorCases() {
    ItAssertError("", "EmptyInput");
    ItAssertError("3 / 0", "DivByZero");
    ItAssertError("(3 + 5", "MissingRightBracket");
    ItAssertError("3 + 5)", "MissingLeftBracket");
    ItAssertError("[3 + 5}", "BracketMismatch");
    ItAssertError("(3 + 5]", "BracketMismatch2");
    ItAssertError("3 + @", "InvalidChar");
    ItAssertError("   ", "WhitespaceOnly");
}

int main() {
    TestBasicArithmetic();
    TestOperatorPrecedence();
    TestUnaryOperator();
    TestBrackets();
    TestDecimal();
    TestErrorCases();

    std::cout << "IT Results: " << g_passCount << " passed, "
              << g_failCount << " failed" << std::endl;

    std::cout << "Press any key to close..." << std::endl;
    std::system("pause >nul");

    return g_failCount > 0 ? 1 : 0;
}
