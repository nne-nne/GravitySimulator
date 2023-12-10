#include "SimulationController.h"
#include "SimulationObject.h"
#include <cmath>

SimulationController::SimulationController(QObject* app, const QPoint& size, const QPoint& margin)
    : app(app), size(size), margin(margin), gforce(6.67408), isPaused(false), simulationSpeed(1.0),
      timeRes(0.0), isAdding(true), editedObject(nullptr)
{
    prevTime.start();
}

void SimulationController::resetSimulation()
{
    simulationObjects.clear();
    gforce = 6.67408;
    prevTime.restart();
}

void SimulationController::brr()
{
    double frameTime = prevTime.elapsed() / 1000.0;
    prevTime.restart();

    if (isPaused)
        return;

    timeRes += frameTime * simulationSpeed;
    int stepsToGo = static_cast<int>(std::floor(timeRes / 0.01));
    timeRes -= stepsToGo * 0.01;

    for (int i = 0; i < stepsToGo; ++i)
        nextFrame(0.01);
}

void SimulationController::nextFrame(double frameTime)
{
    simulateGravity(frameTime, gforce);
}

void SimulationController::checkDestroyObject(SimulationObject* o)
{
    bool destroy = false;
    if (o->position.x() < -margin.x() || o->position.y() < -margin.y() ||
        o->position.x() > margin.x() + size.x() || o->position.y() > margin.y() + size.y())
    {
        destroy = true;
    }

    if (destroy)
    {
        app->setProperty("info_label", QString("Obiekt %1 opuścił obszar symulacji.").arg(o->name));
        simulationObjects.removeOne(o);
        delete o;
    }
}

void SimulationController::fallAll(double frameTime)
{
    for (SimulationObject* o : simulationObjects)
    {
        o->fall(frameTime);
        checkDestroyObject(o);
    }
}

void SimulationController::simulateGravity(double frameTime, double gforce)
{
    for (int i = 0; i < simulationObjects.size(); ++i)
    {
        for (int j = i + 1; j < simulationObjects.size(); ++j)
        {
            simulationObjects[i]->applyGravity(simulationObjects[j], gforce);

            if (simulationObjects[i]->detectCollision(simulationObjects[j]))
            {
                app->setProperty("info_label", QString("Kolizja obiektów %1 i %2.")
                                              .arg(simulationObjects[i]->name)
                                              .arg(simulationObjects[j]->name));
                simulationObjects[i]->collide(simulationObjects[j]);
            }
        }

        simulationObjects[i]->simulateStep(frameTime);
        checkDestroyObject(simulationObjects[i]);
    }
}

void SimulationController::highlightObject(SimulationObject* o)
{
    unhighlight();

    if (o)
        o->isHighlighted = true;
}

void SimulationController::adjustObject(SimulationObject* o)
{
    // Implementation for adjusting object properties
    double prevRadius = o->radius;

    if (!app->property("radius_edit").toString().isEmpty())
    {
        bool ok;
        double r = app->property("radius_edit").toString().toDouble(&ok);
        if (ok && r > 0)
        {
            o->radius = r;
        }
        else
        {
            app->setProperty("info_label", "Zły format liczby");
                return;
        }
    }

    // Update other properties as needed
    // ...

    // Clear the input fields in the UI
    app->setProperty("name_edit", "");
    app->setProperty("mass_edit", "");
    app->setProperty("radius_edit", "");
    app->setProperty("velocity_edit_x", "");
    app->setProperty("velocity_edit_y", "");
    app->setProperty("position_edit_x", "");
    app->setProperty("position_edit_y", "");

    app->setProperty("info_label", QString("edytowano obiekt %1").arg(o->name));
}

void SimulationController::createSimulationObject(const QPointF& clickPosition)
{
    // Implementation for creating a new simulation object
    QString name = app->property("name_edit").toString();
    if (name.isEmpty())
    {
        app->setProperty("info_label", "uzupełnij pole 'nazwa'");
        return;
    }

    // Parse other properties from the UI
    QPointF position = clickPosition;
    double mass = 100.0;
    double radius = 10.0;
    QPointF velocity(0.0, 0.0);

    bool ok;
    if (!app->property("mass_edit").toString().isEmpty())
    {
        mass = app->property("mass_edit").toString().toDouble(&ok);
        if (!ok || mass <= 0)
        {
            app->setProperty("info_label", "Zły format liczby");
            return;
        }
    }

    if (!app->property("radius_edit").toString().isEmpty())
    {
        radius = app->property("radius_edit").toString().toDouble(&ok);
        if (!ok || radius <= 0)
        {
            app->setProperty("info_label", "Zły format liczby");
            return;
        }
    }

    // Parse and set other properties (velocity, position) as needed
    // ...

    SimulationObject* o = new SimulationObject(name, position, radius, mass, velocity);

    // Check for collision and other conditions
    bool canPlace = true;
    for (SimulationObject* other : simulation_objects)
    {
        if (o->detectCollision(other))
        {
            app->setProperty("info_label", "akcja spowodowałaby kolizję!");
            canPlace = false;
            break;
        }
    }

    if (canPlace)
    {
        simulation_objects.push_back(o);
        app->addObjectTile(o);
        app->setProperty("info_label", QString("dodano obiekt %1").arg(o->name));

        // Clear the input fields in the UI
        app->setProperty("name_edit", "");
        app->setProperty("mass_edit", "");
        app->setProperty("radius_edit", "");
        app->setProperty("velocity_edit_x", "");
        app->setProperty("velocity_edit_y", "");
        app->setProperty("position_edit_x", "");
        app->setProperty("position_edit_y", "");
    }
}


void SimulationController::chooseObjectToEdit(SimulationObject* o)
{
    if (o)
    {
        app->setProperty("info_label", QString("wybrano obiekt %1").arg(o->name));
        highlightObject(o);
        editedObject = o;
    }
}

void SimulationController::unhighlight()
{
    for (SimulationObject* o : simulationObjects)
        o->isHighlighted = false;
}
