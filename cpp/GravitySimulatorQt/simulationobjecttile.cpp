#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "simulationobjecttile.h"

SimulationObjectTile::SimulationObjectTile(SimulationObject* simulation_object, SimulationController* simulation_controller, QWidget* wrapper)
    : o(simulation_object), controller(simulation_controller), wrapper(wrapper) {
    setFixedWidth(380);
    setFixedHeight(50);

    tile_layout = new QVBoxLayout(this);
    tile_layout->setContentsMargins(0, 0, 0, 0);
    tile_layout->setAlignment(Qt::AlignTop);

    upper_row = new QWidget(this);
    upper_row_layout = new QHBoxLayout(upper_row);
    upper_row_layout->setContentsMargins(0, 0, 0, 0);
    upper_row->setFixedSize(350, 40);
    upper_row_layout->setAlignment(Qt::AlignLeft);

    bottom_row = new QWidget(this);
    bottom_row_layout = new QHBoxLayout(bottom_row);
    bottom_row_layout->setContentsMargins(0, 0, 0, 0);
    bottom_row_layout->setAlignment(Qt::AlignLeft);

    button_name = new QLabel(o->getName(), this);
    mass_label = new QLabel("masa:", this);
    mass_val = new QLabel(QString::number(o->getMass()), this);
    position_label = new QLabel("pozycja:", this);
    position_val = new QLabel(QString("%1; %2").arg(o->getPosition().first).arg(o->getPosition().second), this);
    position_val->setFixedWidth(300);
    speed_label = new QLabel("prędkość:", this);
    speed_val = new QLabel(QString("%1; %2").arg(o->getVelocity().first).arg(o->getVelocity().second), this);
    speed_val->setFixedWidth(80);
    radius_label = new QLabel("promień:", this);
    radius_val = new QLabel(QString::number(o->getRadius()), this);
    acc_label = new QLabel("przyspieszenie:", this);
    acc_val = new QLabel(QString("%1; %2").arg(o->getAcceleration().first).arg(o->getAcceleration().second), this);
    acc_val->setFixedWidth(80);

    upper_row_layout->addWidget(button_name);
    upper_row_layout->addWidget(mass_label);
    upper_row_layout->addWidget(mass_val);
    upper_row_layout->addWidget(speed_label);
    upper_row_layout->addWidget(speed_val);
    bottom_row_layout->addWidget(position_label);
    bottom_row_layout->addWidget(position_val);
    bottom_row_layout->addWidget(radius_label);
    bottom_row_layout->addWidget(radius_val);
    bottom_row_layout->addWidget(acc_label);
    bottom_row_layout->addWidget(acc_val);

    tile_layout->addWidget(upper_row);
    tile_layout->addWidget(bottom_row);
}

void SimulationObjectTile::update() {
    button_name->setText(o->getName());
    mass_val->setText(QString::number(o->getMass()));
    position_val->setText(QString("%1; %2").arg(o->getPosition().first).arg(o->getPosition().second));
    speed_val->setText(QString("%1; %2").arg(o->getVelocity().first).arg(o->getVelocity().second));
    radius_val->setText(QString::number(o->getRadius()));
    acc_val->setText(QString("%1; %2").arg(o->getAcceleration().first).arg(o->getAcceleration().second));

    if (!controller->getSimulationObjects().contains(o)) {
        remove();
    }
}

void SimulationObjectTile::mousePressEvent(QMouseEvent* event) {
    controller->chooseObjectToEdit(o);
    if (controller->getIsAdding()) {
        controller->getMainAppWindow()->toggleAdding();
    }
}

void SimulationObjectTile::remove() {
    controller->getMainAppWindow()->getObjectTiles().removeOne(this);
    if (controller->getSimulationObjects().contains(o)) {
        controller->getSimulationObjects().removeOne(o);
    }
    deleteLater();
    controller->getMainAppWindow()->getSimulationObjectLayout()->removeWidget(wrapper);
}

