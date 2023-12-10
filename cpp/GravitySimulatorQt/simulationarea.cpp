#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QBrush>
#include <QColor>
#include <QFont>
#include "SimulationController.h"

class SimulationArea : public QWidget {
    Q_OBJECT

public:
    SimulationArea(QWidget *parent, SimulationController *simulationController)
        : QWidget(parent), simulationController(simulationController) {}

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(Qt::transparent);
        painter.setBrush(QColor(151, 172, 184));

        painter.drawRect(rect());
        painter.setBrush(QBrush(Qt::gray, Qt::SolidPattern));
        QFont font("Sans", 13);
        painter.setFont(font);

        for (const auto &o : simulationController->getSimulationObjects()) {
            if (o.isHighlighted) {
                painter.setBrush(QBrush(Qt::white, Qt::SolidPattern));
            } else {
                painter.setBrush(QBrush(QColor(220, 220, 220), Qt::SolidPattern));
            }

            painter.drawEllipse(QRectF(o.position[0] - o.radius, o.position[1] - o.radius, o.radius * 2, o.radius * 2));
            painter.setPen(Qt::black);
            painter.drawText(QPointF(o.position[0] + 10, o.position[1] + 10), o.name);
            painter.setPen(Qt::transparent);
        }
    }

    void mousePressEvent(QMouseEvent *event) override {
        if (event->button() == Qt::LeftButton) {
            int x = event->x();
            int y = event->y();

            if (simulationController->isAdding()) {
                simulationController->createSimulationObject(QPointF(x, y));
            } else {
                QPointF prevPos = simulationController->getEditedObject().position;
                simulationController->getEditedObject().position = QPointF(x, y);

                for (const auto &other : simulationController->getSimulationObjects()) {
                    if (other.position == simulationController->getEditedObject().position) {
                        continue;
                    }

                    if (simulationController->getEditedObject().detectCollision(other)) {
                        simulationController->getApp()->setInfoLabel("akcja spowodowałaby kolizję!");
                        simulationController->getEditedObject().position = prevPos;
                        break;
                    }
                }
            }
        }
    }

private:
    SimulationController *simulationController;
};
