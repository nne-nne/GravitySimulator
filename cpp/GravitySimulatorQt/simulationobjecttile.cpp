#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

class SimulationObjectTile : public QWidget {
    Q_OBJECT

public:
    SimulationObjectTile(SimulationObject* simulation_object, SimulationController* simulation_controller, QWidget* wrapper)
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

        button_name = new QLabel(o->name, this);
        mass_label = new QLabel("masa:", this);
        mass_val = new QLabel(QString::number(o->mass), this);
        position_label = new QLabel("pozycja:", this);
        position_val = new QLabel(QString("%1; %2").arg(o->position[0]).arg(o->position[1]), this);
        position_val->setFixedWidth(300);
        speed_label = new QLabel("prędkość:", this);
        speed_val = new QLabel(QString("%1; %2").arg(o->velocity[0]).arg(o->velocity[1]), this);
        speed_val->setFixedWidth(80);
        radius_label = new QLabel("promień:", this);
        radius_val = new QLabel(QString::number(o->radius), this);
        acc_label = new QLabel("przyspieszenie:", this);
        acc_val = new QLabel(QString("%1; %2").arg(o->acceleration[0]).arg(o->acceleration[1]), this);
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

    void update() {
        button_name->setText(o->name);
        mass_val->setText(QString::number(o->mass));
        position_val->setText(QString("%1; %2").arg(o->position[0]).arg(o->position[1]));
        speed_val->setText(QString("%1; %2").arg(o->velocity[0]).arg(o->velocity[1]));
        radius_val->setText(QString::number(o->radius));
        acc_val->setText(QString("%1; %2").arg(o->acceleration[0]).arg(o->acceleration[1]));

        if (!controller->simulation_objects.contains(o)) {
            remove();
        }
    }

    void mousePressEvent(QMouseEvent* event) override {
        controller->chooseObjectToEdit(o);
        if (controller->isAdding) {
            controller->app->toggleAdding();
        }
    }

    void remove() {
        controller->app->objectTiles.removeOne(this);
        if (controller->simulation_objects.contains(o)) {
            controller->simulation_objects.removeOne(o);
        }
        deleteLater();
        controller->app->simulationObjectLayout->removeWidget(wrapper);
    }

private:
    SimulationObject* o;
    SimulationController* controller;
    QWidget* wrapper;
    QVBoxLayout* tile_layout;
    QWidget* upper_row;
    QHBoxLayout* upper_row_layout;
    QWidget* bottom_row;
    QHBoxLayout* bottom_row_layout;
    QLabel* button_name;
    QLabel* mass_label;
    QLabel* mass_val;
    QLabel* position_label;
    QLabel* position_val;
    QLabel* speed_label;
    QLabel* speed_val;
    QLabel* radius_label;
    QLabel* radius_val;
    QLabel* acc_label;
    QLabel* acc_val;
};
