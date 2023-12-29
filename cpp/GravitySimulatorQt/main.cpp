#include "mainwindow.h"

#include <QApplication>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainAppWindow mainAppWindow;
    QTimer timer;
    SimulationArea* simulationArea = mainAppWindow.getSimulationArea();
    QObject::connect(&timer, &QTimer::timeout, simulationArea, &SimulationArea::updateSimulation);
    QObject::connect(&timer, &QTimer::timeout, &mainAppWindow, &MainAppWindow::updateTiles);
    QObject::connect(&timer, &QTimer::timeout, mainAppWindow.getController(), &SimulationController::brr);
    timer.start(1);
    mainAppWindow.show();
    return a.exec();
}
