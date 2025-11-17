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
    setWindowTitle("Multi-Object Time Series Monitor");
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
    // Setup Temperature Plot
    plotTemperature->addGraph();
    plotTemperature->graph(0)->setPen(QPen(Qt::red, 2));
    plotTemperature->graph(0)->setBrush(QBrush(QColor(255, 0, 0, 20)));
    plotTemperature->xAxis->setLabel("Time (seconds)");
    plotTemperature->yAxis->setLabel("Temperature (°C)");
    plotTemperature->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // Setup Humidity Plot
    plotHumidity->addGraph();
    plotHumidity->graph(0)->setPen(QPen(Qt::blue, 2));
    plotHumidity->graph(0)->setBrush(QBrush(QColor(0, 0, 255, 20)));
    plotHumidity->xAxis->setLabel("Time (seconds)");
    plotHumidity->yAxis->setLabel("Humidity (%)");
    plotHumidity->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);

    // Setup Pressure Plot
    plotPressure->addGraph();
    plotPressure->graph(0)->setPen(QPen(Qt::green, 2));
    plotPressure->graph(0)->setBrush(QBrush(QColor(0, 255, 0, 20)));
    plotPressure->xAxis->setLabel("Time (seconds)");
    plotPressure->yAxis->setLabel("Pressure (hPa)");
    plotPressure->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
}

void MainWindow::generateFakeData(int objectId, QVector<double> &time,
                                  QVector<double> &temperature,
                                  QVector<double> &humidity,
                                  QVector<double> &pressure)
{
    const int numPoints = 100;
    time.resize(numPoints);
    temperature.resize(numPoints);
    humidity.resize(numPoints);
    pressure.resize(numPoints);

    // Use objectId as seed for variation
    double tempBase = 20.0 + (objectId % 10) * 2.0;      // Base temperature varies by object
    double humidBase = 50.0 + (objectId % 10) * 3.0;     // Base humidity varies by object
    double pressBase = 1013.0 + (objectId % 10) * 5.0;   // Base pressure varies by object

    for (int i = 0; i < numPoints; ++i)
    {
        time[i] = i;

        // Generate temperature with sinusoidal pattern + noise
        temperature[i] = tempBase + 5.0 * qSin(i * 0.1 + objectId) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;

        // Generate humidity with different frequency pattern
        humidity[i] = humidBase + 10.0 * qCos(i * 0.15 + objectId) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;

        // Generate pressure with slow variation
        pressure[i] = pressBase + 8.0 * qSin(i * 0.05 + objectId) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
    }
}

void MainWindow::updateGraphs(int objectId)
{
    QVector<double> time, temperature, humidity, pressure;

    // Generate fake data for the selected object
    generateFakeData(objectId, time, temperature, humidity, pressure);

    // Update Temperature Graph
    plotTemperature->graph(0)->setData(time, temperature);
    plotTemperature->xAxis->setRange(0, time.size());
    plotTemperature->yAxis->rescale();
    plotTemperature->replot();

    // Update Humidity Graph
    plotHumidity->graph(0)->setData(time, humidity);
    plotHumidity->xAxis->setRange(0, time.size());
    plotHumidity->yAxis->rescale();
    plotHumidity->replot();

    // Update Pressure Graph
    plotPressure->graph(0)->setData(time, pressure);
    plotPressure->xAxis->setRange(0, time.size());
    plotPressure->yAxis->rescale();
    plotPressure->replot();
}

void MainWindow::onObjectIdChanged(int objectId)
{
    updateGraphs(objectId);
}
