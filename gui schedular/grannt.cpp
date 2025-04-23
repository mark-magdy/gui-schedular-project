#include "grannt.h"
#include <QGraphicsTextItem>
#include <QGraphicsRectItem>
#include <QVBoxLayout>
#include <QPen>
#include <QBrush>
#include <algorithm>

GanttChartWidget::GanttChartWidget(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);

    timeLabel = new QLabel("Time: 0");
    timeLabel->setAlignment(Qt::AlignCenter);
    timeLabel->setStyleSheet("font-weight: bold; font-size: 16px;");
    layout->addWidget(timeLabel);

    QLabel* title = new QLabel("Gantt Chart");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-weight: bold; font-size: 16px;");
    layout->addWidget(title);

    view = new QGraphicsView();
    scene = new QGraphicsScene();
    view->setScene(scene);
    layout->addWidget(view);

    setLayout(layout);
}

void GanttChartWidget::updateTimeCounter(float currentTime) {
    timeLabel->setText(QString("Time: %1").arg(currentTime, 0, 'f', 1));
}
//
//void GanttChartWidget::updateGanttChart(const std::vector<process*>& processes, float currentTime) {
//    scene->clear();
//    if (processes.empty()) return;
//
//    // Determine the visible time range
//    float maxTime = currentTime;
//    for (const auto& process : processes) {
//        for (const auto& [start, end] : process->getSchedule()) {
//            if (end <= currentTime) {
//                maxTime = max(maxTime, end);
//            }
//        }
//    }
//    if (maxTime == 0) return;
//
//    // Prepare the scene dimensions
//    int width = view->width() - 50;
//    int height = 50 + processes.size() * 40;
//    scene->setSceneRect(0, 0, width, height);
//
//    // Timeline scale
//    float timeScale = width / maxTime +1;
//    for (int i = 0; i <= maxTime; i++) {
//        int x = i * timeScale;
//        scene->addLine(x, 0, x, height - 30, QPen(Qt::gray));
//        QGraphicsTextItem* text = scene->addText(QString::number(i));
//        text->setPos(x - 10, height - 25);
//    }
//
//    // Current time marker
//    int currentX = currentTime * timeScale;
//    scene->addLine(currentX, 0, currentX, height, QPen(Qt::red, 2));
//
//    // Color palette
//    std::vector<QColor> colors = {
//        QColor(255, 100, 100), QColor(100, 255, 100), QColor(100, 100, 255),
//        QColor(255, 255, 100), QColor(255, 100, 255), QColor(100, 255, 255)
//    };
//
//    // Draw process bars
//    int row = 0;
//    for (const auto& process : processes) {
//        QGraphicsTextItem* label = scene->addText(QString::fromStdString(process->getName()));
//        label->setPos(-40, 20 + row * 40);
//
//        for (const auto& [start, end] : process->getSchedule()) {
//            if (end <= currentTime) {
//                int x1 = start * timeScale;
//                int x2 = end * timeScale;
//                int w = x2 - x1;
//
//                QColor color = colors[row % colors.size()];
//                QPen pen(Qt::black);
//                QBrush brush = (currentTime >= start && currentTime <= end)
//                    ? QBrush(color.darker(120)) : QBrush(color);
//
//                QGraphicsRectItem* rect = scene->addRect(x1, 10 + row * 40, w, 30, pen, brush);
//
//                if (w > 30) {
//                    QGraphicsTextItem* duration = scene->addText(QString::number(end - start, 'f', 1));
//                    duration->setDefaultTextColor(Qt::black);
//                    duration->setPos(x1 + w / 2 - 15, 15 + row * 40);
//                }
//            }
//        }
//        row++;
//    }
//}
void GanttChartWidget::updateGanttChart(const std::vector<process*>& processes, float currentTime) {
    scene->clear();
    if (processes.empty()) return;

    float maxTime = currentTime;
    for (const auto& process : processes) {
        for (const auto& [start, end] : process->getSchedule()) {
            if (end <= currentTime) {
                maxTime = std::max(maxTime, end);
            }
        }
    }
    if (maxTime == 0) return;

    int width = view->width() - 40;
    int height = 40 + processes.size() * 35;
    scene->setSceneRect(0, 0, width, height);
    float timeScale = width / maxTime;

    // Background grid lines and time labels
    for (int i = 0; i <= maxTime; ++i) {
        int x = i * timeScale;
        scene->addLine(x, 0, x, height, QPen(Qt::lightGray, 1, Qt::DashLine));
        QGraphicsTextItem* timeLabel = scene->addText(QString::number(i));
        timeLabel->setDefaultTextColor(Qt::darkGray);
        timeLabel->setPos(x - 5, 0);
    }

    // Current time marker
    int currentX = currentTime * timeScale;
    scene->addLine(currentX, 0, currentX, height, QPen(Qt::red, 2));

    std::vector<QColor> colors = {
        QColor("#e57373"), QColor("#64b5f6"), QColor("#81c784"),
        QColor("#ffd54f"), QColor("#ba68c8"), QColor("#4db6ac")
    };

    int row = 0;
    for (const auto& process : processes) {
        for (const auto& [start, end] : process->getSchedule()) {
            if (end <= currentTime) {
                int x = start * timeScale;
                int w = (end - start) * timeScale;

                QColor color = colors[row % colors.size()];
                QBrush brush(color);
                QPen pen(Qt::black);

                auto rect = scene->addRect(x, 25 + row * 35, w, 20, pen, brush);
                rect->setToolTip(QString("Process: %1\nStart: %2\nEnd: %3")
                    .arg(QString::fromStdString(process->getName()))
                    .arg(start).arg(end));

                auto label = scene->addText(QString::fromStdString(process->getName()));
                label->setDefaultTextColor(Qt::white);
                label->setPos(x + 3, 25 + row * 35);

                // Optional: duration on bar
                if (w > 30) {
                    auto duration = scene->addText(QString("%1").arg(end - start, 0, 'f', 1));
                    duration->setDefaultTextColor(Qt::black);
                    duration->setPos(x + w / 2 - 10, 25 + row * 35);
                }
            }
        }
        row++;
    }
}
