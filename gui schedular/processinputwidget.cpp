#include "processinputwidget.h"

ProcessInputWidget::ProcessInputWidget(QWidget* parent) : QWidget(parent) {
    QFormLayout* layout = new QFormLayout(this);

    //// Algorithm selection
    //algorithmCombo = new QComboBox();
    //algorithmCombo->addItems({ "FCFS", "SJF", "Priority", "Round Robin" });
    //layout->addRow("Scheduling Algorithm:", algorithmCombo);

    // Process input fields
    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Process name");
    layout->addRow("Name:", nameEdit);

    prioritySpin = new QSpinBox();
    prioritySpin->setRange(0, 10);
    layout->addRow("Priority (0-10):", prioritySpin);

   /* arrivalSpin = new QDoubleSpinBox();
    arrivalSpin->setRange(0, 100);
    arrivalSpin->setSingleStep(0.5);
    layout->addRow("Arrival Time:", arrivalSpin);*/

    burstSpin = new QDoubleSpinBox();
    burstSpin->setRange(1, 100);
    burstSpin->setSingleStep(1);
    layout->addRow("Burst Time:", burstSpin);

    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    QPushButton* addButton = new QPushButton("Add Process");
    QPushButton* runButton = new QPushButton("Run Scheduler");

    connect(addButton, &QPushButton::clicked, this, &ProcessInputWidget::addProcessClicked);
    connect(runButton, &QPushButton::clicked, this, &ProcessInputWidget::runSchedulerClicked);

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(runButton);
    layout->addRow(buttonLayout);
}

process ProcessInputWidget::getCurrentProcess() const {
    return process(
        nameEdit->text().toStdString(),
        SingletonCounter::getInstance().getValue(), 
        burstSpin->value(),
        prioritySpin->value()

    );
}
