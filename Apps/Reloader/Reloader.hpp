#ifndef LAUNCHERA_RELOADER_HPP
#define LAUNCHERA_RELOADER_HPP

#include "main.h"

enum ReloaderProcess
{
    RELOADER_INSTALL_0 = 0,
    RELOADER_INSTALL_1,
    RELOADER_INSTALL_2,
    RELOADER_INSTALL_3,
    RELOADER_INSTALL_4_READY,
    RELOADER_RELEASE_1,
    RELOADER_RELEASE_2,
    RELOADER_RELEASE_3,
    RELOADER_RELEASE_4_STOP,
};

enum RelayID
{
    RELAY_1 = 0,
    RELAY_2,
    RELAY_3,
    RELAY_4,
};

class Reloader
{
public:
    static void Setup();

    static void SetRelay(RelayID id, bool on);

    static void ResetAndInstall();

    static void StepNext();
};

#endif //LAUNCHERA_RELOADER_HPP