# QCustomPlot Multi-Series Data Monitor Widget

A comprehensive guide to building a clean, reusable Qt widget for displaying multi-series time-series data using QCustomPlot.

## Table of Contents

- [Overview](#overview)
- [Code Quality Features](#code-quality-features)
- [Prerequisites](#prerequisites)
- [Project Setup](#project-setup)
- [Step-by-Step Implementation](#step-by-step-implementation)
  - [Step 1: Define Configuration Constants](#step-1-define-configuration-constants)
  - [Step 2: Define Data Structures](#step-2-define-data-structures)
  - [Step 3: Create the Widget Header](#step-3-create-the-widget-header)
  - [Step 4: Implement UI Setup](#step-4-implement-ui-setup)
  - [Step 5: Create Helper Functions](#step-5-create-helper-functions)
  - [Step 6: Implement Data Generation](#step-6-implement-data-generation)
  - [Step 7: Update Plot with Data](#step-7-update-plot-with-data)
  - [Step 8: Add Interactive Callouts](#step-8-add-interactive-callouts)
  - [Step 9: Create Main Application](#step-9-create-main-application)
  - [Step 10: Configure CMake Build](#step-10-configure-cmake-build)
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

## Code Quality Features

This implementation follows best practices for clean, maintainable code:

✅ **No Magic Numbers** - All constants defined in `Config` namespace
✅ **No Code Duplication** - Helper functions eliminate repetitive code
✅ **Consistent Naming** - Variables follow abstract naming conventions
✅ **Const Correctness** - Proper use of `const` throughout
✅ **Separation of Concerns** - Clear separation between UI, data, and visualization
✅ **Easy Customization** - All configuration in one place
✅ **Well Documented** - Inline comments explain the "why"
✅ **Minimal Dependencies** - Only necessary includes

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

### Step 1: Define Configuration Constants

Start by centralizing all configuration values in a `Config` namespace. This eliminates magic numbers and makes customization easy.

**In `datamonitorwidget.h`:**

```cpp
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
```

**Benefits:**
- All configuration in one place - easy to customize
- No magic numbers scattered throughout code
- Type-safe constants with `constexpr`
- Self-documenting code

### Step 2: Define Data Structures

Define clear data structures to represent time-series data.

**In `datamonitorwidget.h`:**

```cpp
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
    TimeSeriesData metricA;  // Data for first chart
    TimeSeriesData metricB;  // Data for second chart
    TimeSeriesData metricC;  // Data for third chart
};
```

**Why this structure?**
- `DataPoint`: Single point in time with values for all three series
- Default constructor initializes all values to zero
- `TimeSeriesData`: Collection of points forming a complete time series
- `ObjectData`: Groups all three metrics for a single object ID
- Easy to extend with more metrics or series

### Step 3: Create the Widget Header

Define the widget class with proper organization and helper methods.

**Complete `datamonitorwidget.h`:**

```cpp
#ifndef DATAMONITORWIDGET_H
#define DATAMONITORWIDGET_H

#include <QWidget>
#include <QSpinBox>
#include <QLabel>
#include "qcustomplot.h"

// [Config namespace and data structures from Steps 1-2]

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
```

**Key improvements:**
- Consistent naming: `plotMetricA/B/C` instead of `plotTemperature/Humidity/Pressure`
- `PlotCallout` uses array for tracers instead of individual variables
- Helper method `setupSinglePlot()` to eliminate code duplication
- Helper method `createTracer()` to create tracers consistently
- `generateFakeData()` is `const` (doesn't modify widget state)

### Step 4: Implement UI Setup

Create the user interface layout with consistent configuration.

**In `datamonitorwidget.cpp`:**

```cpp
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
    updateGraphs(Config::DEFAULT_OBJECT_ID);  // Use constant, not magic number
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
    objectIdSpinBox->setMinimum(Config::MIN_OBJECT_ID);    // Use config constants
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

    plotMetricA->setMinimumHeight(Config::PLOT_MIN_HEIGHT);  // Use config constant
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
```

**No magic numbers** - All values come from `Config` namespace.

### Step 5: Create Helper Functions

Eliminate code duplication by creating reusable helper functions.

**In `datamonitorwidget.cpp`:**

```cpp
void DataMonitorWidget::setupPlots()
{
    const QString unitsA[] = {"Unit A", "Unit B", "Unit C"};
    const QString unitsB[] = {"Unit D", "Unit E", "Unit F"};
    const QString unitsC[] = {"Unit G", "Unit H", "Unit I"};

    // One function call instead of repeating setup code 3 times
    setupSinglePlot(plotMetricA, Config::METRIC_LABELS[0], unitsA, calloutMetricA);
    setupSinglePlot(plotMetricB, Config::METRIC_LABELS[1], unitsB, calloutMetricB);
    setupSinglePlot(plotMetricC, Config::METRIC_LABELS[2], unitsC, calloutMetricC);
}

void DataMonitorWidget::setupSinglePlot(QCustomPlot *plot, const QString &metricLabel,
                                         const QString units[], PlotCallout &callout)
{
    // Add graphs for each series using loops instead of copy-paste
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
    // Create tracers using helper function
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
```

**Benefits of helper functions:**
- `setupSinglePlot()`: Eliminates triple repetition of plot setup
- `createTracer()`: Consistent tracer creation with config constants
- Loops instead of copy-paste for series setup
- Much easier to maintain - change in one place affects all plots

### Step 6: Implement Data Generation

Generate fake sinusoidal data using configuration constants.

**In `datamonitorwidget.cpp`:**

```cpp
ObjectData DataMonitorWidget::generateFakeData(int objectId) const
{
    ObjectData data;

    // Pre-allocate vectors using config constant
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

        // Generate Metric B data (using cosine for variety)
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
```

**Key points:**
- Method is `const` - doesn't modify widget state
- Uses `Config::NUM_DATA_POINTS` instead of magic number 100
- Sinusoidal patterns create smooth, realistic data
- Random noise adds variation
- ObjectId affects both base value and pattern phase

### Step 7: Update Plot with Data

Extract data from structures and update plots.

**In `datamonitorwidget.cpp`:**

```cpp
void DataMonitorWidget::updatePlot(QCustomPlot *plot, const TimeSeriesData &data)
{
    const int numPoints = data.dataPoints.size();
    QVector<double> time(numPoints);
    QVector<double> series1(numPoints);
    QVector<double> series2(numPoints);
    QVector<double> series3(numPoints);

    // Extract data into separate vectors for QCustomPlot
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
    plot->yAxis->rescale();  // Auto-scale to fit all data
    plot->replot();
}

void DataMonitorWidget::updateGraphs(int objectId)
{
    const ObjectData data = generateFakeData(objectId);

    updatePlot(plotMetricA, data.metricA);
    updatePlot(plotMetricB, data.metricB);
    updatePlot(plotMetricC, data.metricC);
}
```

### Step 8: Add Interactive Callouts

Implement callouts using loops and configuration constants.

**In `datamonitorwidget.cpp`:**

```cpp
void DataMonitorWidget::showCallout(QCustomPlot *plot, PlotCallout &callout, double xCoord)
{
    // Lazy initialization: associate tracers with graphs on first use
    if (!callout.tracers[0]->graph()) {
        for (int i = 0; i < Config::NUM_SERIES; ++i) {
            callout.tracers[i]->setGraph(plot->graph(i));
        }
    }

    // Position tracers and get values using loop
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
```

**Improvements:**
- Loops instead of individual tracer handling
- Values array instead of separate variables
- Lazy initialization prevents startup warnings
- Consistent use of `const`

### Step 9: Create Main Application

Create the main entry point.

**Create `main.cpp`:**

```cpp
#include <QApplication>
#include <QMainWindow>
#include "datamonitorwidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

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

### Step 10: Configure CMake Build

Set up the build system.

**Create `CMakeLists.txt`:**

```cmake
cmake_minimum_required(VERSION 3.10)

project(QCustomPlotExample VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets PrintSupport REQUIRED)

set(PROJECT_SOURCES
    main.cpp
    datamonitorwidget.cpp
    datamonitorwidget.h
    qcustomplot.cpp
    qcustomplot.h
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::PrintSupport
)
```

## Building the Project

### Linux/macOS

```bash
mkdir build
cd build
cmake ..
make
./QCustomPlotExample
```

### Windows (MinGW)

```cmd
mkdir build
cd build
cmake -G "MinGW Makefiles" ..
mingw32-make
QCustomPlotExample.exe
```

### Windows (Visual Studio)

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

### 5. Clean Code Architecture
- No magic numbers - all configuration centralized
- No code duplication - helper functions for common tasks
- Consistent naming conventions
- Easy to customize and extend

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

### Changing Configuration Values

All customization starts in the `Config` namespace in `datamonitorwidget.h`:

```cpp
namespace Config {
    constexpr int NUM_DATA_POINTS = 200;  // Change to 200 points
    constexpr int LINE_WIDTH = 3;         // Thicker lines
    constexpr int MAX_OBJECT_ID = 500;    // Support more objects

    // Change colors
    const QColor SERIES_COLORS[NUM_SERIES] = {
        QColor(0, 128, 255),   // Blue
        QColor(255, 128, 0),   // Orange
        QColor(128, 0, 255)    // Purple
    };
}
```

### Adding More Series

To add a 4th series:

1. Update `Config::NUM_SERIES` to 4
2. Add 4th color to `SERIES_COLORS` array
3. Add `series4Value` to `DataPoint` struct
4. Update `generateFakeData()` to populate 4th series
5. Update `updatePlot()` to extract 4th series

### Adding More Metrics

To add a 4th chart:

1. Update `Config::NUM_METRICS` to 4
2. Add "Metric D" to `METRIC_LABELS` array
3. Add `QCustomPlot *plotMetricD` member variable
4. Add `TimeSeriesData metricD` to `ObjectData` struct
5. Create plot in `setupUI()`
6. Setup in `setupPlots()`
7. Update in `generateFakeData()` and `updateGraphs()`

### Loading Real Data

Replace `generateFakeData()` with your data source:

```cpp
ObjectData DataMonitorWidget::loadDataFromDatabase(int objectId) const
{
    ObjectData data;

    // Query your database
    QSqlQuery query;
    query.prepare("SELECT time, s1, s2, s3 FROM metrics WHERE object_id = ?");
    query.addBindValue(objectId);
    query.exec();

    while (query.next()) {
        DataPoint point;
        point.time = query.value(0).toDouble();
        point.series1Value = query.value(1).toDouble();
        point.series2Value = query.value(2).toDouble();
        point.series3Value = query.value(3).toDouble();
        data.metricA.dataPoints.append(point);
    }

    return data;
}
```

### Styling Plots

Customize appearance in `setupSinglePlot()`:

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

**Solution:** Set `CMAKE_PREFIX_PATH`:
```bash
cmake -DCMAKE_PREFIX_PATH=/path/to/Qt/5.15.2/gcc_64 ..
```

### QCustomPlot Compilation Errors

Ensure QCustomPlot version matches your Qt version. Download from [qcustomplot.com](https://www.qcustomplot.com).

### Graphs Not Showing

- Check `generateFakeData()` is being called
- Verify `updateGraphs()` is called in constructor
- Ensure `plot->replot()` is called after setting data

### Callouts Not Working

- Verify `mousePress` signal is connected in `setupSinglePlot()`
- Check interaction flags: `QCP::iRangeDrag | QCP::iRangeZoom`
- Don't use `QCP::iSelectPlottables` (causes color changes)

## Code Quality Checklist

When extending this widget, maintain code quality by:

- [ ] Add new configuration values to `Config` namespace, not as magic numbers
- [ ] Use loops instead of copy-paste for repetitive operations
- [ ] Create helper functions for code used more than once
- [ ] Use `const` for methods that don't modify state
- [ ] Use `const` for local variables that don't change
- [ ] Follow naming conventions (plotMetricA, not plotTemperature)
- [ ] Add comments explaining "why", not "what"
- [ ] Only include headers you actually use
- [ ] Initialize all struct members

## License

This project uses QCustomPlot, which is licensed under GPL v3. For commercial closed-source applications, purchase a commercial QCustomPlot license from [qcustomplot.com](https://www.qcustomplot.com).

## Credits

- **QCustomPlot**: Emanuel Eichhammer ([qcustomplot.com](https://www.qcustomplot.com))
- **Qt Framework**: The Qt Company ([qt.io](https://www.qt.io))

## Further Reading

- [QCustomPlot Documentation](https://www.qcustomplot.com/documentation/index.html)
- [Qt Widgets Tutorial](https://doc.qt.io/qt-5/qtwidgets-tutorials-widgets-tutorial-example.html)
- [Qt Signals and Slots](https://doc.qt.io/qt-5/signalsandslots.html)
- [C++ Best Practices](https://github.com/cpp-best-practices/cppbestpractices)
