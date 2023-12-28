#ifndef SIMULATIONOBJECT_H
#define SIMULATIONOBJECT_H

#include <cmath>
#include <string>
#include <QString>

class SimulationObject {
public:
    SimulationObject(const QString name, std::pair<double, double> position,
                     std::pair<double, double> velocity, std::pair<double, double> acceleration,
                     double radius, double mass);


    void fall(double frame_time);
    void resetAcceleration();
    void collide(SimulationObject& other);
    void simulateStep(double frame_time);
    void applyGravity(SimulationObject& other, double gforce);
    bool detectCollision(const SimulationObject& other);

    std::pair<double, double> getPosition();
    void setPosition(double x, double y);
    QString getName();
    double getRadius();
    void setRadius(double r);
    bool getIsHighlighted();

private:
    QString name;
    std::pair<double, double> position;
    std::pair<double, double> velocity;
    std::pair<double, double> acceleration;
    double radius;
    double mass;
    bool isHighlighted;
};

#endif // SIMULATIONOBJECT_H
