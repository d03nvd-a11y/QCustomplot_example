#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QSpinBox>
#include <QLabel>
#include "qcustomplot.h"

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onObjectIdChanged(int objectId);

private:
    // UI Components
    QSpinBox *objectIdSpinBox;
    QLabel *objectIdLabel;
    QCustomPlot *plotTemperature;
    QCustomPlot *plotHumidity;
    QCustomPlot *plotPressure;

    // Data generation
    void generateFakeData(int objectId, QVector<double> &time,
                         QVector<double> &temp1, QVector<double> &temp2, QVector<double> &temp3,
                         QVector<double> &hum1, QVector<double> &hum2, QVector<double> &hum3,
                         QVector<double> &press1, QVector<double> &press2, QVector<double> &press3);

    // Setup methods
    void setupUI();
    void setupPlots();
    void updateGraphs(int objectId);
};

#endif // MAINWINDOW_H
