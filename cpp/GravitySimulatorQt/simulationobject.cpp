#include "SimulationObject.h"
#include <cmath>

SimulationObject::SimulationObject(const std::string& name, std::pair<double, double> position,
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
    std::cout << "Collision between " << name << " and " << other.name << std::endl;

    // Example: Invert the velocities for a simple bounce effect
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
    // Calculate the distance and direction between the two objects
    double distX = other.position.first - position.first;
    double distY = other.position.second - position.second;
    double dist = std::sqrt(distX * distX + distY * distY);

    // Avoid division by zero
    if (dist == 0.0) {
        return;
    }

    // Calculate the force and direction of the gravitational force
    double force = (gforce * mass * other.mass) / (dist * dist);
    double dirX = distX / dist;
    double dirY = distY / dist;

    // Apply the gravitational force to the acceleration of both objects
    acceleration.first += dirX * force / mass;
    acceleration.second += dirY * force / mass;

    other.acceleration.first -= dirX * force / other.mass;
    other.acceleration.second -= dirY * force / other.mass;
}

bool SimulationObject::detectCollision(const SimulationObject& other) {
    if (this == &other) {
        return false;
    }

    double dist = std::sqrt(std::pow(other.position.first - position.first, 2) +
                            std::pow(other.position.second - position.second, 2));
    return dist < radius + other.radius;
}
