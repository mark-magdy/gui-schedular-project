#ifndef PROCESSINPUTWIDGET_H
#define PROCESSINPUTWIDGET_H

#include <QWidget>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include "header.h"
#include "SingletonCounter.h"
class ProcessInputWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProcessInputWidget(QWidget* parent = nullptr);

    process getCurrentProcess() const;
    QString getAlgorithm() const;

signals:
    void addProcessClicked();
    void runSchedulerClicked();

private:
    QLineEdit* nameEdit;
    QSpinBox* prioritySpin;
    QDoubleSpinBox* arrivalSpin;
    QDoubleSpinBox* burstSpin;
    //QComboBox* algorithmCombo;
};

#endif // PROCESSINPUTWIDGET_H