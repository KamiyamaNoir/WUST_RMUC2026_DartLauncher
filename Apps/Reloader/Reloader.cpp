#include "Reloader.hpp"
#include "bsp_drive.hpp"
#include "Reloader/HoverControl.hpp"

// 俩限位舵机的通道以及限位角度微调
static constexpr PWM_Channel _Servo_RLocker = PWM_CHANNEL_C;
static constexpr PWM_Channel _Servo_Locker1 = PWM_CHANNEL_D;
static constexpr PWM_Channel _Servo_Locker2 = PWM_CHANNEL_E;

static constexpr float _PreServo_Lock_Angle = 20.0f;
static constexpr float _PreServo_Unlock_Angle = 90.0f;
static constexpr float _PostServo_Lock_Angle = 20.0f;
static constexpr float _PostServo_Unlock_Angle = 90.0f;

static ReloaderProcess _process = RELOADER_INSTALL_4_READY;

void Reloader::Setup() {
    HoverControl::Setup();
    CorePWM::SetAngle(_Servo_RLocker, _PostServo_Lock_Angle);
    CorePWM::SetAngle(_Servo_Locker1, _PreServo_Lock_Angle);
    CorePWM::SetAngle(_Servo_Locker2, _PreServo_Lock_Angle);
    CorePWM::Start(_Servo_RLocker);
    CorePWM::Start(_Servo_Locker1);
    CorePWM::Start(_Servo_Locker2);
}

void Reloader::ResetAndInstall() {
    _process = RELOADER_INSTALL_0;
    // Release all locker and relay
    SetRelay(RELAY_1, false);
    SetRelay(RELAY_2, false);
    SetRelay(RELAY_3, false);
    SetRelay(RELAY_4, false);
    CorePWM::SetAngle(_Servo_RLocker, _PostServo_Unlock_Angle);
    CorePWM::SetAngle(_Servo_Locker1, _PreServo_Unlock_Angle);
    CorePWM::SetAngle(_Servo_Locker2, _PreServo_Unlock_Angle);
}

void Reloader::StepNext() {
    if (_process == RELOADER_RELEASE_4_STOP)
        return;

    if (_process < RELOADER_INSTALL_4_READY) {
        // Install step
        CorePWM::SetAngle(_Servo_Locker1, _PreServo_Lock_Angle);
        CorePWM::SetAngle(_Servo_Locker2, _PreServo_Lock_Angle);
        CorePWM::SetAngle(_Servo_RLocker, _PostServo_Lock_Angle);
        SetRelay(static_cast<RelayID>(_process), true);
        _process = static_cast<ReloaderProcess>(_process + 1);
        return;
    }

    uint8_t current = _process - RELOADER_INSTALL_4_READY;

    // Release Locker1
    CorePWM::SetAngle(_Servo_Locker1, _PreServo_Unlock_Angle);
    HAL_Delay(1000);
    // Relock
    CorePWM::SetAngle(_Servo_Locker1, _PreServo_Lock_Angle);
    HAL_Delay(800);

    // Lower hover
    HoverControl::GoTo(HOVER_ON_LOWER);
    while (HoverControl::Get() != HOVER_ON_LOWER) {
        HAL_Delay(1);
    }
    HAL_Delay(1000);

    // Turn off relay
    SetRelay(static_cast<RelayID>(current), false);
    HAL_Delay(1000);

    // Upper hover
    HoverControl::GoTo(HOVER_ON_UPPER);

    // Reload fine, add up step
    _process = static_cast<ReloaderProcess>(_process + 1);
    // When enter the final step, return
    if (_process == RELOADER_RELEASE_4_STOP)
        return;

    // Other, prepare for next step
    while (HoverControl::Get() != HOVER_ON_UPPER) {
        HAL_Delay(1);
    }
    HAL_Delay(1000);
    CorePWM::SetAngle(_Servo_RLocker, _PostServo_Unlock_Angle);
    HAL_Delay(800);
    CorePWM::SetAngle(_Servo_RLocker, _PostServo_Lock_Angle);

    // Release Locker2
    CorePWM::SetAngle(_Servo_Locker2, _PreServo_Unlock_Angle);
    HAL_Delay(1000);
    // Relock
    CorePWM::SetAngle(_Servo_Locker2, _PreServo_Lock_Angle);
    HAL_Delay(800);
}

void Reloader::SetRelay(RelayID id, bool on) {
    GPIO_TypeDef * gpio;
    uint32_t pin;
    switch (id) {
    case RELAY_1:
        gpio = RELAY1_GPIO_Port;
        pin = RELAY1_Pin;
        break;
    case RELAY_2:
        gpio = RELAY2_GPIO_Port;
        pin = RELAY2_Pin;
        break;
    case RELAY_3:
        gpio = RELAY3_GPIO_Port;
        pin = RELAY3_Pin;
        break;
    default:
        gpio = RELAY4_GPIO_Port;
        pin = RELAY4_Pin;
        break;
    }
    if (on) {
        gpio->BSRR = pin << 16;
    }
    else {
        gpio->BSRR = pin;
    }
}

