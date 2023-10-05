#ifndef ELEVATORCAR_H
#define ELEVATORCAR_H

#include "enums.h"
#include <algorithm>
#include <QObject>
#include <QTimer>
#include <QDebug>
#include <QVector>

class ElevatorControlSystem;

class ElevatorCar : public QObject
{
    Q_OBJECT
private:
    int elevatorNumber;

    int currentFloorPosition;
    elevatorState elevatorStatus;
    int destinationFloor;

    bool overload;
    bool doorBlocked;
    doorStatus doorState;

    QVector<int> floorQueue;

    ElevatorControlSystem *ecs;
    int allocationStrategy = 1; // 0 = queue, 1 = direction based
    QTimer *helpTimer;
    QTimer *doorTimer;
    QTimer *moveTimer;

    void sortFloorQueue();
    void openDoor();

private slots:
    void handleHelpTimer();
    void closeDoor();
    void moveToFloor();

public:
    ElevatorCar(int number, int currentFloorPosition, ElevatorControlSystem *ecs);

    elevatorState getState() { return elevatorStatus; }

    int getCurrentFloor() { return currentFloorPosition; }

    void addFloorToQueue(int floor) { floorQueue.push_back(floor); }

    void setOverload(bool overload);

    void setDoorBlocked(bool doorBlocked);

    void stop();

    void move(int floor);

    void moveToSafeFloor();

    void carRequest(int floor);

    void emergency(emergencyType type);

    void helpRequest();

    void closeDoorRequest();

    void openDoorRequest();
};

#endif // ELEVATORCAR_H

// get uml format for this class like this:

// -elevatorNumber: int
// -currentFloorPosition: int
// -elevatorStatus: elevatorState
// -destinationFloor: int
// -overload: bool
// -doorBlocked: bool
// -doorState: doorStatus
// -floorQueue: QVector<int>
// -helpTimer: QTimer
// -doorTimer: QTimer
// -moveTimer: QTimer

// +addFloorToQueue(int)
// +stop()
// +move(int)
// +moveToSafeFloor()
// +carRequest(int)
// +emergency(emergencyType)
// +helpRequest()
// +closeDoorRequest()
// +openDoorRequest()
// +sortFloorQueue()
// +openDoor()
// +closeDoor()
// +handleHelpTimer()
// +moveToFloor()
