#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "ElevatorControlSystem.h"
#include "ElevatorCar.h"
#include "ui_mainwindow.h"
#include "enums.h"
#include <QObject>

namespace Ui
{
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startSimulation();
    void endSimulation();

    void upButton();
    void downButton();

    void requestRideButton();
    void helpButton();

    void openDoorButton();
    void closeDoorButton();

    void fireButton();
    void powerOutageButton();

    void doorBlock();
    void unblockDoor();

    void overload();
    void unOverload();

public slots:
    void appendSystemLog(QString q, int elevatorId);

private:
    Ui::MainWindow *ui;
    ElevatorControlSystem *ecs;
    QVector<ElevatorCar *> elevators;
    void setUpConnection();
};

#endif // MAINWINDOW_H
