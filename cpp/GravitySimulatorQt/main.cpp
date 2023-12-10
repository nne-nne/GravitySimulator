#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    QTimer timer;
    QObject::connect(&timer, &QTimer::timeout, &main_window.simulation_area, &QWidget::repaint);
    QObject::connect(&timer, &QTimer::timeout, &main_window, &MainWindow::update_tiles);
    QObject::connect(&timer, &QTimer::timeout, &main_window.controller, &ControllerClass::brr);
    timer.start(1);
    main_window.show();
    w.show();
    return a.exec();
}
