#ifndef LAUNCHERA_TRAILERCONTROL_HPP
#define LAUNCHERA_TRAILERCONTROL_HPP

#include "main.h"

class TrailerControl
{
public:
    static void Setup();

    static void CarPowerLimit(float load, float release);
    static void CarSetSpeed(float rpm);
    static void CarGoToPosition(float pos);
    static float CarGetPositon();
    static void CarResetPosition();

    static void TriggerPowerLimit(float up, float down);
    static void TriggerSetSpeed(float rpm);
    static void TriggerGoToPosition(float pos);
    static float TriggerGetPositon();
    static void TriggerResetPosition();

    static void TriggerOn();
    static void TriggerOff();

    static void GoLock(CancellationToken& token);

    static void ZeroPosCaliberate();

    static void CalcusInvoke();
};

#endif //LAUNCHERA_TRAILERCONTROL_HPP