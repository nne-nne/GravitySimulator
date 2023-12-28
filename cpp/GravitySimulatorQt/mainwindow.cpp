#include "MainWindow.h"
#include "SimulationObjectTile.h"
#include <QDateTime>

MainWindow::MainWindow() : simulationArea(new SimulationArea(this, &controller)) {
    setWindowTitle("Symulator Grawitacji");
    setFixedSize(1150, 550);

    // Create simulation controller
    controller.setParent(this);
    controller.initialize(QPointF(500, 500), QPointF(100, 100));

    // Create root widget
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
    connect(pauseButton, &QPushButton::clicked, this, &MainWindow::togglePause);

    // Create "New simulation" button
    newSimulationButton = new QPushButton("Nowa symulacja", this);
    newSimulationButton->setFixedWidth(100);
    connect(newSimulationButton, &QPushButton::clicked, this, &MainWindow::showNewSimulationDialogue);

    // Create "Simulation speed" field
    simSpeedLabel = new QLabel("Prędkość symulacji:", this);
    simSpeedLabel->setFixedWidth(100);
    simSpeedField = new QLineEdit("1");
    simSpeedField->setFixedWidth(30);
    simSpeedField->setValidator(new QDoubleValidator());
    connect(simSpeedField, &QLineEdit::returnPressed, this, &MainWindow::changeSimulationSpeed);

    menuBarLayout->addWidget(infoLabel);
    menuBarLayout->addWidget(pauseButton);
    menuBarLayout->addWidget(newSimulationButton);
    menuBarLayout->addWidget(simSpeedLabel);
    menuBarLayout->addWidget(simSpeedField);
    menuBarLayout->setAlignment(Qt::AlignLeft);
    menuBarWidget->setLayout(menuBarLayout);

    rootLayout->addWidget(menuBarWidget);

    addEditButton1 = new QPushButton("➕ Tryb dodawania");
    connect(addEditButton1, &QPushButton::clicked, this, &MainWindow::toggleAdding);
    addEditButton2 = new QPushButton("➕ Tryb dodawania");
    connect(addEditButton2, &QPushButton::clicked, this, &MainWindow::toggleAdding);

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

void MainWindow::adjustEdited() {
    if (!controller.isAdding) {
        controller.adjustObject(controller.getEditedObject());
    }
}

void MainWindow::addObjectTile(SimulationObject *o) {
    SimulationObjectTile *tile = new SimulationObjectTile(o, &controller);
    QWidget *wrapper = new QWidget(this);
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

void MainWindow::showAboutView() {
    mainWidget->setHidden(true);
    aboutView->setHidden(false);
    menuBarWidget->setHidden(true);
}

void MainWindow::returnToMain() {
    mainWidget->setHidden(false);
    aboutView->setHidden(true);
    menuBarWidget->setHidden(false);
}

void MainWindow::setInfoLabel(const QString &text) {
    QString currentTime = QDateTime::currentDateTime().toString("HH:mm:ss:zzz");
    QString labeledText = currentTime + ": " + text;
    infoLabel->setText(labeledText);
}

void MainWindow::togglePause() {
    if (!controller.getIsPaused()) {
        pauseButton->setText("Wznów");
            controller.setIsPaused(true);
    } else {
        pauseButton->setText("Pauza");
        controller.setIsPaused(false);
    }
}

void MainWindow::toggleAdding() {
    if (!controller.isAdding) {
        addEditButton1->setText("➕ Tryb dodawania");
        addEditButton2->setText("➕ Tryb dodawania");
        controller.isAdding = true;
        controller.unhighlight();
    } else {
        if (!controller.simulationObjects.isEmpty()) {
            addEditButton1->setText("✏️ Tryb edycji");
            addEditButton2->setText("✏️ Tryb edycji");
            controller.isAdding = false;
            controller.chooseObjectToEdit(controller.simulationObjects[0]);
        } else {
            setInfoLabel("brak obiektów do edycji!");
        }
    }
}

void MainWindow::showNewSimulationDialogue() {
    if (!controller.isPaused) {
        pauseButton->setText("Wznów");
        controller.isPaused = true;
    }

    QMessageBox msgBox(this);
    msgBox.setWindowTitle("Nowa symulacja");
    msgBox.setText("Czy na pewno chcesz rozpocząć nową symulację?");
    msgBox.setInformativeText("Obecny stan symulacji zostanie trwale usunięty.");
    msgBox.addButton("Rozpocznij nową symulację", QMessageBox::AcceptRole);
    msgBox.addButton("Zaniechaj", QMessageBox::RejectRole);

    int result = msgBox.exec();
    if (result == QMessageBox::AcceptRole) {
        controller.resetSimulation();
    } else if (result == QMessageBox::RejectRole) {
        pauseButton->setText("Pauza");
        controller.isPaused = false;
    }
}

void MainWindow::changeSimulationSpeed() {
    bool conversionOk;
    double newSpeed = simSpeedField->text().replace(',', '.').toDouble(&conversionOk);
    if (conversionOk) {
        controller.simulationSpeed = newSpeed;
        setInfoLabel("Nowa prędkość symulacji: " + QString::number(newSpeed));
    }
}

void MainWindow::updateTiles() {
    for (SimulationObjectTile *tile : objectTiles) {
        tile->update();
    }
}

void MainWindow::createMenuBar() {
    aboutMenu = new QMenu("O aplikacji");
    menuBar = new QMenuBar();
    menuBar->addMenu(aboutMenu);
    setMenuBar(menuBar);

    newAction = new QAction(this);
    newAction->setText("Opis aplikacji:\nAplikacja służy do symulowania oddziaływań grawitacyjnych\
 pomiędzy obiektami o zadanych masach.\nZasymulowano również niecentralne odbicia idealnie sprężyste.");
    aboutMenu->addAction(newAction);
}

void MainWindow::createObjectPanel() {
    objectPanel = new QWidget(this);
    objectPanel->setFixedWidth(400);
    simulationObjectLayout = new QVBoxLayout(objectPanel);
    simulationObjectLayout->setContentsMargins(0, 0, 0, 0);
    objectTiles.clear();
    simulationObjectLayout->setAlignment(Qt::AlignTop);
    simulationObjectLayout->addWidget(addEditButton1);
    mainLayout->addWidget(objectPanel);
}

void MainWindow::createPropertiesPanel() {
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

    addEditButton2 = new QPushButton("➕ Tryb edycji", this);
    connect(addEditButton2, &QPushButton::clicked, this, &MainWindow::adjustEdited);

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


void MainWindow::createAboutView() {
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
    connect(aboutViewBackButton, &QPushButton::clicked, this, &MainWindow::returnToMain);

    aboutViewLayout->addWidget(aboutViewTextLabel);
    aboutViewLayout->addWidget(aboutViewBackButton);
    aboutView->setLayout(aboutViewLayout);

    rootLayout->addWidget(aboutView);
    aboutView->setHidden(true);
}
