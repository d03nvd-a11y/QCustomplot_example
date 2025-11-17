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

void MainWindow::generateFakeData(int objectId, QVector<double> &time,
                                  QVector<double> &temp1, QVector<double> &temp2, QVector<double> &temp3,
                                  QVector<double> &hum1, QVector<double> &hum2, QVector<double> &hum3,
                                  QVector<double> &press1, QVector<double> &press2, QVector<double> &press3)
{
    const int numPoints = 100;
    time.resize(numPoints);
    temp1.resize(numPoints);
    temp2.resize(numPoints);
    temp3.resize(numPoints);
    hum1.resize(numPoints);
    hum2.resize(numPoints);
    hum3.resize(numPoints);
    press1.resize(numPoints);
    press2.resize(numPoints);
    press3.resize(numPoints);

    // Use objectId as seed for variation
    double tempBase = 20.0 + (objectId % 10) * 2.0;      // Base temperature varies by object
    double humidBase = 50.0 + (objectId % 10) * 3.0;     // Base humidity varies by object
    double pressBase = 1013.0 + (objectId % 10) * 5.0;   // Base pressure varies by object

    for (int i = 0; i < numPoints; ++i)
    {
        time[i] = i;

        // Generate temperature series with different patterns
        temp1[i] = tempBase + 5.0 * qSin(i * 0.1 + objectId) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        temp2[i] = tempBase + 3.0 + 6.0 * qSin(i * 0.12 + objectId + 1.0) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        temp3[i] = tempBase - 2.0 + 4.0 * qSin(i * 0.08 + objectId + 2.0) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;

        // Generate humidity series with different patterns
        hum1[i] = humidBase + 10.0 * qCos(i * 0.15 + objectId) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        hum2[i] = humidBase + 5.0 + 8.0 * qCos(i * 0.18 + objectId + 1.5) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        hum3[i] = humidBase - 3.0 + 12.0 * qCos(i * 0.12 + objectId + 2.5) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;

        // Generate pressure series with different patterns
        press1[i] = pressBase + 8.0 * qSin(i * 0.05 + objectId) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        press2[i] = pressBase + 10.0 + 6.0 * qSin(i * 0.06 + objectId + 1.0) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        press3[i] = pressBase - 5.0 + 7.0 * qSin(i * 0.04 + objectId + 2.0) + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
    }
}

void MainWindow::updateGraphs(int objectId)
{
    QVector<double> time, temp1, temp2, temp3, hum1, hum2, hum3, press1, press2, press3;

    // Generate fake data for the selected object
    generateFakeData(objectId, time, temp1, temp2, temp3, hum1, hum2, hum3, press1, press2, press3);

    // Update Temperature Graphs (3 series)
    plotTemperature->graph(0)->setData(time, temp1);  // Green series
    plotTemperature->graph(1)->setData(time, temp2);  // Yellow series
    plotTemperature->graph(2)->setData(time, temp3);  // Red series
    plotTemperature->xAxis->setRange(0, time.size());
    plotTemperature->yAxis->rescale();
    plotTemperature->replot();

    // Update Humidity Graphs (3 series)
    plotHumidity->graph(0)->setData(time, hum1);  // Green series
    plotHumidity->graph(1)->setData(time, hum2);  // Yellow series
    plotHumidity->graph(2)->setData(time, hum3);  // Red series
    plotHumidity->xAxis->setRange(0, time.size());
    plotHumidity->yAxis->rescale();
    plotHumidity->replot();

    // Update Pressure Graphs (3 series)
    plotPressure->graph(0)->setData(time, press1);  // Green series
    plotPressure->graph(1)->setData(time, press2);  // Yellow series
    plotPressure->graph(2)->setData(time, press3);  // Red series
    plotPressure->xAxis->setRange(0, time.size());
    plotPressure->yAxis->rescale();
    plotPressure->replot();
}

void MainWindow::onObjectIdChanged(int objectId)
{
    updateGraphs(objectId);
}
