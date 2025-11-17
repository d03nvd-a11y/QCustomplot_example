#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QtMath>
#include <QDateTime>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("Multi-Series Data Monitor");
    resize(1200, 900);

    setupUI();
    setupPlots();

    // Initialize with object ID 1
    updateGraphs(1);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // Main layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Control panel at the top
    QHBoxLayout *controlLayout = new QHBoxLayout();
    objectIdLabel = new QLabel("Select Object ID:", this);
    objectIdSpinBox = new QSpinBox(this);
    objectIdSpinBox->setMinimum(1);
    objectIdSpinBox->setMaximum(100);
    objectIdSpinBox->setValue(1);
    objectIdSpinBox->setPrefix("Object #");

    controlLayout->addWidget(objectIdLabel);
    controlLayout->addWidget(objectIdSpinBox);
    controlLayout->addStretch();

    mainLayout->addLayout(controlLayout);

    // Create three plot widgets
    plotTemperature = new QCustomPlot(this);
    plotHumidity = new QCustomPlot(this);
    plotPressure = new QCustomPlot(this);

    // Set minimum heights for plots
    plotTemperature->setMinimumHeight(200);
    plotHumidity->setMinimumHeight(200);
    plotPressure->setMinimumHeight(200);

    // Add plots to layout
    mainLayout->addWidget(plotTemperature);
    mainLayout->addWidget(plotHumidity);
    mainLayout->addWidget(plotPressure);

    setLayout(mainLayout);

    // Connect spinbox signal
    connect(objectIdSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onObjectIdChanged);
}

void MainWindow::setupPlots()
{
    // Setup Temperature Plot with 3 series
    plotTemperature->addGraph(); // Series 1 - Green
    plotTemperature->graph(0)->setPen(QPen(Qt::green, 2));
    plotTemperature->graph(0)->setName("Series 1 (Unit A)");

    plotTemperature->addGraph(); // Series 2 - Yellow
    plotTemperature->graph(1)->setPen(QPen(Qt::yellow, 2));
    plotTemperature->graph(1)->setName("Series 2 (Unit B)");

    plotTemperature->addGraph(); // Series 3 - Red
    plotTemperature->graph(2)->setPen(QPen(Qt::red, 2));
    plotTemperature->graph(2)->setName("Series 3 (Unit C)");

    plotTemperature->xAxis->setLabel("Time");
    plotTemperature->yAxis->setLabel("Metric A");
    plotTemperature->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    plotTemperature->legend->setVisible(true);
    plotTemperature->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);

    // Setup Humidity Plot with 3 series
    plotHumidity->addGraph(); // Series 1 - Green
    plotHumidity->graph(0)->setPen(QPen(Qt::green, 2));
    plotHumidity->graph(0)->setName("Series 1 (Unit D)");

    plotHumidity->addGraph(); // Series 2 - Yellow
    plotHumidity->graph(1)->setPen(QPen(Qt::yellow, 2));
    plotHumidity->graph(1)->setName("Series 2 (Unit E)");

    plotHumidity->addGraph(); // Series 3 - Red
    plotHumidity->graph(2)->setPen(QPen(Qt::red, 2));
    plotHumidity->graph(2)->setName("Series 3 (Unit F)");

    plotHumidity->xAxis->setLabel("Time");
    plotHumidity->yAxis->setLabel("Metric B");
    plotHumidity->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    plotHumidity->legend->setVisible(true);
    plotHumidity->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);

    // Setup Pressure Plot with 3 series
    plotPressure->addGraph(); // Series 1 - Green
    plotPressure->graph(0)->setPen(QPen(Qt::green, 2));
    plotPressure->graph(0)->setName("Series 1 (Unit G)");

    plotPressure->addGraph(); // Series 2 - Yellow
    plotPressure->graph(1)->setPen(QPen(Qt::yellow, 2));
    plotPressure->graph(1)->setName("Series 2 (Unit H)");

    plotPressure->addGraph(); // Series 3 - Red
    plotPressure->graph(2)->setPen(QPen(Qt::red, 2));
    plotPressure->graph(2)->setName("Series 3 (Unit I)");

    plotPressure->xAxis->setLabel("Time");
    plotPressure->yAxis->setLabel("Metric C");
    plotPressure->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    plotPressure->legend->setVisible(true);
    plotPressure->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);
}

ObjectData MainWindow::generateFakeData(int objectId)
{
    ObjectData data;
    const int numPoints = 100;

    // Resize all vectors
    data.metricA.time.resize(numPoints);
    data.metricA.series1.resize(numPoints);
    data.metricA.series2.resize(numPoints);
    data.metricA.series3.resize(numPoints);

    data.metricB.time.resize(numPoints);
    data.metricB.series1.resize(numPoints);
    data.metricB.series2.resize(numPoints);
    data.metricB.series3.resize(numPoints);

    data.metricC.time.resize(numPoints);
    data.metricC.series1.resize(numPoints);
    data.metricC.series2.resize(numPoints);
    data.metricC.series3.resize(numPoints);

    // Use objectId as seed for variation
    double metricABase = 20.0 + (objectId % 10) * 2.0;
    double metricBBase = 50.0 + (objectId % 10) * 3.0;
    double metricCBase = 1013.0 + (objectId % 10) * 5.0;

    for (int i = 0; i < numPoints; ++i)
    {
        // Common time axis for all metrics
        double timeValue = i;
        data.metricA.time[i] = timeValue;
        data.metricB.time[i] = timeValue;
        data.metricC.time[i] = timeValue;

        // Generate Metric A series with different patterns
        data.metricA.series1[i] = metricABase + 5.0 * qSin(i * 0.1 + objectId) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        data.metricA.series2[i] = metricABase + 3.0 + 6.0 * qSin(i * 0.12 + objectId + 1.0) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        data.metricA.series3[i] = metricABase - 2.0 + 4.0 * qSin(i * 0.08 + objectId + 2.0) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;

        // Generate Metric B series with different patterns
        data.metricB.series1[i] = metricBBase + 10.0 * qCos(i * 0.15 + objectId) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        data.metricB.series2[i] = metricBBase + 5.0 + 8.0 * qCos(i * 0.18 + objectId + 1.5) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        data.metricB.series3[i] = metricBBase - 3.0 + 12.0 * qCos(i * 0.12 + objectId + 2.5) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;

        // Generate Metric C series with different patterns
        data.metricC.series1[i] = metricCBase + 8.0 * qSin(i * 0.05 + objectId) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        data.metricC.series2[i] = metricCBase + 10.0 + 6.0 * qSin(i * 0.06 + objectId + 1.0) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        data.metricC.series3[i] = metricCBase - 5.0 + 7.0 * qSin(i * 0.04 + objectId + 2.0) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
    }

    return data;
}

void MainWindow::updatePlot(QCustomPlot *plot, const TimeSeriesData &data)
{
    // Update all 3 series
    plot->graph(0)->setData(data.time, data.series1);
    plot->graph(1)->setData(data.time, data.series2);
    plot->graph(2)->setData(data.time, data.series3);

    // Update axis ranges
    plot->xAxis->setRange(0, data.time.size());
    plot->yAxis->rescale();

    // Replot
    plot->replot();
}

void MainWindow::updateGraphs(int objectId)
{
    // Generate fake data for the selected object
    ObjectData data = generateFakeData(objectId);

    // Update all plots using the structured data
    updatePlot(plotTemperature, data.metricA);
    updatePlot(plotHumidity, data.metricB);
    updatePlot(plotPressure, data.metricC);
}

void MainWindow::onObjectIdChanged(int objectId)
{
    updateGraphs(objectId);
}
