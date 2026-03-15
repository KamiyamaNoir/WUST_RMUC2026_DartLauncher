#include "Launcher.hpp"
#include "bsp_core_timer.hpp"
#include "usb_device.h"
#include "bsp_et16s.hpp"
#include "bsp_peripheral_stepper.hpp"
#include "ReloaderControl.hpp"
#include "TrailerControl.hpp"
#include <Asynchronous.hpp>

#include "LaunchControl.hpp"

enum
{
    RC_CHANNEL_CALIB = 4,
    RC_CHANNEL_RELOAD = 5,
    RC_CHANNEL_MODE = 6,
    RC_CHANNEL_ACTION_CANCEL = 7,
    RC_CHANNEL_LAUNCH = 11,
};

enum
{
    MODE_MAUNUAL,
    MODE_AUTO,
} _mode;

enum
{
    ACTION_NONE,
    ACTION_CALIB,
    ACTION_RELOAD,
    ACTION_LAUNCH,
} _action;

static void Action() {
    auto& rc = ET16S::Get();
    uint8_t channel;
    switch (_action) {
    case ACTION_CALIB:
        channel = RC_CHANNEL_CALIB;break;
    case ACTION_RELOAD:
        channel = RC_CHANNEL_RELOAD;break;
    default:
        channel = RC_CHANNEL_LAUNCH;break;
    };

    if (rc.channel[channel] > -0.8) {
        return;
    }

    if (_action == ACTION_CALIB) {
        TrailerControl::ZeroPosCaliberate();
    }
    else if (_action == ACTION_RELOAD) {
        LaunchControl::Next();
    }
    else if (_action == ACTION_LAUNCH) {
        TrailerControl::TriggerOn();
        HAL_Delay(1000);
        TrailerControl::TriggerOff();
    }

    _action = ACTION_NONE;
}

void Launcher::Run()
{
    TrailerControl::Setup();
    ReloaderControl::Setup();

    CoreTimer::SetupTimebase();

    ZDT_X42::Enable(true);

    ReloaderControl::PowerOn();

    Asynchronous report([] {
        for (;;) {
            printf("%f,%f\r\n", TrailerControl::GetCarPosition(), TrailerControl::GetTriggerPositon());
            HAL_Delay(20);
        }
    }, "report", 2);

    for (;;) {
        auto& rc = ET16S::Get();

        if (_mode == MODE_AUTO && _action != ACTION_NONE) {
            Action();
        }
        else if (_mode == MODE_MAUNUAL) {
            _action = ACTION_NONE;
            if (rc.channel[10] > 0) {
                ReloaderControl::HoverDown();
            }
            else {
                ReloaderControl::HoverUp();
            }

            if (rc.channel[11] > 0) {
                TrailerControl::TriggerOn();
            }
            else {
                TrailerControl::TriggerOff();
            }
        }

        TrailerControl::SetCarSpeed(rc.channel[1] * 100);
        TrailerControl::SetTriggerSpeed(rc.channel[0] * 400);
        ZDT_X42::SetSpeed(rc.channel[3] * 1000);

        HAL_Delay(10);
    }
}

void Launcher::Timebase_1ms() {
    TrailerControl::CalcusInvoke();
    ReloaderControl::CalcusInvoke();
}

void Launcher::RC_OnChannelChanged() {
    auto& rc = ET16S::Get();
    // System Reset
    if (rc.channel[13] > 0) {
        __disable_irq();
        NVIC_SystemReset();
    }
    // Set mode
    if (rc.channel[RC_CHANNEL_MODE] > 0) {
        _mode = MODE_AUTO;
    }
    else {
        _mode = MODE_MAUNUAL;
    }
    // Calcel action
    if (rc.channel[RC_CHANNEL_ACTION_CANCEL] > -0.8) {
        _action = ACTION_NONE;
        return;
    }

    //// Action List ////
    if (rc.channel[RC_CHANNEL_CALIB] > 0.8) {
        _action = ACTION_CALIB;
    }
    if (rc.channel[RC_CHANNEL_RELOAD] > 0.8) {
        _action = ACTION_RELOAD;
    }
    if (rc.channel[RC_CHANNEL_LAUNCH] > 0.8) {
        _action = ACTION_LAUNCH;
    }
}
