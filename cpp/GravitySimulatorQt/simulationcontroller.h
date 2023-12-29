#ifndef SIMULATION_CONTROLLER_H
#define SIMULATION_CONTROLLER_H

#include <QObject>
#include <QList>
#include <QPointF>
#include <QElapsedTimer>
#include "simulationobject.h"
#include "mainwindow.h"

class SimulationController : public QObject {
    Q_OBJECT

public slots:
    void brr();

public:
    SimulationController(QObject* app, const QPoint& size, const QPoint& margin);

    void resetSimulation();
    void nextFrame(double frameTime);
    void checkDestroyObject(SimulationObject* o);
    void fallAll(double frameTime);
    void simulateGravity(double frameTime, double gforce);
    void highlightObject(SimulationObject* o);
    void adjustObject(SimulationObject* o);
    void createSimulationObject(const QPointF& clickPosition);
    void chooseObjectToEdit(SimulationObject* o);
    void unhighlight();
    SimulationObject* getSimulationObject(int i);
    QList<SimulationObject*> getSimulationObjects();
    SimulationObject* getEditedObject();
    bool getIsPaused();
    void setIsPaused(bool val);
    bool getIsAdding();
    void setIsAdding(bool val);
    MainAppWindow* getMainAppWindow();
    void setSimulationSpeed(double val);

private:
    QList<SimulationObject*> simulationObjects;
    MainAppWindow* mainAppWindow;
    QPoint size;
    QPoint margin;
    QElapsedTimer prevTime;
    double gforce;
    bool isPaused;
    double simulationSpeed;
    double timeRes;
    bool isAdding;
    SimulationObject* editedObject;
};

#endif // SIMULATION_CONTROLLER_H
