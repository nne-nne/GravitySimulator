#ifndef SIMULATIONAREA_H
#define SIMULATIONAREA_H

#include <QWidget>
#include <QPointF>
#include "simulationcontroller.h"

class SimulationArea : public QWidget {
    Q_OBJECT

public:
    SimulationArea(QWidget *parent, SimulationController *simulationController);

public slots:
    void update();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    SimulationController *simulationController;
};

#endif // SIMULATIONAREA_H
