#include "calc_window.h"
#include "calc_display.h"
#include "calc_button.h"
#include "calc_history.h"
#include "calc_parser.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QKeyEvent>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>

const int WINDOW_MIN_WIDTH = 320;
const int WINDOW_MIN_HEIGHT = 480;
const int TITLE_BAR_HEIGHT = 30;
const int TITLE_BUTTON_WIDTH = 46;
const int TITLE_BUTTON_HEIGHT = 30;
const int GRID_SPACING = 4;
const int GRID_MARGIN = 8;

struct ButtonDef {
    const char* text;
    CalcButtonRole role;
    Qt::Key key;
    int row;
    int col;
    int rowSpan;
    int colSpan;
};

static const ButtonDef BUTTON_DEFS[] = {
    {"CE",   CALC_BUTTON_ROLE_ACTION,   Qt::Key_Escape,  0, 0, 1, 1},
    {"C",    CALC_BUTTON_ROLE_ACTION,   Qt::Key_unknown,  0, 1, 1, 1},
    {"\u2190", CALC_BUTTON_ROLE_ACTION, Qt::Key_Backspace, 0, 2, 1, 1},
    {"/",    CALC_BUTTON_ROLE_OPERATOR, Qt::Key_Slash,    0, 3, 1, 1},
    {"(",    CALC_BUTTON_ROLE_BRACKET,  Qt::Key_ParenLeft,  1, 0, 1, 1},
    {")",    CALC_BUTTON_ROLE_BRACKET,  Qt::Key_ParenRight, 1, 1, 1, 1},
    {"[",    CALC_BUTTON_ROLE_BRACKET,  Qt::Key_BracketLeft,  1, 2, 1, 1},
    {"*",    CALC_BUTTON_ROLE_OPERATOR, Qt::Key_Asterisk, 1, 3, 1, 1},
    {"{",    CALC_BUTTON_ROLE_BRACKET,  Qt::Key_BraceLeft,  2, 0, 1, 1},
    {"}",    CALC_BUTTON_ROLE_BRACKET,  Qt::Key_BraceRight, 2, 1, 1, 1},
    {"]",    CALC_BUTTON_ROLE_BRACKET,  Qt::Key_BracketRight, 2, 2, 1, 1},
    {"-",    CALC_BUTTON_ROLE_OPERATOR, Qt::Key_Minus,    2, 3, 1, 1},
    {"7",    CALC_BUTTON_ROLE_DIGIT,    Qt::Key_7,        3, 0, 1, 1},
    {"8",    CALC_BUTTON_ROLE_DIGIT,    Qt::Key_8,        3, 1, 1, 1},
    {"9",    CALC_BUTTON_ROLE_DIGIT,    Qt::Key_9,        3, 2, 1, 1},
    {"+",    CALC_BUTTON_ROLE_OPERATOR, Qt::Key_Plus,     3, 3, 1, 1},
    {"4",    CALC_BUTTON_ROLE_DIGIT,    Qt::Key_4,        4, 0, 1, 1},
    {"5",    CALC_BUTTON_ROLE_DIGIT,    Qt::Key_5,        4, 1, 1, 1},
    {"6",    CALC_BUTTON_ROLE_DIGIT,    Qt::Key_6,        4, 2, 1, 1},
    {"=",    CALC_BUTTON_ROLE_EQUALS,   Qt::Key_Return,   4, 3, 2, 1},
    {"1",    CALC_BUTTON_ROLE_DIGIT,    Qt::Key_1,        5, 0, 1, 1},
    {"2",    CALC_BUTTON_ROLE_DIGIT,    Qt::Key_2,        5, 1, 1, 1},
    {"3",    CALC_BUTTON_ROLE_DIGIT,    Qt::Key_3,        5, 2, 1, 1},
    {"+/-",  CALC_BUTTON_ROLE_ACTION,   Qt::Key_unknown,  6, 0, 1, 1},
    {"0",    CALC_BUTTON_ROLE_DIGIT,    Qt::Key_0,        6, 1, 1, 1},
    {".",    CALC_BUTTON_ROLE_DIGIT,    Qt::Key_Period,   6, 2, 1, 1},
};

const size_t BUTTON_DEFS_COUNT = sizeof(BUTTON_DEFS) / sizeof(BUTTON_DEFS[0]);

/*
 * Function: Construct the main calculator window
 * Input: parent - parent widget
 * Output: none
 * Return: none
 */
CalcWindow::CalcWindow(QWidget* parent)
    : QWidget(parent), display(nullptr), history(nullptr),
      modeLabel(nullptr), minimizeBtn(nullptr), maximizeBtn(nullptr), closeBtn(nullptr),
      isDragging(false), isMaximizedState(false) {
    setWindowFlags(Qt::FramelessWindowHint);
    setMinimumSize(WINDOW_MIN_WIDTH, WINDOW_MIN_HEIGHT);
    setContextMenuPolicy(Qt::NoContextMenu);

    CalcWindowInitTitleBar();
    CalcWindowInitButtons();
    CalcWindowInitLayout();
    CalcWindowInitStyle();

    installEventFilter(this);
}

/*
 * Function: Handle keyboard key press events
 * Input: event - key event
 * Output: none
 * Return: none
 */
void CalcWindow::keyPressEvent(QKeyEvent* event) {
    int key = event->key();
    Qt::KeyboardModifiers mods = event->modifiers();

    if (mods & Qt::ControlModifier) {
        if (key == Qt::Key_C) {
            QString result = display->CalcDisplayGetResult();
            if (!result.isEmpty()) {
                QApplication::clipboard()->setText(result);
            }
            return;
        }
        if (key == Qt::Key_V) {
            QString clipText = QApplication::clipboard()->text();
            if (!clipText.isEmpty()) {
                display->CalcDisplaySetExpression(clipText);
            }
            return;
        }
        return;
    }

    if (key == Qt::Key_Enter) {
        key = Qt::Key_Return;
    }

    QList<CalcButton*> buttons = findChildren<CalcButton*>();
    for (CalcButton* btn : buttons) {
        if (btn->CalcButtonGetKey() == key) {
            CalcWindowOnButtonClicked(btn->text());
            return;
        }
    }

    QWidget::keyPressEvent(event);
}

/*
 * Function: Event filter to suppress unwanted context menus
 * Input: watched - watched object; event - event object
 * Output: none
 * Return: true if event should be filtered
 */
bool CalcWindow::eventFilter(QObject* watched, QEvent* event) {
    if (event->type() == QEvent::ContextMenu) {
        return true;
    }
    return QWidget::eventFilter(watched, event);
}

/*
 * Function: Handle mouse press for title bar dragging
 * Input: event - mouse event
 * Output: none
 * Return: none
 */
void CalcWindow::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && event->position().y() < TITLE_BAR_HEIGHT) {
        isDragging = true;
        dragStartPos = event->globalPosition().toPoint() - frameGeometry().topLeft();
    }
    QWidget::mousePressEvent(event);
}

/*
 * Function: Handle mouse move for title bar dragging
 * Input: event - mouse event
 * Output: none
 * Return: none
 */
void CalcWindow::mouseMoveEvent(QMouseEvent* event) {
    if (isDragging) {
        move(event->globalPosition().toPoint() - dragStartPos);
    }
    QWidget::mouseMoveEvent(event);
}

/*
 * Function: Handle mouse release to stop dragging
 * Input: event - mouse event
 * Output: none
 * Return: none
 */
void CalcWindow::mouseReleaseEvent(QMouseEvent* event) {
    isDragging = false;
    QWidget::mouseReleaseEvent(event);
}

/*
 * Function: Handle button click by dispatching to role-specific handler
 * Input: text - button label text
 * Output: none
 * Return: none
 */
void CalcWindow::CalcWindowOnButtonClicked(const QString& text) {
    CalcButton* btn = qobject_cast<CalcButton*>(sender());
    if (btn == nullptr) {
        QList<CalcButton*> buttons = findChildren<CalcButton*>();
        for (CalcButton* b : buttons) {
            if (b->text() == text) {
                btn = b;
                break;
            }
        }
    }

    if (btn == nullptr) {
        return;
    }

    CalcButtonRole role = btn->CalcButtonGetRole();
    switch (role) {
    case CALC_BUTTON_ROLE_DIGIT:
        CalcWindowProcessDigit(text);
        break;
    case CALC_BUTTON_ROLE_OPERATOR:
        CalcWindowProcessOperator(text);
        break;
    case CALC_BUTTON_ROLE_BRACKET:
        CalcWindowProcessBracket(text);
        break;
    case CALC_BUTTON_ROLE_ACTION:
        CalcWindowProcessAction(text);
        break;
    case CALC_BUTTON_ROLE_EQUALS:
        CalcWindowProcessEquals();
        break;
    }
}

/*
 * Function: Handle expression changed - perform real-time calculation
 * Input: expression - current expression text
 * Output: updates result display
 * Return: none
 */
void CalcWindow::CalcWindowOnExpressionChanged(const QString& expression) {
    if (expression.isEmpty()) {
        display->CalcDisplaySetResult("0");
        return;
    }

    double result = 0;
    std::string errorMsg;
    std::string input = expression.toStdString();

    if (CALC_Evaluate(input, result, errorMsg)) {
        display->CalcDisplaySetResult(QString::number(result, 'g', 15));
    }
}

/*
 * Function: Handle history entry click to fill expression back
 * Input: expression - the expression from history
 * Output: none
 * Return: none
 */
void CalcWindow::CalcWindowOnHistoryEntryClicked(const QString& expression) {
    display->CalcDisplaySetExpression(expression);
}

/*
 * Function: Minimize the window
 * Input: none
 * Output: none
 * Return: none
 */
void CalcWindow::CalcWindowOnMinimize() {
    showMinimized();
}

/*
 * Function: Toggle maximize/restore window
 * Input: none
 * Output: none
 * Return: none
 */
void CalcWindow::CalcWindowOnMaximize() {
    if (isMaximizedState) {
        setGeometry(geometryBeforeMaximize);
        isMaximizedState = false;
    } else {
        geometryBeforeMaximize = geometry();
        setGeometry(screen()->availableGeometry());
        isMaximizedState = true;
    }
}

/*
 * Function: Initialize the custom title bar
 * Input: none
 * Output: none
 * Return: none
 */
void CalcWindow::CalcWindowInitTitleBar() {
    modeLabel = new QLabel("Standard", this);
    modeLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    minimizeBtn = new QPushButton("\u2014", this);
    minimizeBtn->setFixedSize(TITLE_BUTTON_WIDTH, TITLE_BUTTON_HEIGHT);
    connect(minimizeBtn, &QPushButton::clicked, this, &CalcWindow::CalcWindowOnMinimize);

    maximizeBtn = new QPushButton("\u25A1", this);
    maximizeBtn->setFixedSize(TITLE_BUTTON_WIDTH, TITLE_BUTTON_HEIGHT);
    connect(maximizeBtn, &QPushButton::clicked, this, &CalcWindow::CalcWindowOnMaximize);

    closeBtn = new QPushButton("\u2715", this);
    closeBtn->setFixedSize(TITLE_BUTTON_WIDTH, TITLE_BUTTON_HEIGHT);
    connect(closeBtn, &QPushButton::clicked, this, &QWidget::close);

    minimizeBtn->setContextMenuPolicy(Qt::NoContextMenu);
    maximizeBtn->setContextMenuPolicy(Qt::NoContextMenu);
    closeBtn->setContextMenuPolicy(Qt::NoContextMenu);
}

/*
 * Function: Initialize all calculator buttons from definition table
 * Input: none
 * Output: none
 * Return: none
 */
void CalcWindow::CalcWindowInitButtons() {
}

/*
 * Function: Initialize the main layout with title bar, display, buttons, and history
 * Input: none
 * Output: none
 * Return: none
 */
void CalcWindow::CalcWindowInitLayout() {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QHBoxLayout* titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(8, 0, 0, 0);
    titleLayout->setSpacing(0);
    titleLayout->addWidget(modeLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(minimizeBtn);
    titleLayout->addWidget(maximizeBtn);
    titleLayout->addWidget(closeBtn);

    QWidget* titleBar = new QWidget(this);
    titleBar->setFixedHeight(TITLE_BAR_HEIGHT);
    titleBar->setLayout(titleLayout);
    titleBar->setContextMenuPolicy(Qt::NoContextMenu);

    display = new CalcDisplay(this);
    connect(display, &CalcDisplay::expressionChanged,
            this, &CalcWindow::CalcWindowOnExpressionChanged);

    QGridLayout* gridLayout = new QGridLayout();
    gridLayout->setSpacing(GRID_SPACING);
    gridLayout->setContentsMargins(GRID_MARGIN, GRID_MARGIN, GRID_MARGIN, GRID_MARGIN);

    for (size_t i = 0; i < BUTTON_DEFS_COUNT; i++) {
        const ButtonDef& def = BUTTON_DEFS[i];
        CalcButton* btn = new CalcButton(def.text, def.role, def.key, this);
        btn->setContextMenuPolicy(Qt::NoContextMenu);
        connect(btn, &QPushButton::clicked, this, [this, btn]() {
            CalcWindowOnButtonClicked(btn->text());
        });
        gridLayout->addWidget(btn, def.row, def.col, def.rowSpan, def.colSpan);
    }

    history = new CalcHistory(this);
    connect(history, &CalcHistory::entryClicked,
            this, &CalcWindow::CalcWindowOnHistoryEntryClicked);

    mainLayout->addWidget(titleBar);
    mainLayout->addWidget(display);
    mainLayout->addLayout(gridLayout);
    mainLayout->addWidget(history);
}

/*
 * Function: Initialize window style
 * Input: none
 * Output: none
 * Return: none
 */
void CalcWindow::CalcWindowInitStyle() {
    setStyleSheet(
        "CalcWindow {"
        "   background-color: #f5f5f5;"
        "}"
    );

    modeLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 12px;"
        "   color: #444444;"
        "   padding-left: 4px;"
        "}"
    );

    QString titleBtnBase =
        "QPushButton {"
        "   border: none;"
        "   font-size: 14px;"
        "   background-color: transparent;"
        "   color: #444444;"
        "}"
        "QPushButton:hover {"
        "   background-color: #e0e0e0;"
        "}";

    minimizeBtn->setStyleSheet(titleBtnBase);
    maximizeBtn->setStyleSheet(titleBtnBase);

    closeBtn->setStyleSheet(
        "QPushButton {"
        "   border: none;"
        "   font-size: 14px;"
        "   background-color: transparent;"
        "   color: #444444;"
        "}"
        "QPushButton:hover {"
        "   background-color: #e81123;"
        "   color: #ffffff;"
        "}"
    );
}

/*
 * Function: Process equals button - evaluate and add to history
 * Input: none
 * Output: updates result and history
 * Return: none
 */
void CalcWindow::CalcWindowProcessEquals() {
    QString expression = display->CalcDisplayGetExpression();
    if (expression.isEmpty()) {
        return;
    }

    double result = 0;
    std::string errorMsg;
    std::string input = expression.toStdString();

    if (CALC_Evaluate(input, result, errorMsg)) {
        QString resultStr = QString::number(result, 'g', 15);
        display->CalcDisplaySetResult(resultStr);
        history->CalcHistoryAddEntry(expression, resultStr);
    } else {
        display->CalcDisplaySetResult(QString::fromStdString(errorMsg));
    }
}

/*
 * Function: Process digit input (0-9 and decimal point)
 * Input: text - the digit character
 * Output: none
 * Return: none
 */
void CalcWindow::CalcWindowProcessDigit(const QString& text) {
    display->CalcDisplayAppendChar(text[0]);
}

/*
 * Function: Process operator input (+, -, *, /)
 * Input: text - the operator character
 * Output: none
 * Return: none
 */
void CalcWindow::CalcWindowProcessOperator(const QString& text) {
    display->CalcDisplayAppendChar(text[0]);
}

/*
 * Function: Process bracket input (parentheses, square brackets, curly braces)
 * Input: text - the bracket character
 * Output: none
 * Return: none
 */
void CalcWindow::CalcWindowProcessBracket(const QString& text) {
    display->CalcDisplayAppendChar(text[0]);
}

/*
 * Function: Process action buttons (CE, C, backspace, +/-)
 * Input: text - the action button label
 * Output: none
 * Return: none
 */
void CalcWindow::CalcWindowProcessAction(const QString& text) {
    if (text == "CE") {
        display->CalcDisplayClearExpression();
    } else if (text == "C") {
        display->CalcDisplayClearAll();
    } else if (text == "\u2190") {
        display->CalcDisplayRemoveLastChar();
    } else if (text == "+/-") {
        display->CalcDisplayToggleSign();
    }
}
