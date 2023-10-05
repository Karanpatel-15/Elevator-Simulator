#include "ElevatorControlSystem.h"

ElevatorControlSystem::ElevatorControlSystem(int numElevators, int numFloors) : numElevators(numElevators), numFloors(numFloors){};

void ElevatorControlSystem::addElevator(ElevatorCar *elevator)
{
    elevators.push_back(elevator);
}

void ElevatorControlSystem::floorRequest(int floor, direction dir)
{
    if (allocationStrategy == 0)
    {
        qInfo() << "Allocation strategy: random elevator";
        // randomly select an elevator that is idle
        QVector<int> idleElevators;
        for (int i = 0; i < numElevators; i++)
        {
            if (elevators[i]->getState() == elevatorState::idle)
            {
                idleElevators.push_back(i);
            }
        }
        if (idleElevators.size() > 0)
        {
            // send move command to a random idle elevator
            int randomElevator = idleElevators[rand() % idleElevators.size()];
            moveElevator(randomElevator + 1, floor);
        }
        else
        {
            // make floor request a pair of floor and direction
            floorRequests.push_back(make_pair(floor, dir));
        }
    }
    else if (allocationStrategy == 1)
    {
        qInfo() << "Allocation strategy: closest elevator";
        // find the closest idle elevator to the requested floor
        int minDistance = numFloors + 1;
        int closestElevator = -1;

        for (int i = 0; i < numElevators; i++)
        {
            if (elevators[i]->getState() == elevatorState::idle)
            {
                int distance = abs(elevators[i]->getCurrentFloor() - floor);
                if (distance < minDistance)
                {
                    minDistance = distance;
                    closestElevator = i;
                }
            }
        }

        if (closestElevator != -1)
        {
            // send move command to the closest idle elevator
            moveElevator(closestElevator + 1, floor);
        }
        else
        {
            // make floor request a pair of floor and direction
            floorRequests.push_back(make_pair(floor, dir));
        }
    }
}

void ElevatorControlSystem::checkFloorRequests(int elevatorId)
{
    // if there are no floor requests, return
    if (floorRequests.size() == 0)
    {
        return;
    }

    // find the closest floor request to the elevator
    int minDistance = numFloors + 1;
    int closestFloorRequest = -1;

    for (int i = 0; i < floorRequests.size(); i++)
    {
        int distance = abs(elevators[elevatorId - 1]->getCurrentFloor() - floorRequests[i].first);
        if (distance < minDistance)
        {
            minDistance = distance;
            closestFloorRequest = i;
        }
    }

    if (closestFloorRequest != -1)
    {
        // send move command to the closest floor request
        moveElevator(elevatorId, floorRequests[closestFloorRequest].first);
        // remove the floor request from the queue
        floorRequests.erase(floorRequests.begin() + closestFloorRequest);
    }
}

void ElevatorControlSystem::carRequest(int elevatorNumber, int floor)
{
    if (elevators[elevatorNumber - 1]->getState() == elevatorState::idle)
    {
        // if the elevator is idle, send move command to the elevator
        moveElevator(elevatorNumber, floor);
    }
    else
    {
        // if the elevator is not idle, add the request to the queue
        elevators[elevatorNumber - 1]->addFloorToQueue(floor);
    }
}

void ElevatorControlSystem::moveElevator(int elevatorId, int floor)
{
    // send move elevator to the elevator
    elevators[elevatorId - 1]->move(floor);
}

void ElevatorControlSystem::fireAlarm()
{
    // emit a signal to log the fire alarm
    emit log("Fire alarm activated.", -1);

    for (int i = 0; i < numElevators; i++)
    {
        // send fire alarm to each elevator
        elevators[i]->emergency(emergencyType::fire);
    }
}

void ElevatorControlSystem::powerOutage()
{
    // emit a signal to log the power outage
    emit log("Power outage detected.", -1);

    for (int i = 0; i < numElevators; i++)
    {
        // send power outage to each elevator
        elevators[i]->emergency(emergencyType::powerOutage);
    }
}

void ElevatorControlSystem::inform(QString q, int elevatorId)
{
    // emit a signal to log the inform
    emit log(q, elevatorId);
}

ElevatorControlSystem::~ElevatorControlSystem() {}
