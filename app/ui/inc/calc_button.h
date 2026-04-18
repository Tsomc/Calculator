#ifndef CALC_BUTTON_H
#define CALC_BUTTON_H

#include <QPushButton>

enum CalcButtonRole {
    CALC_BUTTON_ROLE_DIGIT,
    CALC_BUTTON_ROLE_OPERATOR,
    CALC_BUTTON_ROLE_BRACKET,
    CALC_BUTTON_ROLE_ACTION,
    CALC_BUTTON_ROLE_EQUALS
};

/*
 * Class: Calculator button with role-based styling and keyboard mapping
 * Input: text - button label; role - button functional role; key - associated keyboard key
 * Output: clicked signal with button text
 */
class CalcButton : public QPushButton {
    Q_OBJECT

public:
    explicit CalcButton(const QString& text, CalcButtonRole role, Qt::Key key = Qt::Key_unknown,
        QWidget* parent = nullptr);

    CalcButtonRole CalcButtonGetRole() const;
    Qt::Key CalcButtonGetKey() const;

private:
    CalcButtonRole buttonRole;
    Qt::Key buttonKey;

    void CalcButtonInitStyle();
};

#endif
