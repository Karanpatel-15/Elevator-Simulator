#ifndef ENUMS_H
#define ENUMS_H

enum class direction
{
    up,
    down
};

enum class doorStatus
{
    close,
    open
};

enum class emergencyType
{
    fire,
    powerOutage
};

enum class elevatorState
{
    up,
    down,
    idle,
    occupied
};

#endif // ENUMS_H
