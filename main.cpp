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
