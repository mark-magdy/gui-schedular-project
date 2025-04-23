#ifndef GANTTCHARTWIDGET_H
#define GANTTCHARTWIDGET_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QLabel>
#include <vector>
#include "header.h"
#include <mutex>
#include <QThread>

class GanttChartWidget : public QWidget {
    Q_OBJECT
public:
    explicit GanttChartWidget(QWidget* parent = nullptr);
    void updateTimeCounter(float currentTime);
    void updateGanttChart(const std::vector<process*>& processes, float currentTime);

private:
    QGraphicsView* view;
    QGraphicsScene* scene;
    QLabel* timeLabel;
};

#endif // GANTTCHARTWIDGET_H