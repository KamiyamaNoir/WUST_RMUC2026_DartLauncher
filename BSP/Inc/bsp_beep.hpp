#ifndef LAUNCHERA_BSP_BEEP_HPP
#define LAUNCHERA_BSP_BEEP_HPP

class Beep
{
public:
    static void Start();
    static void Stop();
    static void Set(float freq);

    // From 0.0 - 1.0
    static void Volume(float vol);
};

#endif //LAUNCHERA_BSP_BEEP_HPP