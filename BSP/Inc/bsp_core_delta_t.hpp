#ifndef LAUNCHERA_BSP_CORE_DELTA_T_HPP
#define LAUNCHERA_BSP_CORE_DELTA_T_HPP

#include "main.h"

class DeltaT
{
public:
    static void Setup();

    DeltaT();

    uint32_t us();
    uint32_t lastTick;
};

#endif //LAUNCHERA_BSP_CORE_DELTA_T_HPP