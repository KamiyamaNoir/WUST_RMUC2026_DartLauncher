#include "Launcher.hpp"
#include "Asynchronous.hpp"
#include "bsp_core_timer.hpp"
#include "bsp_drive.hpp"
#include "usb_device.h"
#include "bsp_et16s.hpp"
#include "bsp_peripheral_stepper.hpp"
#include "bsp_peripheral_trailer.hpp"
#include "ReloaderControl.hpp"
#include "TrailerControl.hpp"

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
    TrailerControl::Setup();
    ReloaderControl::Setup();

    CoreTimer::SetupTimebase();

    ZDT_X42::Enable(true);

    CorePWM::SetAngle(PWM_CHANNEL_Z, 135);
    CorePWM::Start(PWM_CHANNEL_Z);

    PWR_CTR1_GPIO_Port->BSRR = PWR_CTR1_Pin;

    float target_angle = 0;

    for (;;) {
        auto& rc = ET16S::Get();

        if (rc.channel[4] > 0.8) {
            PWR_CTR2_GPIO_Port->BSRR = PWR_CTR2_Pin;
        }
        else {
            PWR_CTR2_GPIO_Port->BSRR = PWR_CTR2_Pin << 16;
        }

        target_angle += rc.channel[0] * 90.0f * 0.01f;
        // ReloaderControl::SetSpeed(rc.channel[0] * 200);
        ReloaderControl::SetPositon(target_angle);
        // printf("%f,%f\r\n", Trailer::GetReloader().motor_rpm, Trailer::GetReloader().total_angle_speed);
        printf("%f,%f\r\n", target_angle, Trailer::GetReloader().getTotalAngle());
        HAL_Delay(10);
    }
}

void Launcher::Timebase_1ms() {
    TrailerControl::CalcusInvoke();
    ReloaderControl::CalcusInvoke();
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
