#ifndef LAUNCHERA_RELOADERCONTROL_HPP
#define LAUNCHERA_RELOADERCONTROL_HPP

enum ReloaderStage
{
    RELOADER_STAGE_1 = 0,
    RELOADER_STAGE_2,
    RELOADER_STAGE_3,
    RELOADER_STAGE_4,
};

class ReloaderControl
{
public:
    static void Setup();
    static void PowerOn();

    static void HoverUp();
    static void HoverDown();

    static void GoStage(ReloaderStage stage);

    static void CalcusInvoke();
};

#endif //LAUNCHERA_RELOADERCONTROL_HPP