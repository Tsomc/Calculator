#include "calc_history.h"
#include <QVBoxLayout>
#include <QHBoxLayout>

const int HISTORY_MAX_ENTRIES = 50;
const int HISTORY_FONT_SIZE = 12;
const int TOGGLE_BUTTON_HEIGHT = 24;
const char* TOGGLE_EXPAND_TEXT = "History ▼";
const char* TOGGLE_COLLAPSE_TEXT = "History ▲";

/*
 * Function: Construct CalcHistory with collapsible panel
 * Input: parent - parent widget
 * Output: none
 * Return: none
 */
CalcHistory::CalcHistory(QWidget* parent)
    : QWidget(parent), collapsed(true) {
    historyList = nullptr;
    toggleButton = nullptr;
    CalcHistoryInitLayout();
    CalcHistoryInitStyle();
}

/*
 * Function: Add a calculation entry to history
 * Input: expression - the expression string; result - the result string
 * Output: none
 * Return: none
 */
void CalcHistory::CalcHistoryAddEntry(const QString& expression, const QString& result) {
    if (historyList->count() >= HISTORY_MAX_ENTRIES) {
        delete historyList->takeItem(0);
    }

    QString entryText = expression + " = " + result;
    QListWidgetItem* item = new QListWidgetItem(entryText, historyList);
    item->setData(Qt::UserRole, expression);
    historyList->scrollToBottom();
}

/*
 * Function: Clear all history entries
 * Input: none
 * Output: none
 * Return: none
 */
void CalcHistory::CalcHistoryClear() {
    historyList->clear();
}

/*
 * Function: Toggle the collapsed state of the history panel
 * Input: none
 * Output: none
 * Return: none
 */
void CalcHistory::CalcHistoryToggleCollapsed() {
    collapsed = !collapsed;
    historyList->setVisible(!collapsed);
    toggleButton->setText(collapsed ? TOGGLE_EXPAND_TEXT : TOGGLE_COLLAPSE_TEXT);
}

/*
 * Function: Check if the history panel is collapsed
 * Input: none
 * Output: none
 * Return: true if collapsed, false otherwise
 */
bool CalcHistory::CalcHistoryIsCollapsed() const {
    return collapsed;
}

/*
 * Function: Handle history item click to fill expression back
 * Input: item - the clicked list widget item
 * Output: emits entryClicked signal
 * Return: none
 */
void CalcHistory::CalcHistoryOnItemClicked(QListWidgetItem* item) {
    if (item != nullptr) {
        QString expression = item->data(Qt::UserRole).toString();
        emit entryClicked(expression);
    }
}

/*
 * Function: Initialize history layout
 * Input: none
 * Output: none
 * Return: none
 */
void CalcHistory::CalcHistoryInitLayout() {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    toggleButton = new QPushButton(TOGGLE_EXPAND_TEXT, this);
    toggleButton->setCheckable(false);
    toggleButton->setFixedHeight(TOGGLE_BUTTON_HEIGHT);
    connect(toggleButton, &QPushButton::clicked,
            this, &CalcHistory::CalcHistoryToggleCollapsed);

    historyList = new QListWidget(this);
    historyList->setVisible(false);
    historyList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    historyList->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    connect(historyList, &QListWidget::itemClicked,
            this, &CalcHistory::CalcHistoryOnItemClicked);

    layout->addWidget(toggleButton);
    layout->addWidget(historyList);
}

/*
 * Function: Initialize history style
 * Input: none
 * Output: none
 * Return: none
 */
void CalcHistory::CalcHistoryInitStyle() {
    toggleButton->setStyleSheet(QString(
        "QPushButton {"
        "   font-size: %1px;"
        "   background-color: #f5f5f5;"
        "   border: none;"
        "   border-top: 1px solid #d0d0d0;"
        "   color: #666666;"
        "   padding: 2px 8px;"
        "   text-align: left;"
        "}"
        "QPushButton:hover {"
        "   background-color: #ebebeb;"
        "}"
    ).arg(HISTORY_FONT_SIZE));

    historyList->setStyleSheet(QString(
        "QListWidget {"
        "   font-size: %1px;"
        "   background-color: #fafafa;"
        "   border: none;"
        "   border-top: 1px solid #d0d0d0;"
        "   outline: none;"
        "}"
        "QListWidget::item {"
        "   padding: 4px 8px;"
        "   border-bottom: 1px solid #eeeeee;"
        "}"
        "QListWidget::item:hover {"
        "   background-color: #e8e8e8;"
        "}"
    ).arg(HISTORY_FONT_SIZE));
}
