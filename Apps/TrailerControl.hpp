#ifndef LAUNCHERA_TRAILERCONTROL_HPP
#define LAUNCHERA_TRAILERCONTROL_HPP

#include "main.h"

class TrailerControl
{
public:
    static void Setup();

    static void CarPowerLimit(float load, float release);
    static void SetCarSpeed(float rpm);
    static void CarGoToPosition(float pos);
    static float GetCarPosition();
    static void ResetCarPosition();

    static void TriggerPowerLimit(float up, float down);
    static void SetTriggerSpeed(float rpm);
    static void TriggerGoToPosition(float pos);
    static float GetTriggerPositon();
    static void ResetTriggerPosition();

    static void TriggerOn();
    static void TriggerOff();

    static void ZeroPosCaliberate();

    static void CalcusInvoke();
};

#endif //LAUNCHERA_TRAILERCONTROL_HPP