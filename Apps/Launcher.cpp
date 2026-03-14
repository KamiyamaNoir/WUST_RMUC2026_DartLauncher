#include "Launcher.hpp"
#include "Asynchronous.hpp"
#include "bsp_core_timer.hpp"
#include "bsp_drive.hpp"
#include "usb_device.h"
#include "bsp_et16s.hpp"
#include "bsp_peripheral_stepper.hpp"
#include "bsp_peripheral_trailer.hpp"
#include "TrailerControl.hpp"
#include "Reloader/Reloader.hpp"

enum
{
    RC_CHANNEL_CALIB_INSTALL = 4,
    RC_CHANNEL_CANCEL = 7,
    RC_CHANNEL_NEXT_SETP = 5,
    RC_CHANNEL_RELEASE = 13,
    RC_CHANNEL_MODE = 6,
    RC_CHANNEL_LOCK = 11,
    RC_CHANNEL_HOVER = 10,
};

static volatile LauncherMode _mode = MODE_MANUAL;

void Launcher::Run()
{
    Reloader::Setup();
    TrailerControl::Setup();

    CoreTimer::SetupTimebase();

    ZDT_X42::Enable(true);

    Asynchronous report([] {
        for (;;) {
            // printf("%f,%f\r\n", TrailerControl::CarGetPositon(), TrailerControl::TriggerGetPositon());
            printf("%f\r\n", Trailer::GetReloader().getTotalAngle());
            HAL_Delay(50);
        }
    }, "report", 2);

    CorePWM::SetAngle(PWM_CHANNEL_Z, 90);
    CorePWM::Start(PWM_CHANNEL_Z);

    PWR_CTR1_GPIO_Port->BSRR = PWR_CTR1_Pin;

    PWR_CTR2_GPIO_Port->BSRR = PWR_CTR2_Pin;

    for (;;) {
        auto& rc = ET16S::Get();
        Trailer::SetReloaderCurrent(rc.channel[0] * 3);
        HAL_Delay(20);
    }
}

void Launcher::Timebase_1ms() {
    TrailerControl::CalcusInvoke();
}

void Launcher::RC_OnChannelChanged() {
    auto& data = ET16S::Get();
    // Mode
    if (data.channel[RC_CHANNEL_MODE] < -0.2) {
        _mode = MODE_MANUAL;
    }
    else if (data.channel[RC_CHANNEL_MODE] > 0.2) {
        _mode = MODE_STOP;
    }
    else {
        _mode = MODE_AUTO;
    }
}
