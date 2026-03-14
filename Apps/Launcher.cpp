#include "Launcher.hpp"
#include "Asynchronous.hpp"
#include "bsp_core_timer.hpp"
#include "usb_device.h"
#include "bsp_et16s.hpp"
#include "bsp_peripheral_stepper.hpp"
#include "TrailerControl.hpp"
#include "Reloader/HoverControl.hpp"
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

enum RC_Action
{
    RC_ACTION_NONE,
    RC_ACTION_GO_LOCK,
    RC_ACTION_RELEASE,
    RC_ACTION_NEXT_RELOAD,
    RC_ACTION_CALIB,
    RC_ACTION_INSTALL_MODE,
};

static volatile RC_Action _action = RC_ACTION_NONE;
static volatile LauncherMode _mode = MODE_MANUAL;

void Launcher::Run()
{
    Reloader::Setup();
    TrailerControl::Setup();

    CoreTimer::SetupTimebase();

    ZDT_X42::Enable(true);

    // Asynchronous report([] {
    //     for (;;) {
    //         printf("%f,%f\r\n", TrailerControl::CarGetPositon(), TrailerControl::TriggerGetPositon());
    //         HAL_Delay(50);
    //     }
    // }, "report", 2);

    for (;;) {
        auto rc = ET16S::Get();

        if (_mode == MODE_MANUAL) {
            // Manual Control
            TrailerControl::CarSetSpeed(100* rc.channel[1]);
            TrailerControl::TriggerSetSpeed(400* rc.channel[0]);
            ZDT_X42::SetSpeed(rc.channel[3]*1000);
            if (rc.channel[RC_CHANNEL_HOVER] < 0) {
                HoverControl::GoTo(HOVER_ON_UPPER);
            }
            else {
                HoverControl::GoTo(HOVER_ON_LOWER);
            }
        }
        else if (_mode == MODE_AUTO) {
            // Action
            if (_action != RC_ACTION_NONE) {
                // Action Caliberation
                if (rc.channel[RC_CHANNEL_CALIB_INSTALL] < -0.8 && _action == RC_ACTION_CALIB) {
                    TrailerControl::ZeroPosCaliberate();
                    _action = RC_ACTION_NONE;
                }
                // Action Install mode
                else if (rc.channel[RC_CHANNEL_CALIB_INSTALL] < -0.8 && _action == RC_ACTION_INSTALL_MODE) {
                    Reloader::ResetAndInstall();
                    _action = RC_ACTION_NONE;
                }
                // Action Reload next step
                else if (rc.channel[RC_CHANNEL_NEXT_SETP] < -0.8 && _action == RC_ACTION_NEXT_RELOAD) {
                    Reloader::StepNext();
                    _action = RC_ACTION_NONE;
                }
                // Action Launch
                else if (rc.channel[RC_CHANNEL_RELEASE] < -0.8 && _action == RC_ACTION_RELEASE) {
                    TrailerControl::TriggerOn();
                    HAL_Delay(1000);
                    TrailerControl::TriggerOff();
                    _action = RC_ACTION_NONE;
                }
                // Action Lock
                else if (rc.channel[RC_CHANNEL_LOCK] < -0.8 && _action == RC_ACTION_GO_LOCK) {
                    CancellationToken _token = {};
                    bool ready = false;
                    Asynchronous lock([&_token, &ready]() {
                        TrailerControl::GoLock(_token);
                        ready = true;
                        for (;;) {}
                    }, "lock", 3);
                    while (!ready) {
                        if (rc.channel[RC_CHANNEL_LOCK] > 0.8) {
                            _token.cancel = true;
                            break;
                        }
                        HAL_Delay(10);
                    }
                    _action = RC_ACTION_NONE;
                }
            }
        }
        else {
            // Stop mode
            TrailerControl::CarSetSpeed(0);
            TrailerControl::TriggerSetSpeed(0);
            ZDT_X42::SetSpeed(0);
        }
        HAL_Delay(20);
    }
}

void Launcher::Timebase_1ms() {
    TrailerControl::CalcusInvoke();
}

void Launcher::RC_OnChannelChanged() {
    auto data = ET16S::Get();
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
    // Cancel Action
    if (data.channel[RC_CHANNEL_CANCEL] > 0) {
        _action = RC_ACTION_NONE;
        return;
    }
    /** === Actions === **/
    RC_Action tAction = RC_ACTION_NONE;
    // Calib and Install
    if (data.channel[RC_CHANNEL_CALIB_INSTALL] > 0.8) {
        tAction = RC_ACTION_INSTALL_MODE;
    }
    else if (data.channel[RC_CHANNEL_CALIB_INSTALL] > -0.2) {
        tAction = RC_ACTION_CALIB;
    }
    // Reload next step
    else if (data.channel[RC_CHANNEL_NEXT_SETP] > 0.8) {
        tAction = RC_ACTION_NEXT_RELOAD;
    }
    // Release
    else if (data.channel[RC_CHANNEL_RELEASE] > 0.8) {
        tAction = RC_ACTION_RELEASE;
    }
    // Lockl
    else if (data.channel[RC_CHANNEL_LOCK] > 0.8) {
        tAction = RC_ACTION_GO_LOCK;
    }

    if (tAction > _action) {
        _action = tAction;
    }
}
