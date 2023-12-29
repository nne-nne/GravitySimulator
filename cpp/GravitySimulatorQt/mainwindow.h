#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QMessageBox>
#include <QDateTime>
#include "simulationcontroller.h"
#include "SimulationArea.h"
#include "simulationobjecttile.h"

class MainAppWindow : public QWidget {
    Q_OBJECT

public:
    MainAppWindow();
    SimulationArea* getSimulationArea();
    SimulationController* getController();

public slots:
    void updateTiles();

private slots:
    void adjustEdited();
    void addObjectTile(SimulationObject *o);
    void showAboutView();
    void returnToMain();
    void setInfoLabel(const QString &text);
    void togglePause();
    void toggleAdding();
    void showNewSimulationDialogue();
    void changeSimulationSpeed();

private:
    SimulationController controller;
    SimulationArea *simulationArea;
    QVBoxLayout *rootLayout;
    QHBoxLayout *mainLayout;
    QWidget *rootWidget;
    QWidget *mainWidget;
    QMenuBar *menuBar;
    QMenu *aboutMenu;
    QAction *newAction;
    QWidget *menuBarWidget;
    QHBoxLayout *menuBarLayout;
    QLabel *infoLabel;
    QPushButton *pauseButton;
    QPushButton *newSimulationButton;
    QLabel *simSpeedLabel;
    QLineEdit *simSpeedField;
    QPushButton *addEditButton1;
    QPushButton *addEditButton2;
    QWidget *objectPanel;
    QVBoxLayout *simulationObjectLayout;
    QList<SimulationObjectTile*> objectTiles;
    QWidget *propertiesPanel;
    QVBoxLayout *propertiesLayout;
    QLabel *nameLabel;
    QLineEdit *nameEdit;
    QLabel *massLabel;
    QLineEdit *massEdit;
    QLabel *radiusLabel;
    QLineEdit *radiusEdit;
    QLabel *velocityLabel;
    QLineEdit *velocityEditX;
    QLineEdit *velocityEditY;
    QLabel *velocityLabelX;
    QLabel *velocityLabelY;
    QWidget *velocityEditRow;
    QLabel *positionLabel;
    QLineEdit *positionEditX;
    QLineEdit *positionEditY;
    QLabel *positionLabelX;
    QLabel *positionLabelY;
    QWidget *positionEditRow;
    QWidget *aboutView;
    QVBoxLayout *aboutViewLayout;
    QLabel *aboutViewTextLabel;
    QPushButton *aboutViewBackButton;

    void createMenuBar();
    void createMainWidget();
    void createObjectPanel();
    void createPropertiesPanel();
    void createAboutView();
};

#endif // MAINWINDOW_H
