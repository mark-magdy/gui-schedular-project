#include "listWidget.h"
#include <QHBoxLayout>
#include "header.h"

// Updated to take process pointer
ProcessCard::ProcessCard(process* p, QWidget* parent) : QFrame(parent) {
    setFrameShape(QFrame::StyledPanel);
    setFrameShadow(QFrame::Raised);
    setLineWidth(1);
    setFixedHeight(100);  // Increased height
    setStyleSheet(
        "background-color: #2D2D2D;"
        "color: #EEE;"
    );

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(12, 8, 12, 8);

    QWidget* headerWidget = new QWidget();
    QHBoxLayout* headerLayout = new QHBoxLayout(headerWidget);
    headerLayout->setContentsMargins(0, 0, 0, 0);

    QLabel* nameLabel = new QLabel(QString::fromStdString(p->getName()));
    nameLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    headerLayout->addWidget(nameLabel);

    // Status bulb
    QLabel* statusBulb = new QLabel();
    statusBulb->setFixedSize(14, 14);
    statusBulb->setStyleSheet(
        QString("border-radius: 7px; background-color: %1;")
        .arg(p->getFinished() ? "#4CAF50" : "#F44336")
    );
    headerLayout->addWidget(statusBulb);
    headerLayout->addStretch();

    layout->addWidget(headerWidget);

    QWidget* detailsWidget = new QWidget();
    QGridLayout* detailsLayout = new QGridLayout(detailsWidget);
    detailsLayout->setContentsMargins(0, 0, 0, 0);
    detailsLayout->setHorizontalSpacing(10);

    auto addDetail = [detailsLayout](const QString& label, const QString& value, int row, int col) {
        QLabel* lbl = new QLabel(label);
        lbl->setStyleSheet("color: #BBB;");
        detailsLayout->addWidget(lbl, row, col);

        QLabel* val = new QLabel(value);
        val->setStyleSheet("font-weight: 500;");
        detailsLayout->addWidget(val, row, col + 1);
        };

    addDetail("Priority:", QString::number(p->getPriority()), 0, 0);
    addDetail("Arrival:", QString::number(p->getArrivalTime(), 'f', 1), 1, 0);
    addDetail("Burst:", QString::number(p->getBurstTime(), 'f', 1), 0, 2);
    addDetail("Remaining:", QString::number(p->getRemainingTime(), 'f', 1), 1, 2);

    QLabel* statusLabel = new QLabel(p->getFinished() ? "Completed" : "Pending");
    statusLabel->setStyleSheet(
        QString("color: %1; font-weight: bold;")
        .arg(p->getFinished() ? "#4CAF50" : "#F44336")
    );
    detailsLayout->addWidget(statusLabel, 2, 3, 1, 1, Qt::AlignRight);

    layout->addWidget(detailsWidget);
}

ProcessListWidget::ProcessListWidget(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    QLabel* title = new QLabel("Process List");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-weight: bold; font-size: 16px;"
        "padding: 10px;"
        "background-color: #252525;"
        "color: #EEE;"
    );
    layout->addWidget(title);

    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setStyleSheet(
        "background-color: #252525;"
    );

    QWidget* cardsContainer = new QWidget();
    cardsLayout = new QVBoxLayout(cardsContainer);
    cardsLayout->setAlignment(Qt::AlignTop);
    cardsLayout->setSpacing(10);
    cardsLayout->setContentsMargins(12, 12, 12, 12);

    scrollArea->setWidget(cardsContainer);
    layout->addWidget(scrollArea);
}

// Updated to take vector of process pointers
void ProcessListWidget::updateProcessList(const std::vector<process*>& processes) {
    // Clear existing cards
    QMetaObject::invokeMethod(this, [this, processes]() {
        // Actual update code here
        QLayoutItem* item;
        while ((item = cardsLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        
        for (auto p : processes) {
            cardsLayout->addWidget(new ProcessCard(p));
        }
        
        cardsLayout->addStretch();
    }, Qt::QueuedConnection);
}