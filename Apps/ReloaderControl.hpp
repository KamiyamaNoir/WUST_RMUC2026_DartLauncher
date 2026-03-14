#ifndef LAUNCHERA_RELOADERCONTROL_HPP
#define LAUNCHERA_RELOADERCONTROL_HPP

class ReloaderControl
{
public:
    static void Setup();

    static void SetSpeed(float rpm);

    static void SetPositon(float angle);

    static void CalcusInvoke();
};

#endif //LAUNCHERA_RELOADERCONTROL_HPP