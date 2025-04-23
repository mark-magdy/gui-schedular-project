#ifndef LISTWIDGET_H
#define LISTWIDGET_H

#include <QWidget>
#include <QFrame>
#include <QLabel>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <vector>
#include "header.h"  // Assuming this is where your process class is defined

class ProcessCard : public QFrame {
    Q_OBJECT
public:
    explicit ProcessCard(process* p, QWidget* parent = nullptr);
};

class ProcessListWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProcessListWidget(QWidget* parent = nullptr);
    void updateProcessList(const std::vector<process*>& processes);

private:
    QVBoxLayout* cardsLayout;
};

#endif // LISTWIDGET_H