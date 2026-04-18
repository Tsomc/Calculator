#include "calc_button.h"

const int BUTTON_MIN_HEIGHT = 40;
const int BUTTON_MIN_WIDTH = 60;
const int BUTTON_FONT_SIZE = 16;

/*
 * Function: Construct a CalcButton with text, role, and optional keyboard key
 * Input: text - button label; role - functional role; key - associated keyboard key; parent - parent widget
 * Output: none
 * Return: none
 */
CalcButton::CalcButton(const QString& text, CalcButtonRole role, Qt::Key key, QWidget* parent)
    : QPushButton(text, parent), buttonRole(role), buttonKey(key) {
    setMinimumHeight(BUTTON_MIN_HEIGHT);
    setMinimumWidth(BUTTON_MIN_WIDTH);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    CalcButtonInitStyle();
}

/*
 * Function: Get the functional role of this button
 * Input: none
 * Output: none
 * Return: the button role
 */
CalcButtonRole CalcButton::CalcButtonGetRole() const {
    return buttonRole;
}

/*
 * Function: Get the associated keyboard key
 * Input: none
 * Output: none
 * Return: the keyboard key
 */
Qt::Key CalcButton::CalcButtonGetKey() const {
    return buttonKey;
}

/*
 * Function: Initialize button style based on role
 * Input: none
 * Output: none
 * Return: none
 */
void CalcButton::CalcButtonInitStyle() {
    QString baseStyle = QString(
        "QPushButton {"
        "   font-size: %1px;"
        "   border: 1px solid #d0d0d0;"
        "   border-radius: 4px;"
        "   padding: 6px;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #c0c0c0;"
        "}"
    ).arg(BUTTON_FONT_SIZE);

    switch (buttonRole) {
    case CALC_BUTTON_ROLE_DIGIT:
        baseStyle += "QPushButton { background-color: #ffffff; color: #333333; }";
        break;
    case CALC_BUTTON_ROLE_OPERATOR:
        baseStyle += "QPushButton { background-color: #f0f0f0; color: #333333; }";
        break;
    case CALC_BUTTON_ROLE_BRACKET:
        baseStyle += "QPushButton { background-color: #f0f0f0; color: #333333; }";
        break;
    case CALC_BUTTON_ROLE_ACTION:
        baseStyle += "QPushButton { background-color: #e8e8e8; color: #333333; }";
        break;
    case CALC_BUTTON_ROLE_EQUALS:
        baseStyle += "QPushButton { background-color: #4cc2ff; color: #ffffff; font-weight: bold; }"
                     "QPushButton:pressed { background-color: #3aa8e0; }";
        break;
    }

    setStyleSheet(baseStyle);
}
