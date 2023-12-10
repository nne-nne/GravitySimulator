#ifndef SIMULATION_CONTROLLER_H
#define SIMULATION_CONTROLLER_H

#include <QObject>
#include <QList>
#include <QPointF>
#include <QTime>
#include "SimulationObject.h"  // Include the header file for SimulationObject

class SimulationController : public QObject {
    Q_OBJECT

public:
    SimulationController(QObject* app, const QPoint& size, const QPoint& margin);

    void resetSimulation();
    void brr();
    void nextFrame(double frameTime);
    void checkDestroyObject(SimulationObject* o);
    void fallAll(double frameTime);
    void simulateGravity(double frameTime, double gforce);
    void highlightObject(SimulationObject* o);
    void adjustObject(SimulationObject* o);
    void createSimulationObject(const QPointF& clickPosition);
    void chooseObjectToEdit(SimulationObject* o);
    void unhighlight();

private:
    QList<SimulationObject*> simulationObjects;
    QObject* app;
    QPoint size;
    QPoint margin;
    QTime prevTime;
    double gforce;
    bool isPaused;
    double simulationSpeed;
    double timeRes;
    bool isAdding;
    SimulationObject* editedObject;
};

#endif // SIMULATION_CONTROLLER_H
