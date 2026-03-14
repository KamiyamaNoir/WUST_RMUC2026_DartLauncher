#ifndef MISSILELAUNCHER_LAUNCHER_H
#define MISSILELAUNCHER_LAUNCHER_H

enum LauncherMode
{
    MODE_STOP,
    MODE_MANUAL,
    MODE_AUTO,
};


class Launcher
{
public:
    static void Run();

    static void Timebase_1ms();

    static void RC_OnChannelChanged();
};

#endif //MISSILELAUNCHER_LAUNCHER_H