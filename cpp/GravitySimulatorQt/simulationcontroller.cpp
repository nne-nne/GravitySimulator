#include "simulationcontroller.h"
#include "simulationobject.h"
#include <cmath>
#include <QVariant>

SimulationController::SimulationController(MainAppWindow* mainAppWindow, const QPoint& size, const QPoint& margin, double gforce, bool isPaused, double simulationSpeed, double timeRes, bool isAdding, SimulationObject* editedObject)
    : mainAppWindow(mainAppWindow), size(size), margin(margin), gforce(gforce), isPaused(isPaused), simulationSpeed(simulationSpeed),
      timeRes(timeRes), isAdding(isAdding), editedObject(editedObject)
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
        mainAppWindow->setInfoLabel(QString("Obiekt %1 opuścił obszar symulacji.").arg(o->getName()));
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
                mainAppWindow->setInfoLabel(QString("Kolizja obiektów %1 i %2.")
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
    QString name = mainAppWindow->getNameEditValue(o->getName());
    o->setName(name);

    double mass = mainAppWindow->getMassEditValue(o->getMass());
    o->setMass(mass);

    double radius = mainAppWindow->getRadiusEditValue(o->getRadius());
    o->setRadius(radius);

    std::pair<double, double> currentPosition = o->getPosition();
    std::pair<double, double> position = mainAppWindow->getPositionEditValue(currentPosition.first, currentPosition.second);

    o->setPosition(position.first, position.second);

    std::pair<double, double> currentVelocity = o->getVelocity();
    std::pair<double, double> velocity = mainAppWindow->getVelocityEditValue(currentVelocity.first, currentVelocity.second);

    o->setVelocity(velocity.first, velocity.second);

    mainAppWindow->clearEditFields();

    mainAppWindow->setInfoLabel(QString("edytowano obiekt %1").arg(o->getName()));
}

void SimulationController::createSimulationObject(const QPointF& clickPosition)
{
    QString name = mainAppWindow->getNameEditValue("");
    if (!mainAppWindow->canCreateSimulationObject())
    {
        mainAppWindow->setInfoLabel("uzupełnij pole 'nazwa'");
        return;
    }

    std::pair<double, double> position = mainAppWindow->getPositionEditValue(clickPosition.x(), clickPosition.y());
    double mass = mainAppWindow->getMassEditValue(10.0);
    double radius = mainAppWindow->getRadiusEditValue(10.0);
    std::pair<double, double> velocity =  mainAppWindow->getVelocityEditValue(0.0, 0.0);
    std::pair<double, double> acceleration(0, 0);
    SimulationObject* o = new SimulationObject(name, position, velocity, acceleration, radius, mass);

    bool canPlace = true;
    for (int i = 0; i < simulationObjects.size(); i++)
    {
        SimulationObject* other = simulationObjects.at(i);
        if (o->detectCollision(*other))
        {
            mainAppWindow->setInfoLabel("akcja spowodowałaby kolizję!");
            canPlace = false;
            break;
        }
    }

    if (canPlace)
    {
        simulationObjects.push_back(o);
        mainAppWindow->addObjectTile(o);
        mainAppWindow->setInfoLabel(QString("dodano obiekt %1").arg(o->getName()));
        mainAppWindow->clearEditFields();
    }
}

void SimulationController::chooseObjectToEdit(SimulationObject* o)
{
    if (o)
    {
        mainAppWindow->setInfoLabel(QString("wybrano obiekt %1").arg(o->getName()));
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
