#include "mainwindow.h"

// Static Variables for Elevator number and floor number
static int elevatorCount;
static int floorCount;
static int passengerCount;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent),
                                          ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setUpConnection(); // Connect all the buttons to their respective functions
}

void MainWindow::setUpConnection()
{
    // connect button to start and simulation
    connect(ui->startSimulationButton, SIGNAL(clicked()), this, SLOT(startSimulation()));
    connect(ui->endSimulationButton, SIGNAL(clicked()), this, SLOT(endSimulation()));

    // make spinbox for number of elevators and floors
    ui->elevatorSpinBox->setRange(1, 20);
    ui->floorSpinBox->setRange(2, 20);

    // connect floor panel buttons to floor panel slots
    connect(ui->upButton, SIGNAL(clicked()), this, SLOT(upButton()));
    connect(ui->downButton, SIGNAL(clicked()), this, SLOT(downButton()));

    // connect elevator panel buttons to elevator panel slots
    connect(ui->requestRideButton, SIGNAL(clicked()), this, SLOT(requestRideButton()));

    // connect help button
    connect(ui->helpButton, SIGNAL(clicked()), this, SLOT(helpButton()));

    // connect open door and close button
    connect(ui->openDoorButton, SIGNAL(pressed()), this, SLOT(openDoorButton()));
    connect(ui->closeDoorButton, SIGNAL(clicked()), this, SLOT(closeDoorButton()));

    // connect overload button
    connect(ui->overloadButton, SIGNAL(pressed()), this, SLOT(overload()));
    connect(ui->overloadButton, SIGNAL(released()), this, SLOT(unOverload()));

    // connect door block button
    connect(ui->doorBlockButton, SIGNAL(pressed()), this, SLOT(doorBlock()));
    connect(ui->doorBlockButton, SIGNAL(released()), this, SLOT(unblockDoor()));

    // connect fire button and power outage button
    connect(ui->fireButton, SIGNAL(clicked()), this, SLOT(fireButton()));
    connect(ui->powerOutageButton, SIGNAL(clicked()), this, SLOT(powerOutageButton()));
}

void MainWindow::startSimulation()
{
    // get number of elevators and floors
    elevatorCount = ui->elevatorSpinBox->value();
    floorCount = ui->floorSpinBox->value();

    // reset system log
    ui->systemLog->clear();

    QString q = "Starting Simulation With " + QString::number(elevatorCount) + " Elevators And " + QString::number(floorCount) + " Floors.";
    appendSystemLog(q, -1);

    // disable button and list widgets after user clicks button
    ui->startSimulationButton->setEnabled(false);
    ui->elevatorSpinBox->setEnabled(false);
    ui->floorSpinBox->setEnabled(false);

    // enable all other buttons
    ui->endSimulationButton->setEnabled(true);
    ui->upButton->setEnabled(true);
    ui->downButton->setEnabled(true);
    ui->requestRideButton->setEnabled(true);
    ui->helpButton->setEnabled(true);
    ui->openDoorButton->setEnabled(true);
    ui->closeDoorButton->setEnabled(true);
    ui->overloadButton->setEnabled(true);
    ui->doorBlockButton->setEnabled(true);
    ui->fireButton->setEnabled(true);
    ui->powerOutageButton->setEnabled(true);

    // update spinbox with number of elevators and floors
    ui->floorPanelSpinBox->setRange(1, floorCount);
    ui->elevatorPanelCarSpinBox->setRange(1, elevatorCount);
    ui->elevatorPanelFloorSpinBox->setRange(1, floorCount);
    ui->floorPanelSpinBox->setEnabled(true);
    ui->elevatorPanelCarSpinBox->setEnabled(true);
    ui->elevatorPanelFloorSpinBox->setEnabled(true);

    // create elevator control system
    ecs = new ElevatorControlSystem(elevatorCount, floorCount);

    // create elevators
    for (int i = 0; i < elevatorCount; i++)
    {
        // start elevators at random floor
        int randomFloor = rand() % floorCount + 1;
        ElevatorCar *elevator = new ElevatorCar(i + 1, randomFloor, ecs);
        elevators.push_back(elevator);
        ecs->addElevator(elevator);
    }

    // connect ElevatorControlSystem systemLog signals to MainWindow appendSystemLog slots
    connect(ecs, &ElevatorControlSystem::log, this, &MainWindow::appendSystemLog);

    // display elevators position
    for (int i = 0; i < elevatorCount; i++)
    {
        QString q = "Elevator " + QString::number(i + 1) + " is idle at floor " + QString::number(elevators[i]->getCurrentFloor()) + ".";
        appendSystemLog(q, i + 1);
    }
    appendSystemLog("", -1);
}

void MainWindow::endSimulation()
{
    // disable all buttons
    ui->endSimulationButton->setEnabled(false);
    ui->upButton->setEnabled(false);
    ui->downButton->setEnabled(false);
    ui->requestRideButton->setEnabled(false);
    ui->helpButton->setEnabled(false);
    ui->openDoorButton->setEnabled(false);
    ui->closeDoorButton->setEnabled(false);
    ui->overloadButton->setEnabled(false);
    ui->doorBlockButton->setEnabled(false);
    ui->fireButton->setEnabled(false);
    ui->powerOutageButton->setEnabled(false);

    // enable start simulation button
    ui->startSimulationButton->setEnabled(true);
    ui->elevatorSpinBox->setEnabled(true);
    ui->floorSpinBox->setEnabled(true);
    ui->floorPanelSpinBox->setEnabled(false);
    ui->elevatorPanelCarSpinBox->setEnabled(false);
    ui->elevatorPanelFloorSpinBox->setEnabled(false);

    // delete elevator control system
    delete ecs;
    ecs = nullptr;

    // delete elevators
    for (int i = 0; i < elevators.size(); i++)
    {
        delete elevators[i];
    }
    elevators.clear();

    appendSystemLog("Simulation Ended. You can start the simulation again to reset the elevators.\n", -1);
}

void MainWindow::appendSystemLog(QString q, int elevatorNumber)
{
    // reset color to default
    ui->systemLog->setTextColor(QColor(0,0,0)); // TODO: change to default color
    if (elevatorNumber != -1)
    {
        // switch case to change color of elevator number
        switch (elevatorNumber)
        {
        case 1:
            // set color to 00C0FF
            ui->systemLog->setTextColor(QColor(0, 192, 255));
            break;
        case 2:
            // set color to 0040FF
            ui->systemLog->setTextColor(QColor(0, 64, 255));
            break;

        case 3:
            // set color to 0040FF
            ui->systemLog->setTextColor(QColor(0, 192, 0));
            break;
        default:
            ui->systemLog->setTextColor(QColor(255, 0, 0));
            break;
        }
    }
    ui->systemLog->append(q);
}

void MainWindow::upButton()
{
    int floor = ui->floorPanelSpinBox->value();

    // make up button disabled if floor is top floor
    if (floor == floorCount)
    {
        qInfo("Cannot press up button on top floor.");
        return;
    }

    QString q = "Floor " + QString::number(floor) + " up button pressed.";
    appendSystemLog(q, -1);

    // send floor request to elevator control system
    ecs->floorRequest(floor, direction::up);
}

void MainWindow::downButton()
{
    int floor = ui->floorPanelSpinBox->value();

    // make down button disabled if floor is bottom floor
    if (floor == 1)
    {
        qInfo("Cannot press down button on bottom floor.");
        return;
    }
    QString q = "Floor " + QString::number(floor) + " down button pressed.";
    appendSystemLog(q, -1);

    // send floor request to elevator control system
    ecs->floorRequest(floor, direction::down);
}

void MainWindow::requestRideButton()
{
    int car = ui->elevatorPanelCarSpinBox->value();
    int floor = ui->elevatorPanelFloorSpinBox->value();
    QString q = "Car " + QString::number(car) + " requested to go to floor " + QString::number(floor) + ".";
    appendSystemLog(q, -1);

    // send car request to elevator
    elevators[car - 1]->carRequest(floor);
}

void MainWindow::helpButton()
{
    int car = ui->elevatorPanelCarSpinBox->value();
    QString q = "Help button pressed by passenger in car " + QString::number(car) + ".";
    appendSystemLog(q, -1);

    // send help request to elevator
    elevators[car - 1]->helpRequest();
}

void MainWindow::openDoorButton()
{
    int car = ui->elevatorPanelCarSpinBox->value();
    int floor = ui->elevatorPanelFloorSpinBox->value();

    // send open door request to elevator
    elevators[car - 1]->openDoorRequest();
}

void MainWindow::closeDoorButton()
{
    int car = ui->elevatorPanelCarSpinBox->value();
    int floor = ui->elevatorPanelFloorSpinBox->value();

    // send close door request to elevator
    elevators[car - 1]->closeDoorRequest();
}

void MainWindow::doorBlock()
{
    int car = ui->elevatorPanelCarSpinBox->value();

    // send door block request to elevator
    elevators[car - 1]->setDoorBlocked(true);
}

void MainWindow::unblockDoor()
{
    int car = ui->elevatorPanelCarSpinBox->value();

    // send door unblock request to elevator
    elevators[car - 1]->setDoorBlocked(false);
}

void MainWindow::overload()
{
    int car = ui->elevatorPanelCarSpinBox->value();

    // send overload request to elevator
    elevators[car - 1]->setOverload(true);
}

void MainWindow::unOverload()
{
    int car = ui->elevatorPanelCarSpinBox->value();

    // send unoverload request to elevator
    elevators[car - 1]->setOverload(false);
}

void MainWindow::fireButton()
{
    // send fire alarm to elevator control system
    ecs->fireAlarm();

    // log fire alarm
    appendSystemLog("\nDue to a fire alarm, all elevators operations have been suspended until further notice.\n", -1);

    endSimulation();
}

void MainWindow::powerOutageButton()
{
    // send power outage to elevator control system
    ecs->powerOutage();

    // log power outage
    appendSystemLog("\nDue to a power outage, all elevators operations have been suspended until further notice.\nYou can start the simulation again to reset the elevators.\n", -1);

    endSimulation();
}

MainWindow::~MainWindow()
{
    delete ecs;
    delete ui;
    for (int i = 0; i < elevators.size(); i++)
    {
        delete elevators[i];
    }
}
