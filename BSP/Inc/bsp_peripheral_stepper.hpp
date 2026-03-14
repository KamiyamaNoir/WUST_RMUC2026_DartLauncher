#ifndef LAUNCHERA_BSP_PERIPHERAL_STEPPER_HPP
#define LAUNCHERA_BSP_PERIPHERAL_STEPPER_HPP

class ZDT_X42
{
public:
    static void Enable(bool enable);
    static void SetSpeed(float speed);
    static void SetCurrent(float current);
    static void ZeroPointSet(bool save);
};

#endif //LAUNCHERA_BSP_PERIPHERAL_STEPPER_HPP