#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QElapsedTimer timer;
    QObject::connect(&timer, &QElapsedTimer::timeout, &main_window.simulation_area, &QWidget::repaint);
    QObject::connect(&timer, &QElapsedTimer::timeout, &main_window, &MainWindow::update_tiles);
    QObject::connect(&timer, &QElapsedTimer::timeout, &main_window.controller, &ControllerClass::brr);
    timer.start(1);
    main_window.show();
    w.show();
    return a.exec();
}
