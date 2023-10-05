#include "ElevatorCar.h"
#include "ElevatorControlSystem.h"

ElevatorCar::ElevatorCar(int number, int currentFloorPosition, ElevatorControlSystem *ecs) : elevatorNumber(number), ecs(ecs), currentFloorPosition(currentFloorPosition), elevatorStatus(elevatorState::idle)
{
    // set door open
    doorState = doorStatus::close;
    overload = false;
    doorBlocked = false;

    // initialize the help timer
    helpTimer = new QTimer(this);
    helpTimer->setInterval(2000);
    connect(helpTimer, SIGNAL(timeout()), this, SLOT(handleHelpTimer()));

    // initialize the door timer
    doorTimer = new QTimer(this);
    doorTimer->setInterval(10000);
    connect(doorTimer, SIGNAL(timeout()), this, SLOT(closeDoor()));

    // initialize the move timer
    moveTimer = new QTimer(this);
    moveTimer->setInterval(3000);
    connect(moveTimer, SIGNAL(timeout()), this, SLOT(moveToFloor()));
}

void ElevatorCar::setOverload(bool overload)
{
    // if doors are open
    if (doorState == doorStatus::open)
    {
        this->overload = overload;
        qInfo() << "Elevator " << elevatorNumber << ": Overload Set To " << overload;
        return;
    }
    qInfo() << "Elevator cannot be overloaded when doors are closed.";
}

void ElevatorCar::setDoorBlocked(bool doorBlocked)
{
    // if doors are open
    if (doorState == doorStatus::open)
    {
        this->doorBlocked = doorBlocked;
        qInfo() << "Elevator " << elevatorNumber << ": Door Blocked Set To " << doorBlocked;
        return;
    }
    qInfo() << "Elevator cannot be door blocked when doors are closed.";
}

void ElevatorCar::stop()
{
    // if doors are open do nothing
    // if elevator is idle do nothing
    // if elevator is moving stop elevator

    if (doorState == doorStatus::open)
    {
        qInfo() << "Elevator " << elevatorNumber << ": Cannot stop when doors are open.";
        return;
    }
    else if (elevatorStatus == elevatorState::idle)
    {
        qInfo() << "Elevator " << elevatorNumber << ": Cannot stop when elevator is idle.";
        return;
    }
    else
    {
        elevatorStatus = elevatorState::idle;
    }
}

void ElevatorCar::move(int floor)
{

    // figure out the direction
    if (floor > currentFloorPosition)
    {
        elevatorStatus = elevatorState::up;
    }
    else if (floor < currentFloorPosition)
    {
        elevatorStatus = elevatorState::down;
    }
    else
    {
        openDoor();
        return;
    }

    // set the destination floor
    destinationFloor = floor;

    // timer to move floor by floor
    moveTimer->start();
}

void ElevatorCar::moveToFloor()
{

    if (currentFloorPosition == destinationFloor)
    {
        moveTimer->stop();
        elevatorStatus = elevatorState::idle;
        ecs->inform("Elevator " + QString::number(elevatorNumber) + ": Reached Destination Floor " + QString::number(currentFloorPosition), elevatorNumber);
        openDoor();
        return;
    }
    else if (elevatorStatus == elevatorState::up)
    {
        currentFloorPosition++;
        ecs->inform("Elevator " + QString::number(elevatorNumber) + ": Moved Up To Floor " + QString::number(currentFloorPosition), elevatorNumber);
    }
    else if (elevatorStatus == elevatorState::down)
    {
        currentFloorPosition--;
        ecs->inform("Elevator " + QString::number(elevatorNumber) + ": Moved Down To Floor " + QString::number(currentFloorPosition), elevatorNumber);
    }
}

void ElevatorCar::moveToSafeFloor()
{
    // if doors are open do nothing
    // if elevator is idle do nothing
    // if elevator is moving go one floor down unless already at ground floor

    if (doorState == doorStatus::open)
    {
        qInfo() << "Elevator " << elevatorNumber << ": Cannot move to same floor when doors are open.";
        return;
    }
    else if (elevatorStatus == elevatorState::idle)
    {
        qInfo() << "Elevator " << elevatorNumber << ": Cannot move to same floor when elevator is idle.";
        return;
    }
    else
    {
        moveTimer->stop();
        if (currentFloorPosition == 1)
        {
            elevatorStatus = elevatorState::idle;
            ecs->inform("Elevator " + QString::number(elevatorNumber) + ": At Emergency Exit Floor " + QString::number(currentFloorPosition), elevatorNumber);
            openDoor();
            return;
        }
        currentFloorPosition--;
        ecs->inform("Elevator " + QString::number(elevatorNumber) + ": Moved Down Emergency Exit To Floor " + QString::number(currentFloorPosition), elevatorNumber);
    }
}

void ElevatorCar::sortFloorQueue()
{
    int first = floorQueue.at(0);
    int min, max;
    if (currentFloorPosition <= first)
    {
        min = currentFloorPosition;
        max = first;
    }
    else
    {
        min = first;
        max = currentFloorPosition;
    }
    QVector<int> firstHalf, secondHalf;
    for (int i = 0; i < floorQueue.size(); i++)
    {
        int num = floorQueue[i];
        if (num >= min && num <= max)
        {
            firstHalf.push_back(num);
        }
        else
        {
            secondHalf.push_back(num);
        }
    }

    if (currentFloorPosition <= first)
    {
        sort(firstHalf.begin(), firstHalf.end());
    }
    else
    {
        sort(firstHalf.begin(), firstHalf.end(), greater<int>());
    }
    // concatenate the two vectors
    floorQueue = firstHalf + secondHalf;
}

void ElevatorCar::openDoor()
{
    // cancel any existing timer
    if (doorTimer->isActive())
    {
        doorTimer->stop();
    }

    // open the door
    ecs->inform("Elevator " + QString::number(elevatorNumber) + ": Bell Ringed, Door Opened.", elevatorNumber);
    doorState = doorStatus::open;
    elevatorStatus = elevatorState::occupied;

    // start a new timer
    doorTimer->start();
}

void ElevatorCar::closeDoor()
{
    // cancel any existing timer
    if (doorTimer->isActive())
    {
        doorTimer->stop();
    }

    // check for obstruction and overload
    if (doorBlocked)
    {
        ecs->inform("Elevator " + QString::number(elevatorNumber) + ": door blocked. Cannot close.", elevatorNumber);
        openDoor();
        return;
    }
    else if (overload)
    {
        ecs->inform("Elevator " + QString::number(elevatorNumber) + ": Passenger Overload. Cannot close.", elevatorNumber);
        openDoor();
        return;
    }

    // close the door
    ecs->inform("Elevator " + QString::number(elevatorNumber) + ": Bell Ringed, Door Closed.", elevatorNumber);
    doorState = doorStatus::close;

    // check if there are any more floors to visit
    if (floorQueue.size() > 0)
    {
        if (allocationStrategy == 1)
        {
            sortFloorQueue();
        }
        move(floorQueue[0]);
        floorQueue.remove(0);
    }
    else
    {
        // stop the elevator
        stop();
        ecs->checkFloorRequests(elevatorNumber);
        // see if there are any floor Requests to service
    }
}

void ElevatorCar::openDoorRequest()
{
    // is door timer running?
    if (doorTimer->isActive())
    {
        ecs->inform("Elevator " + QString::number(elevatorNumber) + " Door Opened by Passengers Request.", elevatorNumber);
        openDoor();
        return;
    }
    qInfo() << "Elevator " << elevatorNumber << ": Can Not Open Door. Elevator Is Moving.";
}

void ElevatorCar::closeDoorRequest()
{
    if (doorTimer->isActive())
    {
        // check for obstruction and overload
        if (doorBlocked)
        {
            ecs->inform("Elevator " + QString::number(elevatorNumber) + ": Door Blocked. Can Not close.", elevatorNumber);
            openDoor();
            return;
        }
        else if (overload)
        {
            ecs->inform("Elevator " + QString::number(elevatorNumber) + ": Overloaded. Can Not close.", elevatorNumber);
            openDoor();
            return;
        }

        ecs->inform("Elevator " + QString::number(elevatorNumber) + ": Door Closed By Passengers Request.", elevatorNumber);
        closeDoor();
        return;
    }
    qInfo() << "Elevator " << elevatorNumber << ": Door Already Closed Or Elevator Is Moving.";
}

void ElevatorCar::carRequest(int floor)
{
    // Code to handle a car request
    ecs->carRequest(elevatorNumber, floor);
}

void ElevatorCar::helpRequest()
{
    // stop the elevator
    stop();

    // inform the elevator control system and log the help request
    ecs->inform("Elevator " + QString::number(elevatorNumber) + ": Help Button Pressed. Ringing For Help. Elevator Stopped.", elevatorNumber);

    helpTimer->start();
}

void ElevatorCar::handleHelpTimer()
{

    // stop the help timer
    helpTimer->stop();

    // simulate trying to call building maintenance
    if (rand() % 2 == 0)
    {
        // call building maintenance
        ecs->inform("Elevator " + QString::number(elevatorNumber) + ": Connected To Building Maintenance.", elevatorNumber);
    }
    else
    {
        // building maintenance is busy
        ecs->inform("Elevator " + QString::number(elevatorNumber) + ": Building Maintenance Was Busy. Instead Established Connection With 911.", elevatorNumber);
    }
}

void ElevatorCar::emergency(emergencyType type)
{

    // if doors are open
    if (doorState == doorStatus::open)
    {
        // disable the door timer
        doorTimer->stop();
        // instruct the passenger to exit the elevator
        ecs->inform("Elevator " + QString::number(elevatorNumber) + " audio/visual message playing: Please exit the elevator there is an emergency.", elevatorNumber);
        return;
    }

    if (type == emergencyType::fire)
    {
        ecs->inform("Elevator " + QString::number(elevatorNumber) + ": Fire Emergency. Moving To Nearest Floor.", elevatorNumber);
    }
    else if (type == emergencyType::powerOutage)
    {
        ecs->inform("Elevator " + QString::number(elevatorNumber) + ": Power Outage Emergency. Moving To Nearest Floor.", elevatorNumber);
    }

    // move to nearest floor
    moveToSafeFloor();

    // open the door
    openDoor();

    // stop door timer
    doorTimer->stop();

    // message the passengers to exit the elevator
    ecs->inform("Elevator " + QString::number(elevatorNumber) + " audio/visual message playing: Please exit the elevator there is an emergency.", elevatorNumber);
}
