#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_guischedular.h"

class guischedular : public QMainWindow
{
    Q_OBJECT

public:
    guischedular(QWidget *parent = nullptr);
    ~guischedular();

private:
    Ui::guischedularClass ui;
};
