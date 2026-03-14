#ifndef LAUNCHERA_BSP_CORE_CDC_HPP
#define LAUNCHERA_BSP_CORE_CDC_HPP

#include "main.h"

class CDC
{
public:
    static void Transmit(const uint8_t* data, uint32_t len);
};

#endif //LAUNCHERA_BSP_CORE_CDC_HPP