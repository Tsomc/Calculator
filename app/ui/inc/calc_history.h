#ifndef CALC_HISTORY_H
#define CALC_HISTORY_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVector>

struct CalcHistoryEntry {
    QString expression;
    QString result;
};

/*
 * Class: Collapsible history panel showing past calculations
 * Input: history entries added by user action
 * Output: entryClicked signal when an entry is selected
 */
class CalcHistory : public QWidget {
    Q_OBJECT

public:
    explicit CalcHistory(QWidget* parent = nullptr);

    void CalcHistoryAddEntry(const QString& expression, const QString& result);
    void CalcHistoryClear();
    void CalcHistoryToggleCollapsed();
    bool CalcHistoryIsCollapsed() const;

signals:
    void entryClicked(const QString& expression);

private slots:
    void CalcHistoryOnItemClicked(QListWidgetItem* item);

private:
    QListWidget* historyList;
    QPushButton* toggleButton;
    bool collapsed;

    void CalcHistoryInitLayout();
    void CalcHistoryInitStyle();
};

#endif
