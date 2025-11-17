# QCustomPlot Multi-Series Data Monitor Widget

A comprehensive guide to building a reusable Qt widget for displaying multi-series time-series data using QCustomPlot.

## Table of Contents

- [Overview](#overview)
- [Prerequisites](#prerequisites)
- [Project Setup](#project-setup)
- [Step-by-Step Implementation](#step-by-step-implementation)
  - [Step 1: Define Data Structures](#step-1-define-data-structures)
  - [Step 2: Create the Widget Header](#step-2-create-the-widget-header)
  - [Step 3: Implement UI Setup](#step-3-implement-ui-setup)
  - [Step 4: Configure Plot Settings](#step-4-configure-plot-settings)
  - [Step 5: Implement Data Generation](#step-5-implement-data-generation)
  - [Step 6: Update Plot with Data](#step-6-update-plot-with-data)
  - [Step 7: Add Interactive Callouts](#step-7-add-interactive-callouts)
  - [Step 8: Create Main Application](#step-8-create-main-application)
  - [Step 9: Configure CMake Build](#step-9-configure-cmake-build)
- [Building the Project](#building-the-project)
- [Features](#features)
- [Usage](#usage)
- [Customization](#customization)

## Overview

This widget displays three separate charts, each showing three time-series with different colors (green, yellow, red). Users can:
- Select different object IDs (1-100) to display different datasets
- Click on charts to see callouts showing values at specific time points
- Zoom and drag to navigate the data
- View legends identifying each series

## Prerequisites

- **Qt5** (Core, Gui, Widgets, PrintSupport modules)
- **CMake** 3.10 or higher
- **C++11** compatible compiler
- **QCustomPlot** library (included in this project)

### Installing Qt5

**Ubuntu/Debian:**
```bash
sudo apt-get install qt5-default qtbase5-dev
```

**macOS:**
```bash
brew install qt@5
```

**Windows:**
Download and install Qt from [qt.io](https://www.qt.io/download)

## Project Setup

Create a new directory for your project:

```bash
mkdir QCustomplot_example
cd QCustomplot_example
```

Download QCustomPlot from [qcustomplot.com](https://www.qcustomplot.com/index.php/download) and place `qcustomplot.h` and `qcustomplot.cpp` in your project directory.

## Step-by-Step Implementation

### Step 1: Define Data Structures

First, we define clear data structures to represent our time-series data. This makes the code extensible and easy to maintain.

**In `datamonitorwidget.h`:**

```cpp
// Data structures for time series
struct DataPoint {
    double time;           // X-axis value (time)
    double series1Value;   // Y-axis value for series 1
    double series2Value;   // Y-axis value for series 2
    double series3Value;   // Y-axis value for series 3
};

struct TimeSeriesData {
    QVector<DataPoint> dataPoints;  // Collection of data points
};

struct ObjectData {
    TimeSeriesData metricA;  // Data for first chart
    TimeSeriesData metricB;  // Data for second chart
    TimeSeriesData metricC;  // Data for third chart
};
```

**Why this structure?**
- `DataPoint`: Represents a single point in time with values for all three series
- `TimeSeriesData`: A collection of points forming a complete time series
- `ObjectData`: Groups all three metrics for a single object ID
- This hierarchical structure makes it easy to add more metrics or series later

### Step 2: Create the Widget Header

Define the widget class with all necessary components.

**Complete `datamonitorwidget.h`:**

```cpp
#ifndef DATAMONITORWIDGET_H
#define DATAMONITORWIDGET_H

#include <QWidget>
#include <QSpinBox>
#include <QLabel>
#include "qcustomplot.h"

// [Data structures from Step 1 go here]

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
    QCustomPlot *plotTemperature;  // First chart (Metric A)
    QCustomPlot *plotHumidity;     // Second chart (Metric B)
    QCustomPlot *plotPressure;     // Third chart (Metric C)

    // Callout components for each plot
    struct PlotCallout {
        QCPItemTracer *tracer1;    // Marker for series 1
        QCPItemTracer *tracer2;    // Marker for series 2
        QCPItemTracer *tracer3;    // Marker for series 3
        QCPItemText *textLabel;    // Text box showing values
    };

    PlotCallout calloutTemperature;
    PlotCallout calloutHumidity;
    PlotCallout calloutPressure;

    // Methods
    ObjectData generateFakeData(int objectId);
    void updatePlot(QCustomPlot *plot, const TimeSeriesData &data);
    void setupUI();
    void setupPlots();
    void setupCallouts(QCustomPlot *plot, PlotCallout &callout);
    void updateGraphs(int objectId);
    void showCallout(QCustomPlot *plot, PlotCallout &callout, double xCoord);
};

#endif // DATAMONITORWIDGET_H
```

**Key points:**
- `Q_OBJECT` macro enables Qt's signals and slots
- `explicit` constructor prevents implicit conversions
- Private slots handle user interactions
- PlotCallout struct groups all callout-related items for one plot

### Step 3: Implement UI Setup

Create the user interface layout with control panel and three plots.

**In `datamonitorwidget.cpp`:**

```cpp
#include "datamonitorwidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QtMath>
#include <QDateTime>
#include <QRandomGenerator>

DataMonitorWidget::DataMonitorWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
    setupPlots();
    updateGraphs(1);  // Initialize with object ID 1
}

DataMonitorWidget::~DataMonitorWidget()
{
    // Qt handles cleanup of child widgets automatically
}

void DataMonitorWidget::setupUI()
{
    // Create main vertical layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Control panel at the top
    QHBoxLayout *controlLayout = new QHBoxLayout();

    // Label for spinbox
    objectIdLabel = new QLabel("Select Object ID:", this);

    // Spinbox for object selection
    objectIdSpinBox = new QSpinBox(this);
    objectIdSpinBox->setMinimum(1);
    objectIdSpinBox->setMaximum(100);
    objectIdSpinBox->setValue(1);
    objectIdSpinBox->setPrefix("Object #");

    // Add controls to horizontal layout
    controlLayout->addWidget(objectIdLabel);
    controlLayout->addWidget(objectIdSpinBox);
    controlLayout->addStretch();  // Push controls to the left

    mainLayout->addLayout(controlLayout);

    // Create three plot widgets
    plotTemperature = new QCustomPlot(this);
    plotHumidity = new QCustomPlot(this);
    plotPressure = new QCustomPlot(this);

    // Set minimum heights so plots don't get too small
    plotTemperature->setMinimumHeight(200);
    plotHumidity->setMinimumHeight(200);
    plotPressure->setMinimumHeight(200);

    // Add plots to main layout
    mainLayout->addWidget(plotTemperature);
    mainLayout->addWidget(plotHumidity);
    mainLayout->addWidget(plotPressure);

    setLayout(mainLayout);

    // Connect spinbox value changes to update function
    connect(objectIdSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &DataMonitorWidget::onObjectIdChanged);
}
```

**Layout explanation:**
- `QVBoxLayout`: Stacks widgets vertically (control panel, then 3 plots)
- `QHBoxLayout`: Arranges controls horizontally
- `addStretch()`: Adds flexible space to push widgets to one side
- `connect()`: Links spinbox changes to our update function

### Step 4: Configure Plot Settings

Set up each plot with three series, colors, labels, and interactivity.

**In `datamonitorwidget.cpp`:**

```cpp
void DataMonitorWidget::setupPlots()
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

    // Configure axes and interactions
    plotTemperature->xAxis->setLabel("Time");
    plotTemperature->yAxis->setLabel("Metric A");
    plotTemperature->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    plotTemperature->legend->setVisible(true);
    plotTemperature->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);

    // Connect mouse clicks to callout handler
    connect(plotTemperature, &QCustomPlot::mousePress, this, &DataMonitorWidget::onPlotClicked);

    // Repeat for plotHumidity with Units D, E, F and Metric B
    plotHumidity->addGraph();
    plotHumidity->graph(0)->setPen(QPen(Qt::green, 2));
    plotHumidity->graph(0)->setName("Series 1 (Unit D)");

    plotHumidity->addGraph();
    plotHumidity->graph(1)->setPen(QPen(Qt::yellow, 2));
    plotHumidity->graph(1)->setName("Series 2 (Unit E)");

    plotHumidity->addGraph();
    plotHumidity->graph(2)->setPen(QPen(Qt::red, 2));
    plotHumidity->graph(2)->setName("Series 3 (Unit F)");

    plotHumidity->xAxis->setLabel("Time");
    plotHumidity->yAxis->setLabel("Metric B");
    plotHumidity->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    plotHumidity->legend->setVisible(true);
    plotHumidity->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);
    connect(plotHumidity, &QCustomPlot::mousePress, this, &DataMonitorWidget::onPlotClicked);

    // Repeat for plotPressure with Units G, H, I and Metric C
    plotPressure->addGraph();
    plotPressure->graph(0)->setPen(QPen(Qt::green, 2));
    plotPressure->graph(0)->setName("Series 1 (Unit G)");

    plotPressure->addGraph();
    plotPressure->graph(1)->setPen(QPen(Qt::yellow, 2));
    plotPressure->graph(1)->setName("Series 2 (Unit H)");

    plotPressure->addGraph();
    plotPressure->graph(2)->setPen(QPen(Qt::red, 2));
    plotPressure->graph(2)->setName("Series 3 (Unit I)");

    plotPressure->xAxis->setLabel("Time");
    plotPressure->yAxis->setLabel("Metric C");
    plotPressure->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom);
    plotPressure->legend->setVisible(true);
    plotPressure->axisRect()->insetLayout()->setInsetAlignment(0, Qt::AlignTop|Qt::AlignRight);
    connect(plotPressure, &QCustomPlot::mousePress, this, &DataMonitorWidget::onPlotClicked);

    // Setup callouts for all plots
    setupCallouts(plotTemperature, calloutTemperature);
    setupCallouts(plotHumidity, calloutHumidity);
    setupCallouts(plotPressure, calloutPressure);
}
```

**Plot configuration details:**
- `addGraph()`: Creates a new graph/series on the plot
- `setPen()`: Sets line color and width (2 pixels)
- `setName()`: Sets legend text
- `setInteractions()`: Enables drag and zoom (NOT selection to prevent color changes)
- `legend->setVisible(true)`: Shows the legend
- `insetLayout()->setInsetAlignment()`: Positions legend in top-right corner

### Step 5: Implement Data Generation

Generate fake sinusoidal data that varies based on object ID.

**In `datamonitorwidget.cpp`:**

```cpp
ObjectData DataMonitorWidget::generateFakeData(int objectId)
{
    ObjectData data;
    const int numPoints = 100;

    // Pre-allocate vectors for efficiency
    data.metricA.dataPoints.resize(numPoints);
    data.metricB.dataPoints.resize(numPoints);
    data.metricC.dataPoints.resize(numPoints);

    // Use objectId to create variation between different objects
    double metricABase = 20.0 + (objectId % 10) * 2.0;
    double metricBBase = 50.0 + (objectId % 10) * 3.0;
    double metricCBase = 1013.0 + (objectId % 10) * 5.0;

    for (int i = 0; i < numPoints; ++i)
    {
        double timeValue = i;

        // Create Metric A data point with sinusoidal patterns
        DataPoint pointA;
        pointA.time = timeValue;
        pointA.series1Value = metricABase + 5.0 * qSin(i * 0.1 + objectId)
                              + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        pointA.series2Value = metricABase + 3.0 + 6.0 * qSin(i * 0.12 + objectId + 1.0)
                              + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        pointA.series3Value = metricABase - 2.0 + 4.0 * qSin(i * 0.08 + objectId + 2.0)
                              + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        data.metricA.dataPoints[i] = pointA;

        // Create Metric B data point with cosine patterns
        DataPoint pointB;
        pointB.time = timeValue;
        pointB.series1Value = metricBBase + 10.0 * qCos(i * 0.15 + objectId)
                              + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        pointB.series2Value = metricBBase + 5.0 + 8.0 * qCos(i * 0.18 + objectId + 1.5)
                              + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        pointB.series3Value = metricBBase - 3.0 + 12.0 * qCos(i * 0.12 + objectId + 2.5)
                              + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        data.metricB.dataPoints[i] = pointB;

        // Create Metric C data point
        DataPoint pointC;
        pointC.time = timeValue;
        pointC.series1Value = metricCBase + 8.0 * qSin(i * 0.05 + objectId)
                              + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        pointC.series2Value = metricCBase + 10.0 + 6.0 * qSin(i * 0.06 + objectId + 1.0)
                              + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        pointC.series3Value = metricCBase - 5.0 + 7.0 * qSin(i * 0.04 + objectId + 2.0)
                              + (QRandomGenerator::global()->bounded(100)) / 100.0 - 0.5;
        data.metricC.dataPoints[i] = pointC;
    }

    return data;
}
```

**Data generation explained:**
- Each object ID gets a different base value using modulo (`objectId % 10`)
- Sinusoidal patterns (`qSin`, `qCos`) create smooth, wave-like data
- Different frequencies (0.1, 0.12, 0.08) make each series distinct
- Random noise (`QRandomGenerator::global()->bounded(100) / 100.0 - 0.5`) adds ±0.5 variation
- ObjectId offset in sin/cos functions ensures different objects have different patterns

### Step 6: Update Plot with Data

Extract data from our structures and pass it to QCustomPlot.

**In `datamonitorwidget.cpp`:**

```cpp
void DataMonitorWidget::updatePlot(QCustomPlot *plot, const TimeSeriesData &data)
{
    // QCustomPlot requires separate vectors for X and Y data
    int numPoints = data.dataPoints.size();
    QVector<double> time(numPoints);
    QVector<double> series1(numPoints);
    QVector<double> series2(numPoints);
    QVector<double> series3(numPoints);

    // Extract data from our DataPoint structure
    for (int i = 0; i < numPoints; ++i)
    {
        const DataPoint &point = data.dataPoints[i];
        time[i] = point.time;
        series1[i] = point.series1Value;
        series2[i] = point.series2Value;
        series3[i] = point.series3Value;
    }

    // Update all 3 series on this plot
    plot->graph(0)->setData(time, series1);
    plot->graph(1)->setData(time, series2);
    plot->graph(2)->setData(time, series3);

    // Update axis ranges to fit data
    plot->xAxis->setRange(0, numPoints);
    plot->yAxis->rescale();  // Auto-scale Y axis to fit all data

    // Redraw the plot
    plot->replot();
}

void DataMonitorWidget::updateGraphs(int objectId)
{
    // Generate data for the selected object
    ObjectData data = generateFakeData(objectId);

    // Update all three plots
    updatePlot(plotTemperature, data.metricA);
    updatePlot(plotHumidity, data.metricB);
    updatePlot(plotPressure, data.metricC);
}
```

**Why separate the data?**
QCustomPlot's `setData()` method requires separate X and Y vectors, so we transform our `DataPoint` structure into the required format.

### Step 7: Add Interactive Callouts

Implement clickable callouts that show values at a specific time point.

**In `datamonitorwidget.cpp`:**

```cpp
void DataMonitorWidget::setupCallouts(QCustomPlot *plot, PlotCallout &callout)
{
    // Create tracer for series 1 (green)
    callout.tracer1 = new QCPItemTracer(plot);
    callout.tracer1->setInterpolating(true);  // Smooth positioning between points
    callout.tracer1->setStyle(QCPItemTracer::tsCircle);
    callout.tracer1->setPen(QPen(Qt::green, 2));
    callout.tracer1->setBrush(Qt::green);
    callout.tracer1->setSize(8);
    callout.tracer1->setVisible(false);  // Hidden until clicked

    // Create tracer for series 2 (yellow)
    callout.tracer2 = new QCPItemTracer(plot);
    callout.tracer2->setInterpolating(true);
    callout.tracer2->setStyle(QCPItemTracer::tsCircle);
    callout.tracer2->setPen(QPen(Qt::yellow, 2));
    callout.tracer2->setBrush(Qt::yellow);
    callout.tracer2->setSize(8);
    callout.tracer2->setVisible(false);

    // Create tracer for series 3 (red)
    callout.tracer3 = new QCPItemTracer(plot);
    callout.tracer3->setInterpolating(true);
    callout.tracer3->setStyle(QCPItemTracer::tsCircle);
    callout.tracer3->setPen(QPen(Qt::red, 2));
    callout.tracer3->setBrush(Qt::red);
    callout.tracer3->setSize(8);
    callout.tracer3->setVisible(false);

    // Create text label to show values
    callout.textLabel = new QCPItemText(plot);
    callout.textLabel->setPositionAlignment(Qt::AlignTop|Qt::AlignHCenter);
    callout.textLabel->position->setType(QCPItemPosition::ptPlotCoords);
    callout.textLabel->setFont(QFont(font().family(), 10));
    callout.textLabel->setPen(QPen(Qt::black));
    callout.textLabel->setBrush(QBrush(QColor(255, 255, 255, 200)));  // Semi-transparent white
    callout.textLabel->setPadding(QMargins(5, 5, 5, 5));
    callout.textLabel->setVisible(false);
}

void DataMonitorWidget::showCallout(QCustomPlot *plot, PlotCallout &callout, double xCoord)
{
    // Lazy initialization: Set graph associations on first use
    // This avoids warnings when graphs have no data yet
    if (!callout.tracer1->graph()) {
        callout.tracer1->setGraph(plot->graph(0));
        callout.tracer2->setGraph(plot->graph(1));
        callout.tracer3->setGraph(plot->graph(2));
    }

    // Position tracers at the clicked X coordinate
    callout.tracer1->setGraphKey(xCoord);
    callout.tracer2->setGraphKey(xCoord);
    callout.tracer3->setGraphKey(xCoord);

    // Get Y values at this X position
    double value1 = callout.tracer1->position->value();
    double value2 = callout.tracer2->position->value();
    double value3 = callout.tracer3->position->value();

    // Format text to display all values
    QString text = QString("Time: %1\nSeries 1: %2\nSeries 2: %3\nSeries 3: %4")
                       .arg(xCoord, 0, 'f', 1)
                       .arg(value1, 0, 'f', 2)
                       .arg(value2, 0, 'f', 2)
                       .arg(value3, 0, 'f', 2);

    callout.textLabel->setText(text);
    callout.textLabel->position->setCoords(xCoord, value2);  // Position near middle series

    // Make all callout elements visible
    callout.tracer1->setVisible(true);
    callout.tracer2->setVisible(true);
    callout.tracer3->setVisible(true);
    callout.textLabel->setVisible(true);

    plot->replot();
}

void DataMonitorWidget::onPlotClicked(QMouseEvent *event)
{
    // Find which plot was clicked
    QCustomPlot *plot = qobject_cast<QCustomPlot*>(sender());
    if (!plot) return;

    // Convert mouse pixel position to plot coordinate
    double xCoord = plot->xAxis->pixelToCoord(event->pos().x());

    // Show callout on the appropriate plot
    if (plot == plotTemperature) {
        showCallout(plotTemperature, calloutTemperature, xCoord);
    } else if (plot == plotHumidity) {
        showCallout(plotHumidity, calloutHumidity, xCoord);
    } else if (plot == plotPressure) {
        showCallout(plotPressure, calloutPressure, xCoord);
    }
}

void DataMonitorWidget::onObjectIdChanged(int objectId)
{
    updateGraphs(objectId);
}
```

**Callout implementation details:**
- `QCPItemTracer`: Marker that follows a graph at a specific X coordinate
- `setInterpolating(true)`: Calculates Y value between data points for smooth positioning
- `QCPItemText`: Floating text box on the plot
- `pixelToCoord()`: Converts mouse click position to plot coordinates
- Lazy initialization prevents "graph has no data" warnings during startup

### Step 8: Create Main Application

Create the main entry point that uses our custom widget.

**Create `main.cpp`:**

```cpp
#include <QApplication>
#include <QMainWindow>
#include "datamonitorwidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    // Create main window
    QMainWindow mainWindow;
    mainWindow.setWindowTitle("Multi-Series Data Monitor");
    mainWindow.resize(1200, 900);

    // Create and set the custom widget as central widget
    DataMonitorWidget *dataMonitor = new DataMonitorWidget();
    mainWindow.setCentralWidget(dataMonitor);

    mainWindow.show();

    return app.exec();
}
```

**Why use QMainWindow + custom widget?**
- Separates window management from widget functionality
- Makes the widget reusable in other applications
- Follows Qt best practices
- Allows adding menu bars, toolbars, status bars later if needed

### Step 9: Configure CMake Build

Set up the build system.

**Create `CMakeLists.txt`:**

```cmake
cmake_minimum_required(VERSION 3.10)

project(QCustomPlotExample VERSION 1.0 LANGUAGES CXX)

# Use C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable Qt MOC, RCC, and UIC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt5 packages
find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport REQUIRED)

# List all source files
set(PROJECT_SOURCES
    main.cpp
    datamonitorwidget.cpp
    datamonitorwidget.h
    qcustomplot.cpp
    qcustomplot.h
)

# Create executable
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::PrintSupport
)
```

**CMake configuration explained:**
- `CMAKE_AUTOMOC`: Automatically runs Qt's Meta-Object Compiler
- `CMAKE_AUTORCC`: Processes Qt resource files
- `CMAKE_AUTOUIC`: Processes Qt UI files
- `find_package()`: Locates Qt5 installation
- `PrintSupport`: Required by QCustomPlot for printing functionality

## Building the Project

### Linux/macOS

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake ..

# Build
make

# Run
./QCustomPlotExample
```

### Windows (with MinGW)

```cmd
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
QCustomPlotExample.exe
```

### Windows (with Visual Studio)

```cmd
mkdir build
cd build
cmake -G "Visual Studio 16 2019" ..
cmake --build . --config Release
Release\QCustomPlotExample.exe
```

## Features

### 1. Multi-Series Display
- Each chart displays three series with distinct colors (green, yellow, red)
- Three separate charts for different metrics (A, B, C)
- Legend shows series names and units

### 2. Object Selection
- Spinbox allows selecting object IDs from 1 to 100
- Each object has unique data patterns
- Data updates in real-time when selection changes

### 3. Interactive Callouts
- Click anywhere on a chart to see values
- Displays time and all three series values
- Colored markers show exact points on each series

### 4. Navigation
- **Drag**: Click and drag to pan the view
- **Zoom**: Scroll wheel to zoom in/out
- **Auto-scale**: Y-axis automatically fits data range

### 5. Clean Design
- Abstract metric names (Metric A, B, C)
- No semi-transparent fills under lines
- Professional color scheme
- Responsive layout

## Usage

### Basic Usage

```cpp
// In your Qt application:
DataMonitorWidget *monitor = new DataMonitorWidget();
monitor->show();
```

### Embedding in Another Window

```cpp
// In your main window:
QVBoxLayout *layout = new QVBoxLayout();
DataMonitorWidget *monitor = new DataMonitorWidget(this);
layout->addWidget(monitor);
centralWidget->setLayout(layout);
```

### Using Multiple Widgets

```cpp
// Display multiple monitors side by side:
QHBoxLayout *layout = new QHBoxLayout();
layout->addWidget(new DataMonitorWidget());
layout->addWidget(new DataMonitorWidget());
```

## Customization

### Adding More Series

In `datamonitorwidget.h`, modify `DataPoint`:

```cpp
struct DataPoint {
    double time;
    double series1Value;
    double series2Value;
    double series3Value;
    double series4Value;  // Add new series
};
```

Then update `setupPlots()` to add the fourth graph and choose a color.

### Changing Colors

In `setupPlots()`, modify the pen color:

```cpp
plot->graph(0)->setPen(QPen(QColor(0, 128, 255), 2));  // Custom RGB color
```

### Adding More Charts

1. Add new `QCustomPlot` pointer in header
2. Create it in `setupUI()`
3. Configure it in `setupPlots()`
4. Add corresponding data in `ObjectData` struct
5. Update `generateFakeData()` to populate it

### Loading Real Data

Replace `generateFakeData()` with your data loading function:

```cpp
ObjectData DataMonitorWidget::loadDataFromDatabase(int objectId)
{
    ObjectData data;

    // Query your database
    // Populate data.metricA, data.metricB, data.metricC

    return data;
}
```

### Styling

Customize plot appearance:

```cpp
// Background colors
plot->setBackground(QBrush(Qt::white));
plot->axisRect()->setBackground(QBrush(QColor(240, 240, 240)));

// Grid lines
plot->xAxis->grid()->setPen(QPen(QColor(200, 200, 200), 1, Qt::DotLine));
plot->yAxis->grid()->setPen(QPen(QColor(200, 200, 200), 1, Qt::DotLine));

// Font sizes
plot->xAxis->setLabelFont(QFont("Arial", 12));
plot->yAxis->setLabelFont(QFont("Arial", 12));
```

## Troubleshooting

### Qt5 Not Found

**Error:** `Could not find a package configuration file provided by "Qt5"`

**Solution:** Set `CMAKE_PREFIX_PATH` to your Qt installation:
```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/5.15.2/gcc_64 ..
```

### QCustomPlot Compilation Errors

Make sure you're using QCustomPlot version compatible with your Qt version. Download from [qcustomplot.com](https://www.qcustomplot.com).

### Graphs Not Showing

- Check that data is being generated (add debug output in `generateFakeData()`)
- Verify `updateGraphs(1)` is called in constructor
- Ensure `plot->replot()` is called after setting data

### Callouts Not Working

- Verify `mousePress` signal is connected in `setupPlots()`
- Check that interaction flag is set: `setInteractions(QCP::iRangeDrag | QCP::iRangeZoom)`
- Don't include `QCP::iSelectPlottables` (causes color changes on click)

## License

This project uses QCustomPlot, which is licensed under GPL v3. If you plan to use this in a commercial closed-source application, you need to purchase a commercial QCustomPlot license from [qcustomplot.com](https://www.qcustomplot.com).

## Credits

- **QCustomPlot**: Emanuel Eichhammer ([qcustomplot.com](https://www.qcustomplot.com))
- **Qt Framework**: The Qt Company ([qt.io](https://www.qt.io))

## Further Reading

- [QCustomPlot Documentation](https://www.qcustomplot.com/documentation/index.html)
- [Qt Widgets Tutorial](https://doc.qt.io/qt-5/qtwidgets-tutorials-widgets-tutorial-example.html)
- [Qt Signals and Slots](https://doc.qt.io/qt-5/signalsandslots.html)
- [Qt Layouts](https://doc.qt.io/qt-5/layout.html)
