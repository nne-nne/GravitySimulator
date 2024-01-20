#include "simulationobject.h"
#include <cmath>
#include <QDebug>

SimulationObject::SimulationObject(const QString name, std::pair<double, double> position,
                                   std::pair<double, double> velocity, std::pair<double, double> acceleration,
                                   double radius, double mass)
    : name(name), position(position), velocity(velocity), acceleration(acceleration), radius(radius), mass(mass),
    isHighlighted(false) {}

void SimulationObject::fall(double frame_time) {
    acceleration = {0, 10};
    velocity = {velocity.first + acceleration.first * frame_time, velocity.second + acceleration.second * frame_time};
    position = {velocity.first * frame_time + position.first, velocity.second * frame_time + position.second};
}

void SimulationObject::resetAcceleration() {
    acceleration = {0, 0};
}

void SimulationObject::collide(SimulationObject& other) {
    qInfo() << "Collision between " << name << " and " << other.name << "\n";
    std::pair<double, double> temp = velocity;
    velocity = other.velocity;
    other.velocity = temp;
}

void SimulationObject::simulateStep(double frame_time) {
    velocity = {velocity.first + acceleration.first * frame_time, velocity.second + acceleration.second * frame_time};
    position = {velocity.first * frame_time + position.first, velocity.second * frame_time + position.second};
    resetAcceleration();
}

void SimulationObject::applyGravity(SimulationObject& other, double gforce) {
    double distX = other.position.first - position.first;
    double distY = other.position.second - position.second;
    double dist = std::sqrt(distX * distX + distY * distY);

    if (dist == 0.0) {
        return;
    }

    double force = (gforce * mass * other.mass) / (dist * dist);
    double dirX = distX / dist;
    double dirY = distY / dist;

    acceleration.first += dirX * force / mass;
    acceleration.second += dirY * force / mass;

    other.acceleration.first -= dirX * force / other.mass;
    other.acceleration.second -= dirY * force / other.mass;
}

bool SimulationObject::detectCollision(SimulationObject& other) {
    if (this == &other) {
        return false;
    }

    double dist = std::sqrt(std::pow(other.position.first - position.first, 2) +
                            std::pow(other.position.second - position.second, 2));
    return dist < radius + other.radius;
}

std::pair<double, double> SimulationObject::getPosition()
{
    return position;
}

void SimulationObject::setPosition(double x, double y)
{
    position.first = x;
    position.second = y;
}

QString SimulationObject::getName()
{
    return name;
}

double SimulationObject::getRadius()
{
    return radius;
}

double SimulationObject::getMass()
{
    return mass;
}

std::pair<double, double> SimulationObject::getAcceleration(){
    return acceleration;
}

std::pair<double, double> SimulationObject::getVelocity(){
    return velocity;
}

void SimulationObject::setRadius(double r)
{
    radius = r;
}

bool SimulationObject::getIsHighlighted()
{
    return isHighlighted;
}

void SimulationObject::setIsHighlighted(bool val)
{
    isHighlighted = val;
}

void SimulationObject::setName(QString name)
{
    this->name = name;
}

void SimulationObject::setMass(double mass)
{
    this->mass = mass;
}

void SimulationObject::setVelocity(double x, double y)
{
    velocity.first = x;
    velocity.second = y;
}
