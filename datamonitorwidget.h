#ifndef DATAMONITORWIDGET_H
#define DATAMONITORWIDGET_H

#include <QWidget>
#include <QSpinBox>
#include <QLabel>
#include "qcustomplot.h"

// Configuration constants
namespace Config {
    // UI Constants
    constexpr int MIN_OBJECT_ID = 1;
    constexpr int MAX_OBJECT_ID = 100;
    constexpr int DEFAULT_OBJECT_ID = 1;
    constexpr int PLOT_MIN_HEIGHT = 200;

    // Data generation constants
    constexpr int NUM_DATA_POINTS = 100;
    constexpr int NUM_SERIES = 3;
    constexpr int NUM_METRICS = 3;

    // Visual constants
    constexpr int LINE_WIDTH = 2;
    constexpr int TRACER_SIZE = 8;
    constexpr int CALLOUT_FONT_SIZE = 10;
    constexpr int CALLOUT_ALPHA = 200;  // Semi-transparent background

    // Series colors
    const QColor SERIES_COLORS[NUM_SERIES] = {
        Qt::green,
        Qt::yellow,
        Qt::red
    };

    // Metric labels
    const QString METRIC_LABELS[NUM_METRICS] = {
        "Metric A",
        "Metric B",
        "Metric C"
    };
}

// Data structures for time series
struct DataPoint {
    double time;
    double series1Value;
    double series2Value;
    double series3Value;

    DataPoint() : time(0.0), series1Value(0.0), series2Value(0.0), series3Value(0.0) {}
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
    QCustomPlot *plotMetricA;
    QCustomPlot *plotMetricB;
    QCustomPlot *plotMetricC;

    // Callout components for each plot
    struct PlotCallout {
        QCPItemTracer *tracers[Config::NUM_SERIES];
        QCPItemText *textLabel;

        PlotCallout() : textLabel(nullptr) {
            for (int i = 0; i < Config::NUM_SERIES; ++i) {
                tracers[i] = nullptr;
            }
        }
    };

    PlotCallout calloutMetricA;
    PlotCallout calloutMetricB;
    PlotCallout calloutMetricC;

    // Helper methods for setup
    void setupUI();
    void setupPlots();
    void setupSinglePlot(QCustomPlot *plot, const QString &metricLabel,
                         const QString units[], PlotCallout &callout);
    void setupCallouts(QCustomPlot *plot, PlotCallout &callout);
    QCPItemTracer* createTracer(QCustomPlot *plot, const QColor &color);

    // Data methods
    ObjectData generateFakeData(int objectId) const;
    void updatePlot(QCustomPlot *plot, const TimeSeriesData &data);
    void updateGraphs(int objectId);
    void showCallout(QCustomPlot *plot, PlotCallout &callout, double xCoord);
};

#endif // DATAMONITORWIDGET_H
