#ifndef SIMULATIONOBJECT_H
#define SIMULATIONOBJECT_H

#include <cmath>
#include <string>

class SimulationObject {
public:
    SimulationObject(const std::string& name = "?", std::pair<double, double> position = {-1.0, -1.0},
                     std::pair<double, double> velocity = {0.0, 0.0}, std::pair<double, double> acceleration = {0, 0},
                     double radius = 0.0, double mass = 0.0);

    void fall(double frame_time);
    void resetAcceleration();
    void collide(SimulationObject& other);
    void simulateStep(double frame_time);
    void applyGravity(SimulationObject& other, double gforce);
    bool detectCollision(const SimulationObject& other);

    // Getters and setters if needed

private:
    std::string name;
    std::pair<double, double> position;
    std::pair<double, double> velocity;
    std::pair<double, double> acceleration;
    double radius;
    double mass;
    bool isHighlighted;
};

#endif // SIMULATIONOBJECT_H
