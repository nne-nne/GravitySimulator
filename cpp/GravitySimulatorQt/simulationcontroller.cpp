#include "simulationcontroller.h"
#include "simulationobject.h"
#include <cmath>
#include <QVariant>

SimulationController::SimulationController(MainAppWindow* mainAppWindow, const QPoint& size, const QPoint& margin)
    : mainAppWindow(mainAppWindow), size(size), margin(margin), gforce(6.67408), isPaused(false), simulationSpeed(1.0),
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
    if (o->getPosition().first < -margin.x() || o->getPosition().second < -margin.y() ||
        o->getPosition().first > margin.x() + size.x() || o->getPosition().second > margin.y() + size.y())
    {
        destroy = true;
    }

    if (destroy)
    {
        mainAppWindow->setProperty("info_label", QString("Obiekt %1 opuścił obszar symulacji.").arg(o->getName()));
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
        SimulationObject* o1 = simulationObjects[i];
        for (int j = i + 1; j < simulationObjects.size(); ++j)
        {
            SimulationObject* o2 = simulationObjects[j];
            o1->applyGravity(*o2, gforce);

            if (o1->detectCollision(*o2))
            {
                mainAppWindow->setProperty("info_label", QString("Kolizja obiektów %1 i %2.")
                                              .arg(o1->getName())
                                                   .arg(o2->getName()));
               o1->collide(*o2);
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
        o->setIsHighlighted(true);
}

void SimulationController::adjustObject(SimulationObject* o)
{
    if (!mainAppWindow->property("radius_edit").toString().isEmpty())
    {
        bool ok;
        double r = mainAppWindow->property("radius_edit").toString().toDouble(&ok);
        if (ok && r > 0)
        {
            o->setRadius(r);
        }
        else
        {
            mainAppWindow->setProperty("info_label", "Zły format liczby");
                return;
        }
    }

    // Clear the input fields in the UI
    mainAppWindow->setProperty("mass_edit", "");
    mainAppWindow->setProperty("name_edit", "");
    mainAppWindow->setProperty("radius_edit", "");
    mainAppWindow->setProperty("velocity_edit_x", "");
    mainAppWindow->setProperty("velocity_edit_y", "");
    mainAppWindow->setProperty("position_edit_x", "");
    mainAppWindow->setProperty("position_edit_y", "");

    mainAppWindow->setProperty("info_label", QString("edytowano obiekt %1").arg(o->getName()));
}

void SimulationController::createSimulationObject(const QPointF& clickPosition)
{
    // Implementation for creating a new simulation object
    QString name = mainAppWindow->property("name_edit").toString();
    if (name.isEmpty())
    {
        mainAppWindow->setProperty("info_label", "uzupełnij pole 'nazwa'");
        return;
    }

    std::pair<double, double> position(clickPosition.x(), clickPosition.y());
    double mass = 100.0;
    double radius = 10.0;
    std::pair<double, double> velocity(0.0, 0.0);

    bool ok;
    if (!mainAppWindow->property("mass_edit").toString().isEmpty())
    {
        mass = mainAppWindow->property("mass_edit").toString().toDouble(&ok);
        if (!ok || mass <= 0)
        {
            mainAppWindow->setProperty("info_label", "Zły format liczby");
            return;
        }
    }

    if (!mainAppWindow->property("radius_edit").toString().isEmpty())
    {
        radius = mainAppWindow->property("radius_edit").toString().toDouble(&ok);
        if (!ok || radius <= 0)
        {
            mainAppWindow->setProperty("info_label", "Zły format liczby");
            return;
        }
    }
    std::pair<double, double> acceleration(0, 0);
    SimulationObject* o = new SimulationObject(name, position, velocity, acceleration, radius, mass);

    // Check for collision and other conditions
    bool canPlace = true;
    for (int i = 0; i < simulationObjects.size(); i++)
    {
        SimulationObject* other = simulationObjects.at(i);
        if (o->detectCollision(*other))
        {
            mainAppWindow->setProperty("info_label", "akcja spowodowałaby kolizję!");
            canPlace = false;
            break;
        }
    }

    if (canPlace)
    {
        simulationObjects.push_back(o);
        mainAppWindow->addObjectTile(&o);
        mainAppWindow->setProperty("info_label", QString("dodano obiekt %1").arg(o->getName()));

        // Clear the input fields in the UI
        mainAppWindow->setProperty("name_edit", "");
        mainAppWindow->setProperty("mass_edit", "");
        mainAppWindow->setProperty("radius_edit", "");
        mainAppWindow->setProperty("velocity_edit_x", "");
        mainAppWindow->setProperty("velocity_edit_y", "");
        mainAppWindow->setProperty("position_edit_x", "");
        mainAppWindow->setProperty("position_edit_y", "");
    }
}


void SimulationController::chooseObjectToEdit(SimulationObject* o)
{
    if (o)
    {
        mainAppWindow->setProperty("info_label", QString("wybrano obiekt %1").arg(o->getName()));
        highlightObject(o);
        editedObject = o;
    }
}

void SimulationController::unhighlight()
{
    for (SimulationObject* o : simulationObjects)
        o->setIsHighlighted(false);
}


SimulationObject* SimulationController::getSimulationObject(int i)
{
    return simulationObjects[i];
}

QList<SimulationObject*> SimulationController::getSimulationObjects()
{
    return simulationObjects;
}

bool SimulationController::getIsPaused()
{
    return isPaused;
}

void SimulationController::setIsPaused(bool val)
{
    isPaused =val;
}


bool SimulationController::getIsAdding()
{
    return isAdding;
}

void SimulationController::setIsAdding(bool val)
{
    isAdding =val;
}

SimulationObject* SimulationController::getEditedObject(){
    return editedObject;
}

MainAppWindow* SimulationController::getMainAppWindow()
{
    return mainAppWindow;
}

void SimulationController::setSimulationSpeed(double val)
{
    simulationSpeed = val;
}
