#ifndef ELEVATORCONTROLSYSTEM_H
#define ELEVATORCONTROLSYSTEM_H

#include <QVector>
#include "enums.h"
#include <QDebug>
#include <QObject>
#include "ElevatorCar.h"

using namespace std;

class ElevatorControlSystem : public QObject
{
    Q_OBJECT
private:
    // QVector<pair<int, direction>> carRequests;
    QVector<pair<int, direction>> floorRequests;
    QVector<ElevatorCar *> elevators;
    int numElevators;
    int numFloors;
    int allocationStrategy = 1; // 0 = random elevator, 1 = closest elevator

    void moveElevator(int elevatorId, int floor);

signals:
    void log(QString q, int elevatorId);

public:
    ElevatorControlSystem(int numElevators, int numFloors);

    ~ElevatorControlSystem();

    void addElevator(ElevatorCar *elevator);

    void floorRequest(int floor, direction direction);

    void checkFloorRequests(int elevatorId);

    void carRequest(int elevatorNumber, int floor);

    void fireAlarm();

    void powerOutage();

    void inform(QString q, int elevatorId);
};

#endif // ELEVATORCONTROLSYSTEM_H

// get uml format for this class like this:

// -numElevators: int
// -numFloors: int
// -floorRequests: QVector<pair<int, direction>>

// +addElevator(ElevatorCar *)
// +floorRequest(int, direction)
// +checkFloorRequests(int)
// +carRequest(int, int)
// +fireAlarm()
// +powerOutage()
// +inform(QString, int)
// +log(QString, int)
// -moveElevator(int, int)
