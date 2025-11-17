#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include "qcustomplot.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QCustomPlot *customPlot;
    void setupPlot();
};

#endif // MAINWINDOW_H
