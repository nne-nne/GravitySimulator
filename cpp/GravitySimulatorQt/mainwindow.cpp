#include "mainwindow.h"
#include "simulationobjecttile.h"
#include "SimulationArea.h"
#include <QDateTime>

SimulationArea* MainAppWindow::getSimulationArea()
{
    return simulationArea;
}

SimulationController* MainAppWindow::getController()
{
    return controller;
}

QVBoxLayout* MainAppWindow::getSimulationObjectLayout()
{
    return simulationObjectLayout;
}

MainAppWindow::MainAppWindow() : simulationArea() {
    setWindowTitle("Symulator Grawitacji");
    setFixedSize(1150, 550);
    controller = new SimulationController(this, QPoint(500, 500), QPoint(100, 100), 6.67408, false, 1.0, 0.0, true, nullptr);
    controller->setParent(this);
    simulationArea = new SimulationArea(this, controller);
    rootWidget = new QWidget(this);
    setCentralWidget(rootWidget);
    rootLayout = new QVBoxLayout(rootWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootWidget->setLayout(rootLayout);

    // Create main widget
    mainWidget = new QWidget(this);
    mainLayout = new QHBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Create menu bar
    createMenuBar();

    // Create menu bar widget
    menuBarWidget = new QWidget(this);
    menuBarLayout = new QHBoxLayout(menuBarWidget);
    menuBarLayout->setContentsMargins(0, 0, 0, 0);

    // Create "Info" label
    infoLabel = new QLabel();
    setInfoLabel("Rozpoczęcie symulacji");
    infoLabel->setFixedWidth(300);

    // Create "Pause" button
    pauseButton = new QPushButton("Pauza", this);
    pauseButton->setFixedWidth(60);
    connect(pauseButton, &QPushButton::clicked, this, &MainAppWindow::togglePause);

    // Create "New simulation" button
    newSimulationButton = new QPushButton("Nowa symulacja", this);
    newSimulationButton->setFixedWidth(100);
    connect(newSimulationButton, &QPushButton::clicked, this, &MainAppWindow::showNewSimulationDialogue);

    // Create "Simulation speed" field
    simSpeedLabel = new QLabel("Prędkość symulacji:", this);
    simSpeedLabel->setFixedWidth(100);
    simSpeedField = new QLineEdit("1");
    simSpeedField->setFixedWidth(30);
    simSpeedField->setValidator(new QDoubleValidator());
    connect(simSpeedField, &QLineEdit::returnPressed, this, &MainAppWindow::changeSimulationSpeed);

    menuBarLayout->addWidget(infoLabel);
    menuBarLayout->addWidget(pauseButton);
    menuBarLayout->addWidget(newSimulationButton);
    menuBarLayout->addWidget(simSpeedLabel);
    menuBarLayout->addWidget(simSpeedField);
    menuBarLayout->setAlignment(Qt::AlignLeft);
    menuBarWidget->setLayout(menuBarLayout);

    rootLayout->addWidget(menuBarWidget);

    addEditButton1 = new QPushButton("➕ Tryb dodawania");
    connect(addEditButton1, &QPushButton::clicked, this, &MainAppWindow::toggleAdding);
    addEditButton2 = new QPushButton("➕ Tryb dodawania");
    connect(addEditButton2, &QPushButton::clicked, this, &MainAppWindow::toggleAdding);

    // Create simulation object's panel
    createObjectPanel();

    // Create simulation area
    mainLayout->addWidget(objectPanel);
    mainLayout->addWidget(simulationArea);

    // Create object properties panel
    createPropertiesPanel();

    rootLayout->addWidget(mainWidget);

    // Create about view
    createAboutView();
}

void MainAppWindow::adjustEdited() {
    if (!controller->getIsAdding()) {
        controller->adjustObject(controller->getEditedObject());
    }
}

void MainAppWindow::addObjectTile(SimulationObject *o) {
    QWidget *wrapper = new QWidget(this);
    SimulationObjectTile *tile = new SimulationObjectTile(o, controller, wrapper);
    QHBoxLayout *wrapperLayout = new QHBoxLayout(wrapper);
    wrapperLayout->setAlignment(Qt::AlignLeft);
    wrapperLayout->setContentsMargins(0, 0, 0, 0);
    wrapperLayout->addWidget(tile);

    QPushButton *removeButton = new QPushButton("X", this);
    removeButton->setFixedSize(40, 60);
    connect(removeButton, &QPushButton::clicked, tile, &SimulationObjectTile::remove);
    wrapperLayout->addWidget(removeButton);

    objectTiles.append(tile);
    simulationObjectLayout->addWidget(wrapper);
}

void MainAppWindow::showAboutView() {
    mainWidget->setHidden(true);
    aboutView->setHidden(false);
    menuBarWidget->setHidden(true);
}

void MainAppWindow::returnToMain() {
    mainWidget->setHidden(false);
    aboutView->setHidden(true);
    menuBarWidget->setHidden(false);
}

void MainAppWindow::setInfoLabel(const QString &text) {
    QString currentTime = QDateTime::currentDateTime().toString("HH:mm:ss:zzz");
    QString labeledText = currentTime + ": " + text;
    infoLabel->setText(labeledText);
}

void MainAppWindow::togglePause() {
    if (!controller->getIsPaused()) {
        pauseButton->setText("Wznów");
            controller->setIsPaused(true);
    } else {
        pauseButton->setText("Pauza");
        controller->setIsPaused(false);
    }
}

void MainAppWindow::toggleAdding() {
    if (!controller->getIsAdding()) {
        addEditButton1->setText("➕ Tryb dodawania");
        addEditButton2->setText("➕ Tryb dodawania");
        controller->setIsAdding(true);
        controller->unhighlight();
    } else {
        if (!controller->getSimulationObjects().isEmpty()) {
            addEditButton1->setText("✏️ Tryb edycji");
            addEditButton2->setText("✏️ Tryb edycji");
            controller->setIsAdding(false);
            controller->chooseObjectToEdit(controller->getSimulationObject(0));
        } else {
            setInfoLabel("brak obiektów do edycji!");
        }
    }
}

void MainAppWindow::showNewSimulationDialogue() {
    if (!controller->getIsPaused()) {
        pauseButton->setText("Wznów");
            controller->setIsPaused(true);
    }

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Nowa symulacja");
    msgBox.setText("Czy na pewno chcesz rozpocząć nową symulację?");
    msgBox.setInformativeText("Obecny stan symulacji zostanie trwale usunięty.");
    msgBox.addButton("Rozpocznij nową symulację", QMessageBox::AcceptRole);
    msgBox.addButton("Zaniechaj", QMessageBox::RejectRole);

    int result = msgBox.exec();
    if (result == QMessageBox::AcceptRole) {
        controller->resetSimulation();
    } else if (result == QMessageBox::RejectRole) {
        pauseButton->setText("Pauza");
        controller->setIsPaused(false);
    }
}

QList<SimulationObjectTile*> MainAppWindow::getObjectTiles()
{
    return objectTiles;
}

void MainAppWindow::changeSimulationSpeed() {
    bool conversionOk;
    double newSpeed = simSpeedField->text().replace(',', '.').toDouble(&conversionOk);
    if (conversionOk) {
        controller->setSimulationSpeed(newSpeed);
        setInfoLabel("Nowa prędkość symulacji: " + QString::number(newSpeed));
    }
}

void MainAppWindow::updateTiles() {
    for (SimulationObjectTile *tile : objectTiles) {
        tile->update();
    }
}

void MainAppWindow::createMenuBar() {
    aboutMenu = new QMenu("O aplikacji");
    menuBar = new QMenuBar();
    menuBar->addMenu(aboutMenu);
    setMenuBar(menuBar);

    newAction = new QAction(this);
    newAction->setText("Opis aplikacji:\nAplikacja służy do symulowania oddziaływań grawitacyjnych\
 pomiędzy obiektami o zadanych masach.\nZasymulowano również niecentralne odbicia idealnie sprężyste.");
    aboutMenu->addAction(newAction);
}

void MainAppWindow::createObjectPanel() {
    objectPanel = new QWidget(this);
    objectPanel->setFixedWidth(400);
    simulationObjectLayout = new QVBoxLayout(objectPanel);
    simulationObjectLayout->setContentsMargins(0, 0, 0, 0);
    objectTiles.clear();
    simulationObjectLayout->setAlignment(Qt::AlignTop);
    simulationObjectLayout->addWidget(addEditButton1);
    mainLayout->addWidget(objectPanel);
}

void MainAppWindow::createPropertiesPanel() {
    propertiesPanel = new QWidget(this);
    propertiesPanel->setFixedWidth(200);
    propertiesLayout = new QVBoxLayout(propertiesPanel);
    propertiesLayout->setContentsMargins(0, 0, 0, 0);

    nameLabel = new QLabel("Nazwa:", this);
    nameEdit = new QLineEdit(this);

    massLabel = new QLabel("Masa:", this);
    massEdit = new QLineEdit(this);
    massEdit->setValidator(new QDoubleValidator());

    radiusLabel = new QLabel("Promień:", this);
    radiusEdit = new QLineEdit(this);
    radiusEdit->setValidator(new QDoubleValidator());

    velocityLabel = new QLabel("Prędkość:", this);
    velocityEditX = new QLineEdit(this);
    velocityEditY = new QLineEdit(this);
    velocityEditX->setValidator(new QDoubleValidator());
    velocityEditY->setValidator(new QDoubleValidator());
    velocityLabelX = new QLabel("x:", this);
    velocityLabelY = new QLabel("y:", this);
    velocityEditRow = new QWidget(this);
    velocityEditRow->setLayout(new QHBoxLayout());
    velocityEditRow->layout()->addWidget(velocityLabelX);
    velocityEditRow->layout()->addWidget(velocityEditX);
    velocityEditRow->layout()->addWidget(velocityLabelY);
    velocityEditRow->layout()->addWidget(velocityEditY);

    positionLabel = new QLabel("Pozycja:", this);
    positionEditX = new QLineEdit(this);
    positionEditY = new QLineEdit(this);
    positionEditX->setValidator(new QDoubleValidator());
    positionEditY->setValidator(new QDoubleValidator());
    positionLabelX = new QLabel("x:", this);
    positionLabelY = new QLabel("y:", this);
    positionEditRow = new QWidget(this);
    positionEditRow->setLayout(new QHBoxLayout());
    positionEditRow->layout()->addWidget(positionLabelX);
    positionEditRow->layout()->addWidget(positionEditX);
    positionEditRow->layout()->addWidget(positionLabelY);
    positionEditRow->layout()->addWidget(positionEditY);

    connect(addEditButton2, &QPushButton::clicked, this, &MainAppWindow::adjustEdited);
    connect(nameEdit, &QLineEdit::returnPressed, this, &MainAppWindow::adjustEdited);
    connect(massEdit, &QLineEdit::returnPressed, this, &MainAppWindow::adjustEdited);
    connect(radiusEdit, &QLineEdit::returnPressed, this, &MainAppWindow::adjustEdited);
    connect(velocityEditX, &QLineEdit::returnPressed, this, &MainAppWindow::adjustEdited);
    connect(velocityEditY, &QLineEdit::returnPressed, this, &MainAppWindow::adjustEdited);
    connect(positionEditX, &QLineEdit::returnPressed, this, &MainAppWindow::adjustEdited);
    connect(positionEditY, &QLineEdit::returnPressed, this, &MainAppWindow::adjustEdited);

    propertiesLayout->addWidget(nameLabel);
    propertiesLayout->addWidget(nameEdit);
    propertiesLayout->addWidget(massLabel);
    propertiesLayout->addWidget(massEdit);
    propertiesLayout->addWidget(radiusLabel);
    propertiesLayout->addWidget(radiusEdit);
    propertiesLayout->addWidget(velocityLabel);
    propertiesLayout->addWidget(velocityEditRow);
    propertiesLayout->addWidget(positionLabel);
    propertiesLayout->addWidget(positionEditRow);
    propertiesLayout->addWidget(addEditButton2);

    nameEdit->installEventFilter(this);

    propertiesLayout->addStretch();
    mainLayout->addWidget(propertiesPanel);
}


void MainAppWindow::createAboutView() {
    aboutView = new QWidget(this);
    aboutViewLayout = new QVBoxLayout(aboutView);
    aboutViewLayout->setAlignment(Qt::AlignCenter);

    aboutViewTextLabel = new QLabel(
        "Opis aplikacji:\nAplikacja służy do symulowania oddziaływań grawitacyjnych\
 pomiędzy obiektami o zadanych masach.\nZasymulowano również niecentralne odbicia idealnie sprężyste.\n\
Panel po lewej stronie aplikacji przedstawia dane pomiarowe na temat poszczególnych obiektów \
uczestniczących w symulacji.\n\
Wybranie obiektu umożliwia edycję jego właściwości w panelu po prawej stronie,\n \
który służy również do ustalania właściwości nowych obiektów.\n\
dodanie nowego obiektu, albo przesunięcie istniejącego, odbywa się \
poprzez wskazanie myszą miejsca w obszarze symulacji.\n\
Przyciski z napisem 'Tryb dodawania' lub 'Tryb edycji' służą jako przełączniki pomiędzy tymi trybami.",
        aboutView);

    aboutViewTextLabel->setFixedWidth(1100);
    aboutViewTextLabel->setWordWrap(true);

    aboutViewBackButton = new QPushButton("Powrót", this);
    aboutViewBackButton->setFixedWidth(1100);
    connect(aboutViewBackButton, &QPushButton::clicked, this, &MainAppWindow::returnToMain);

    aboutViewLayout->addWidget(aboutViewTextLabel);
    aboutViewLayout->addWidget(aboutViewBackButton);
    aboutView->setLayout(aboutViewLayout);

    rootLayout->addWidget(aboutView);
    aboutView->setHidden(true);
}

QString MainAppWindow::getNameEditValue(QString defaultValue)
{
    if(!nameEdit->text().isEmpty())
    {
        return nameEdit->text();
    }
    else
    {
        return defaultValue;
    }
}

double MainAppWindow::getMassEditValue(double defaultValue)
{
    if(!massEdit->text().isEmpty())
    {
        QString massStr = massEdit->text();
        bool success;
        double mass = massStr.toDouble(&success);
        return mass;
    }
    else
    {
        return defaultValue;
    }
}

double MainAppWindow::getRadiusEditValue(double defaultValue)
{
    if(!radiusEdit->text().isEmpty())
    {
        QString radiusStr = radiusEdit->text();
        bool success;
        double radius = radiusStr.toDouble(&success);
        return radius;
    }
    else
    {
        return defaultValue;
    }
}

std::pair<double, double> MainAppWindow::getVelocityEditValue(double defaultX, double defaultY)
{
    std::pair<double, double> result = std::pair<double, double>(defaultX, defaultY);
    if(!velocityEditX->text().isEmpty())
    {
        QString xStr = velocityEditX->text();
        bool successX;
        double x = xStr.toDouble(&successX);
        result.first = x;
    }
    if(!velocityEditY->text().isEmpty())
    {
        QString yStr = velocityEditY->text();
        bool successY;
        double y = yStr.toDouble(&successY);
        result.second = y;
    }
    return result;
}

std::pair<double, double> MainAppWindow::getPositionEditValue(double defaultX, double defaultY)
{
    std::pair<double, double> result = std::pair<double, double>(defaultX, defaultY);
    if(!positionEditX->text().isEmpty())
    {
        QString xStr = positionEditX->text();
        bool successX;
        double x = xStr.toDouble(&successX);
        result.first = x;
    }
    if(!positionEditY->text().isEmpty())
    {
        QString yStr = positionEditY->text();
        bool successY;
        double y = yStr.toDouble(&successY);
        result.second = y;
    }
    return result;
}

void MainAppWindow::clearEditFields()
{
    nameEdit->clear();
    massEdit->clear();
    radiusEdit->clear();
    velocityEditX->clear();
    velocityEditY->clear();
    positionEditX->clear();
    positionEditY->clear();
}

bool MainAppWindow::canCreateSimulationObject()
{
    return !nameEdit->text().isEmpty();
}
