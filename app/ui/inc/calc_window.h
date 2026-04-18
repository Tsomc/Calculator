#ifndef CALC_WINDOW_H
#define CALC_WINDOW_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>

class CalcDisplay;
class CalcButton;
class CalcHistory;

/*
 * Class: Main calculator window with frameless design and custom title bar
 * Input: keyboard events and button clicks
 * Output: real-time calculation results
 */
class CalcWindow : public QWidget {
    Q_OBJECT

public:
    explicit CalcWindow(QWidget* parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

private slots:
    void CalcWindowOnButtonClicked(const QString& text);
    void CalcWindowOnExpressionChanged(const QString& expression);
    void CalcWindowOnHistoryEntryClicked(const QString& expression);
    void CalcWindowOnHistoryToggled(bool collapsed);
    void CalcWindowOnMinimize();
    void CalcWindowOnMaximize();

private:
    CalcDisplay* display;
    CalcHistory* history;
    QLabel* modeLabel;
    QPushButton* minimizeBtn;
    QPushButton* maximizeBtn;
    QPushButton* closeBtn;

    bool isDragging;
    QPoint dragStartPos;
    bool isMaximizedState;
    QRect geometryBeforeMaximize;

    void CalcWindowInitTitleBar();
    void CalcWindowInitButtons();
    void CalcWindowInitLayout();
    void CalcWindowInitStyle();
    void CalcWindowProcessEquals();
    void CalcWindowProcessDigit(const QString& text);
    void CalcWindowProcessOperator(const QString& text);
    void CalcWindowProcessBracket(const QString& text);
    void CalcWindowProcessAction(const QString& text);
};

#endif
