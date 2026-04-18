#include "calc_display.h"
#include <QVBoxLayout>

const int EXPRESSION_FONT_SIZE = 14;
const int RESULT_FONT_SIZE = 28;
const int RESULT_FONT_SIZE_COMPACT = 18;
const int DISPLAY_MIN_HEIGHT = 80;
const int DISPLAY_MIN_HEIGHT_COMPACT = 50;
const char* RESULT_PLACEHOLDER = "0";

/*
 * Function: Construct CalcDisplay with expression and result areas
 * Input: parent - parent widget
 * Output: none
 * Return: none
 */
CalcDisplay::CalcDisplay(QWidget* parent) : QWidget(parent), isReplacingFullWidth(false), isCompact(false) {
    expressionEdit = nullptr;
    resultLabel = nullptr;
    CalcDisplayInitLayout();
    CalcDisplayInitStyle();
}

/*
 * Function: Set expression text
 * Input: text - expression string
 * Output: none
 * Return: none
 */
void CalcDisplay::CalcDisplaySetExpression(const QString& text) {
    expressionEdit->setText(text);
}

/*
 * Function: Get current expression text
 * Input: none
 * Output: none
 * Return: current expression string
 */
QString CalcDisplay::CalcDisplayGetExpression() const {
    return expressionEdit->text();
}

/*
 * Function: Set result text
 * Input: text - result string
 * Output: none
 * Return: none
 */
void CalcDisplay::CalcDisplaySetResult(const QString& text) {
    resultLabel->setText(text);
}

/*
 * Function: Get current result text
 * Input: none
 * Output: none
 * Return: current result string
 */
QString CalcDisplay::CalcDisplayGetResult() const {
    return resultLabel->text();
}

/*
 * Function: Append a character to the expression
 * Input: ch - character to append
 * Output: none
 * Return: none
 */
void CalcDisplay::CalcDisplayAppendChar(QChar ch) {
    expressionEdit->setText(expressionEdit->text() + ch);
}

/*
 * Function: Remove the last character from the expression
 * Input: none
 * Output: none
 * Return: none
 */
void CalcDisplay::CalcDisplayRemoveLastChar() {
    QString text = expressionEdit->text();
    if (!text.isEmpty()) {
        text.chop(1);
        expressionEdit->setText(text);
    }
}

/*
 * Function: Clear the expression only
 * Input: none
 * Output: none
 * Return: none
 */
void CalcDisplay::CalcDisplayClearExpression() {
    expressionEdit->clear();
}

/*
 * Function: Clear both expression and result
 * Input: none
 * Output: none
 * Return: none
 */
void CalcDisplay::CalcDisplayClearAll() {
    expressionEdit->clear();
    resultLabel->setText(RESULT_PLACEHOLDER);
}

/*
 * Function: Toggle sign of the last number in the expression
 * Input: none
 * Output: none
 * Return: none
 */
void CalcDisplay::CalcDisplayToggleSign() {
    QString text = expressionEdit->text();
    if (text.isEmpty()) {
        expressionEdit->setText("-");
        return;
    }

    int pos = text.length() - 1;
    while (pos >= 0 && (text[pos].isDigit() || text[pos] == '.')) {
        pos--;
    }

    if (pos >= 0 && text[pos] == '-' &&
        (pos == 0 || !text[pos - 1].isDigit())) {
        text.remove(pos, 1);
    } else {
        text.insert(pos + 1, "(-");
        text += ")";
    }

    expressionEdit->setText(text);
}

/*
 * Function: Toggle compact mode for display area
 * Input: compact - true to shrink result font and display height, false to restore
 * Output: none
 * Return: none
 */
void CalcDisplay::CalcDisplaySetCompact(bool compact) {
    if (isCompact == compact) {
        return;
    }
    isCompact = compact;

    int fontSize = compact ? RESULT_FONT_SIZE_COMPACT : RESULT_FONT_SIZE;
    int minHeight = compact ? DISPLAY_MIN_HEIGHT_COMPACT : DISPLAY_MIN_HEIGHT;

    resultLabel->setStyleSheet(QString(
        "QLabel {"
        "   font-size: %1px;"
        "   color: #222222;"
        "   background: transparent;"
        "   padding: 4px;"
        "}"
    ).arg(fontSize));

    setMinimumHeight(minHeight);
}

/*
 * Function: Handle expression text edited by user
 * Input: text - new expression text
 * Output: emits expressionChanged signal
 * Return: none
 */
void CalcDisplay::CalcDisplayOnExpressionEdited(const QString& text) {
    if (isReplacingFullWidth) {
        return;
    }

    QString replaced = CalcDisplayReplaceFullWidth(text);
    if (replaced != text) {
        isReplacingFullWidth = true;
        int cursorPos = expressionEdit->cursorPosition();
        int diff = replaced.length() - text.length();
        expressionEdit->setText(replaced);
        expressionEdit->setCursorPosition(cursorPos + diff);
        isReplacingFullWidth = false;
        emit expressionChanged(replaced);
        return;
    }

    emit expressionChanged(text);
}

/*
 * Function: Initialize display layout
 * Input: none
 * Output: none
 * Return: none
 */
void CalcDisplay::CalcDisplayInitLayout() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(8, 8, 8, 8);
    layout->setSpacing(4);

    expressionEdit = new QLineEdit(this);
    expressionEdit->setAlignment(Qt::AlignRight);
    expressionEdit->setPlaceholderText("Expression");
    expressionEdit->setFrame(false);
    connect(expressionEdit, &QLineEdit::textChanged,
            this, &CalcDisplay::CalcDisplayOnExpressionEdited);

    resultLabel = new QLabel(RESULT_PLACEHOLDER, this);
    resultLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    resultLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);

    layout->addWidget(expressionEdit);
    layout->addWidget(resultLabel);

    setMinimumHeight(DISPLAY_MIN_HEIGHT);
}

/*
 * Function: Initialize display style
 * Input: none
 * Output: none
 * Return: none
 */
void CalcDisplay::CalcDisplayInitStyle() {
    expressionEdit->setStyleSheet(QString(
        "QLineEdit {"
        "   font-size: %1px;"
        "   color: #666666;"
        "   background: transparent;"
        "   border: none;"
        "   padding: 4px;"
        "}"
    ).arg(EXPRESSION_FONT_SIZE));

    resultLabel->setStyleSheet(QString(
        "QLabel {"
        "   font-size: %1px;"
        "   color: #222222;"
        "   background: transparent;"
        "   padding: 4px;"
        "}"
    ).arg(RESULT_FONT_SIZE));

    setStyleSheet(
        "CalcDisplay {"
        "   background-color: #ffffff;"
        "   border-bottom: 1px solid #d0d0d0;"
        "}"
    );
}

struct FullWidthEntry {
    ushort fullWidth;
    char halfWidth;
};

static const FullWidthEntry FULL_WIDTH_MAP[] = {
    { 0xFF08, '(' },
    { 0xFF09, ')' },
    { 0xFF3B, '[' },
    { 0xFF3D, ']' },
    { 0xFF5B, '{' },
    { 0xFF5D, '}' },
    { 0xFF0B, '+' },
    { 0xFF0D, '-' },
    { 0xFF0A, '*' },
    { 0xFF0F, '/' },
    { 0xFF0E, '.' },
    { 0x3010, '[' },
    { 0x3011, ']' },
    { 0x300C, '(' },
    { 0x300D, ')' },
    { 0x300E, '(' },
    { 0x300F, ')' },
};

const size_t FULL_WIDTH_MAP_SIZE = sizeof(FULL_WIDTH_MAP) / sizeof(FULL_WIDTH_MAP[0]);

static bool CalcIsAllowedChar(QChar ch) {
    if (ch.isDigit()) {
        return true;
    }
    if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
        return true;
    }
    if (ch == '.' || ch == '(' || ch == ')' || ch == '[' || ch == ']' ||
        ch == '{' || ch == '}') {
        return true;
    }
    return false;
}

/*
 * Function: Replace full-width characters with ASCII equivalents and filter out unsupported characters
 * Input: text - input string possibly containing full-width or unsupported characters
 * Output: none
 * Return: string with full-width characters replaced and unsupported characters removed
 */
QString CalcDisplay::CalcDisplayReplaceFullWidth(const QString& text) {
    QString result;
    result.reserve(text.size());

    for (int i = 0; i < text.size(); i++) {
        QChar ch = text[i];

        bool replaced = false;
        for (size_t j = 0; j < FULL_WIDTH_MAP_SIZE; j++) {
            if (ch.unicode() == FULL_WIDTH_MAP[j].fullWidth) {
                result += FULL_WIDTH_MAP[j].halfWidth;
                replaced = true;
                break;
            }
        }

        if (!replaced) {
            if (ch.unicode() >= 0xFF10 && ch.unicode() <= 0xFF19) {
                result += static_cast<char>('0' + (ch.unicode() - 0xFF10));
            } else if (CalcIsAllowedChar(ch)) {
                result += ch;
            }
        }
    }

    return result;
}
