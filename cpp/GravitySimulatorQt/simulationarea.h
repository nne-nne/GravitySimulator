#ifndef SIMULATIONAREA_H
#define SIMULATIONAREA_H

#include <QWidget>
#include <QPointF>
#include "SimulationController.h"

class SimulationArea : public QWidget {
    Q_OBJECT

public:
    SimulationArea(QWidget *parent, SimulationController *simulationController);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    SimulationController *simulationController;
};

#endif // SIMULATIONAREA_H
