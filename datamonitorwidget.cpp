#include "datamonitorwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QtMath>
#include <QRandomGenerator>

DataMonitorWidget::DataMonitorWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupPlots();
    updateGraphs(Config::DEFAULT_OBJECT_ID);
}

DataMonitorWidget::~DataMonitorWidget()
{
    // Qt automatically deletes child widgets
}

void DataMonitorWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Control panel
    QHBoxLayout *controlLayout = new QHBoxLayout();
    objectIdLabel = new QLabel("Select Object ID:", this);
    objectIdSpinBox = new QSpinBox(this);
    objectIdSpinBox->setMinimum(Config::MIN_OBJECT_ID);
    objectIdSpinBox->setMaximum(Config::MAX_OBJECT_ID);
    objectIdSpinBox->setValue(Config::DEFAULT_OBJECT_ID);
    objectIdSpinBox->setPrefix("Object #");

    controlLayout->addWidget(objectIdLabel);
    controlLayout->addWidget(objectIdSpinBox);
    controlLayout->addStretch();

    mainLayout->addLayout(controlLayout);

    // Create plot widgets
    plotMetricA = new QCustomPlot(this);
    plotMetricB = new QCustomPlot(this);
    plotMetricC = new QCustomPlot(this);

    plotMetricA->setMinimumHeight(Config::PLOT_MIN_HEIGHT);
    plotMetricB->setMinimumHeight(Config::PLOT_MIN_HEIGHT);
    plotMetricC->setMinimumHeight(Config::PLOT_MIN_HEIGHT);

    mainLayout->addWidget(plotMetricA);
    mainLayout->addWidget(plotMetricB);
    mainLayout->addWidget(plotMetricC);

    setLayout(mainLayout);

    // Connect signals
    connect(objectIdSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &DataMonitorWidget::onObjectIdChanged);
}

void DataMonitorWidget::setupPlots()
{
    const QString unitsA[] = {"Unit A", "Unit B", "Unit C"};
    const QString unitsB[] = {"Unit D", "Unit E", "Unit F"};
    const QString unitsC[] = {"Unit G", "Unit H", "Unit I"};

    setupSinglePlot(plotMetricA, Config::METRIC_LABELS[0], unitsA, calloutMetricA);
    setupSinglePlot(plotMetricB, Config::METRIC_LABELS[1], unitsB, calloutMetricB);
    setupSinglePlot(plotMetricC, Config::METRIC_LABELS[2], unitsC, calloutMetricC);
}

void DataMonitorWidget::setupSinglePlot(QCustomPlot *plot, const QString &metricLabel,
                                         const QString units[], PlotCallout &callout)
{
    // Add graphs for each series
    for (int i = 0; i < Config::NUM_SERIES; ++i) {
        plot->addGraph();
        plot->graph(i)->setPen(QPen(Config::SERIES_COLORS[i], Config::LINE_WIDTH));
        plot->graph(i)->setName(QString("Series %1 (%2)").arg(i + 1).arg(units[i]));
    }

    // Configure axes and interactions
    plot->xAxis->setLabel("Time");
    plot->yAxis->setLabel(metricLabel);
    plot->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    plot->legend->setVisible(true);
    plot->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop | Qt::AlignRight);

    // Setup callouts
    setupCallouts(plot, callout);

    // Connect mouse clicks
    connect(plot, &QCustomPlot::mousePress, this, &DataMonitorWidget::onPlotClicked);
}

void DataMonitorWidget::setupCallouts(QCustomPlot *plot, PlotCallout &callout)
{
    // Create tracers for each series
    for (int i = 0; i < Config::NUM_SERIES; ++i) {
        callout.tracers[i] = createTracer(plot, Config::SERIES_COLORS[i]);
    }

    // Create text label
    callout.textLabel = new QCPItemText(plot);
    callout.textLabel->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
    callout.textLabel->position->setType(QCPItemPosition::ptPlotCoords);
    callout.textLabel->setFont(QFont(font().family(), Config::CALLOUT_FONT_SIZE));
    callout.textLabel->setPen(QPen(Qt::black));
    callout.textLabel->setBrush(QBrush(QColor(255, 255, 255, Config::CALLOUT_ALPHA)));
    callout.textLabel->setPadding(QMargins(5, 5, 5, 5));
    callout.textLabel->setVisible(false);
}

QCPItemTracer* DataMonitorWidget::createTracer(QCustomPlot *plot, const QColor &color)
{
    QCPItemTracer *tracer = new QCPItemTracer(plot);
    tracer->setInterpolating(true);
    tracer->setStyle(QCPItemTracer::tsCircle);
    tracer->setPen(QPen(color, Config::LINE_WIDTH));
    tracer->setBrush(color);
    tracer->setSize(Config::TRACER_SIZE);
    tracer->setVisible(false);
    return tracer;
}

ObjectData DataMonitorWidget::generateFakeData(int objectId) const
{
    ObjectData data;

    // Pre-allocate vectors
    data.metricA.dataPoints.resize(Config::NUM_DATA_POINTS);
    data.metricB.dataPoints.resize(Config::NUM_DATA_POINTS);
    data.metricC.dataPoints.resize(Config::NUM_DATA_POINTS);

    // Base values vary by object ID
    const double metricABase = 20.0 + (objectId % 10) * 2.0;
    const double metricBBase = 50.0 + (objectId % 10) * 3.0;
    const double metricCBase = 1013.0 + (objectId % 10) * 5.0;

    for (int i = 0; i < Config::NUM_DATA_POINTS; ++i) {
        const double time = static_cast<double>(i);
        const double noise = QRandomGenerator::global()->bounded(100) / 100.0 - 0.5;

        // Generate Metric A data
        DataPoint pointA;
        pointA.time = time;
        pointA.series1Value = metricABase + 5.0 * qSin(i * 0.1 + objectId) + noise;
        pointA.series2Value = metricABase + 3.0 + 6.0 * qSin(i * 0.12 + objectId + 1.0) + noise;
        pointA.series3Value = metricABase - 2.0 + 4.0 * qSin(i * 0.08 + objectId + 2.0) + noise;
        data.metricA.dataPoints[i] = pointA;

        // Generate Metric B data
        DataPoint pointB;
        pointB.time = time;
        pointB.series1Value = metricBBase + 10.0 * qCos(i * 0.15 + objectId) + noise;
        pointB.series2Value = metricBBase + 5.0 + 8.0 * qCos(i * 0.18 + objectId + 1.5) + noise;
        pointB.series3Value = metricBBase - 3.0 + 12.0 * qCos(i * 0.12 + objectId + 2.5) + noise;
        data.metricB.dataPoints[i] = pointB;

        // Generate Metric C data
        DataPoint pointC;
        pointC.time = time;
        pointC.series1Value = metricCBase + 8.0 * qSin(i * 0.05 + objectId) + noise;
        pointC.series2Value = metricCBase + 10.0 + 6.0 * qSin(i * 0.06 + objectId + 1.0) + noise;
        pointC.series3Value = metricCBase - 5.0 + 7.0 * qSin(i * 0.04 + objectId + 2.0) + noise;
        data.metricC.dataPoints[i] = pointC;
    }

    return data;
}

void DataMonitorWidget::updatePlot(QCustomPlot *plot, const TimeSeriesData &data)
{
    const int numPoints = data.dataPoints.size();
    QVector<double> time(numPoints);
    QVector<double> series1(numPoints);
    QVector<double> series2(numPoints);
    QVector<double> series3(numPoints);

    // Extract data into separate vectors
    for (int i = 0; i < numPoints; ++i) {
        const DataPoint &point = data.dataPoints[i];
        time[i] = point.time;
        series1[i] = point.series1Value;
        series2[i] = point.series2Value;
        series3[i] = point.series3Value;
    }

    // Update graphs
    plot->graph(0)->setData(time, series1);
    plot->graph(1)->setData(time, series2);
    plot->graph(2)->setData(time, series3);

    // Update axis ranges
    plot->xAxis->setRange(0, numPoints);
    plot->yAxis->rescale();
    plot->replot();
}

void DataMonitorWidget::updateGraphs(int objectId)
{
    const ObjectData data = generateFakeData(objectId);

    updatePlot(plotMetricA, data.metricA);
    updatePlot(plotMetricB, data.metricB);
    updatePlot(plotMetricC, data.metricC);
}

void DataMonitorWidget::showCallout(QCustomPlot *plot, PlotCallout &callout, double xCoord)
{
    // Lazy initialization: associate tracers with graphs on first use
    if (!callout.tracers[0]->graph()) {
        for (int i = 0; i < Config::NUM_SERIES; ++i) {
            callout.tracers[i]->setGraph(plot->graph(i));
        }
    }

    // Position tracers and get values
    double values[Config::NUM_SERIES];
    for (int i = 0; i < Config::NUM_SERIES; ++i) {
        callout.tracers[i]->setGraphKey(xCoord);
        values[i] = callout.tracers[i]->position->value();
        callout.tracers[i]->setVisible(true);
    }

    // Format callout text
    QString text = QString("Time: %1\nSeries 1: %2\nSeries 2: %3\nSeries 3: %4")
                       .arg(xCoord, 0, 'f', 1)
                       .arg(values[0], 0, 'f', 2)
                       .arg(values[1], 0, 'f', 2)
                       .arg(values[2], 0, 'f', 2);

    callout.textLabel->setText(text);
    callout.textLabel->position->setCoords(xCoord, values[1]);  // Position at middle series
    callout.textLabel->setVisible(true);

    plot->replot();
}

void DataMonitorWidget::onPlotClicked(QMouseEvent *event)
{
    QCustomPlot *plot = qobject_cast<QCustomPlot*>(sender());
    if (!plot) return;

    const double xCoord = plot->xAxis->pixelToCoord(event->pos().x());

    // Show callout on the clicked plot
    if (plot == plotMetricA) {
        showCallout(plotMetricA, calloutMetricA, xCoord);
    } else if (plot == plotMetricB) {
        showCallout(plotMetricB, calloutMetricB, xCoord);
    } else if (plot == plotMetricC) {
        showCallout(plotMetricC, calloutMetricC, xCoord);
    }
}

void DataMonitorWidget::onObjectIdChanged(int objectId)
{
    updateGraphs(objectId);
}
