#ifndef CALC_DISPLAY_H
#define CALC_DISPLAY_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>

/*
 * Class: Expression and result display area
 * Input: expression text, result text
 * Output: expressionChanged signal when expression changes
 */
class CalcDisplay : public QWidget {
    Q_OBJECT

public:
    explicit CalcDisplay(QWidget* parent = nullptr);

    void CalcDisplaySetExpression(const QString& text);
    QString CalcDisplayGetExpression() const;
    void CalcDisplaySetResult(const QString& text);
    QString CalcDisplayGetResult() const;
    void CalcDisplayAppendChar(QChar ch);
    void CalcDisplayRemoveLastChar();
    void CalcDisplayClearExpression();
    void CalcDisplayClearAll();
    void CalcDisplayToggleSign();
    void CalcDisplaySetCompact(bool compact);

signals:
    void expressionChanged(const QString& expression);

private slots:
    void CalcDisplayOnExpressionEdited(const QString& text);

private:
    QLineEdit* expressionEdit;
    QLabel* resultLabel;
    bool isReplacingFullWidth;
    bool isCompact;

    void CalcDisplayInitLayout();
    void CalcDisplayInitStyle();
    static QString CalcDisplayReplaceFullWidth(const QString& text);
};

#endif
