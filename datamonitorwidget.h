#ifndef DATAMONITORWIDGET_H
#define DATAMONITORWIDGET_H

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

class DataMonitorWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DataMonitorWidget(QWidget *parent = nullptr);
    ~DataMonitorWidget();

private slots:
    void onObjectIdChanged(int objectId);
    void onPlotClicked(QMouseEvent *event);

private:
    // UI Components
    QSpinBox *objectIdSpinBox;
    QLabel *objectIdLabel;
    QCustomPlot *plotTemperature;
    QCustomPlot *plotHumidity;
    QCustomPlot *plotPressure;

    // Callout components for each plot
    struct PlotCallout {
        QCPItemTracer *tracer1;
        QCPItemTracer *tracer2;
        QCPItemTracer *tracer3;
        QCPItemText *textLabel;
    };

    PlotCallout calloutTemperature;
    PlotCallout calloutHumidity;
    PlotCallout calloutPressure;

    // Data generation
    ObjectData generateFakeData(int objectId);
    void updatePlot(QCustomPlot *plot, const TimeSeriesData &data);

    // Setup methods
    void setupUI();
    void setupPlots();
    void setupCallouts(QCustomPlot *plot, PlotCallout &callout);
    void updateGraphs(int objectId);
    void showCallout(QCustomPlot *plot, PlotCallout &callout, double xCoord);
};

#endif // DATAMONITORWIDGET_H
