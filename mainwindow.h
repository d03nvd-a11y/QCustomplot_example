#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>
#include <QSpinBox>
#include <QLabel>
#include "qcustomplot.h"

// Data structures for time series
struct DataPoint {
    double time;
    double series1Value;
    double series2Value;
    double series3Value;
};

struct TimeSeriesData {
    QVector<DataPoint> dataPoints;
};

struct ObjectData {
    TimeSeriesData metricA;
    TimeSeriesData metricB;
    TimeSeriesData metricC;
};

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
    ObjectData generateFakeData(int objectId);
    void updatePlot(QCustomPlot *plot, const TimeSeriesData &data);

    // Setup methods
    void setupUI();
    void setupPlots();
    void updateGraphs(int objectId);
};

#endif // MAINWINDOW_H
