#ifndef SIMULATION_OBJECT_TILE_H
#define SIMULATION_OBJECT_TILE_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include "simulationobject.h"
#include "simulationcontroller.h"

class SimulationObjectTile : public QWidget {
    Q_OBJECT

public:
    SimulationObjectTile(SimulationObject* simulation_object, SimulationController* simulation_controller, QWidget* wrapper);

    void update();
    void mousePressEvent(QMouseEvent* event) override;
    void remove();

private:
    SimulationObject* o;
    SimulationController* controller;
    QWidget* wrapper;
    QVBoxLayout* tile_layout;
    QWidget* upper_row;
    QHBoxLayout* upper_row_layout;
    QWidget* bottom_row;
    QHBoxLayout* bottom_row_layout;
    QLabel* button_name;
    QLabel* mass_label;
    QLabel* mass_val;
    QLabel* position_label;
    QLabel* position_val;
    QLabel* speed_label;
    QLabel* speed_val;
    QLabel* radius_label;
    QLabel* radius_val;
    QLabel* acc_label;
    QLabel* acc_val;
};

#endif // SIMULATION_OBJECT_TILE_H
