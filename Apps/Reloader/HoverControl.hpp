#ifndef LAUNCHERA_HOVERCONTROL_HPP
#define LAUNCHERA_HOVERCONTROL_HPP

#include "main.h"

enum HoverStatus
{
    HOVER_ON_UPPER,
    HOVER_ON_LOWER,
    HOVER_RUNNING,
};

class HoverControl
{
public:
    static void Setup();

    static void GoTo(HoverStatus status);
    static HoverStatus Get();
};

#endif //LAUNCHERA_HOVERCONTROL_HPP