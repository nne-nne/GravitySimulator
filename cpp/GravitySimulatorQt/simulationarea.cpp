#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QBrush>
#include <QColor>
#include <QFont>
#include "simulationcontroller.h"
#include <QtLogging>

SimulationArea::SimulationArea(QWidget *parent, SimulationController *simulationController)
    : QWidget(parent), simulationController(simulationController) {}


void SimulationArea::updateSimulation()
{
    repaint();
}

void SimulationArea::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::transparent);
    painter.setBrush(QColor(151, 172, 184));

    painter.drawRect(rect());
    painter.setBrush(QBrush(Qt::gray, Qt::SolidPattern));
    QFont font("Sans", 13);
    painter.setFont(font);

    for (const auto &o : simulationController->getSimulationObjects()) {
        if (o->getIsHighlighted()) {
            painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
        } else {
            painter.setBrush(QBrush(QColor(220, 220, 220), Qt::SolidPattern));
        }

        painter.drawEllipse(QRectF(o->getPosition().first - o->getRadius(), o->getPosition().second - o->getRadius(), o->getRadius() * 2, o->getRadius() * 2));
        painter.setPen(Qt::black);
        painter.drawText(QPointF(o->getPosition().first + 10, o->getPosition().second + 10), o->getName());
        painter.setPen(Qt::transparent);
    }
}

void SimulationArea::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        int x = event->x();
        int y = event->y();

        if (simulationController->getIsAdding()) {
            qDebug("get is adding");
            simulationController->createSimulationObject(QPointF(x, y));
        } else {
            std::pair<double, double> prevPos = simulationController->getEditedObject()->getPosition();
            qDebug("%f, %f", prevPos.first, prevPos.second );

            simulationController->getEditedObject()->setPosition(x, y);

            QList<SimulationObject *> simulationObjects = simulationController->getSimulationObjects();
            for (int i = 0; i < simulationObjects.size(); i++)
            {
                SimulationObject* other = simulationObjects.at(i);
                if (other->getPosition() == simulationController->getEditedObject()->getPosition()) {
                    continue;
                }

                if (simulationController->getEditedObject()->detectCollision(*other)) {
                    simulationController->getMainAppWindow()->setInfoLabel("akcja spowodowałaby kolizję!");
                    simulationController->getEditedObject()->setPosition(prevPos.first, prevPos.second);
                    break;
                }
            }
        }
    }
}
