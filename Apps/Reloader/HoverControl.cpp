#include "HoverControl.hpp"
#include "FreeRTOS.h"
#include "task.h"
#include "bsp_drive.hpp"

static float abs(float x) {
    return (x > 0) ? x : -x;
}

// 舵机通道
static constexpr PWM_Channel _ServoChannel = PWM_CHANNEL_A;
static constexpr PWM_Channel _ServoChannelR = PWM_CHANNEL_B;
// 舵机预期角度
static constexpr float _TargetAngleUpper = 180.0f;
static constexpr float _TargetAngleLower = 0.0f;
// 舵机动作大小
static constexpr float _move = 5.0f;

static volatile float _target = _TargetAngleUpper;
static volatile float _angle;

static void StartHoverTask(void * args) {
    for (;;) {
        if (_angle != _target) {
            float move = _target - _angle > 0 ? _move : -_move;

            if (abs(_target - _angle) < _move) {
                _angle = _target;
            }
            else {
                _angle += move;
            }

            CorePWM::SetAngle(_ServoChannel, _angle);
            CorePWM::SetAngle(_ServoChannelR, 180.0f - _angle);
        }
        HAL_Delay(50);
    }
}

void HoverControl::Setup() {
    _angle = _TargetAngleUpper;
    CorePWM::SetAngle(_ServoChannel, _angle);
    CorePWM::SetAngle(_ServoChannelR, 180.0f - _angle);
    CorePWM::Start(_ServoChannel);
    CorePWM::Start(_ServoChannelR);
    xTaskCreate(StartHoverTask, "hover", 512, nullptr, 2, nullptr);
}

void HoverControl::GoTo(HoverStatus status) {
    if (status == HOVER_ON_UPPER)
        _target = _TargetAngleUpper;
    else if (status == HOVER_ON_LOWER)
        _target = _TargetAngleLower;
}

HoverStatus HoverControl::Get() {
    if (_angle == _TargetAngleUpper)
        return HOVER_ON_UPPER;

    if (_angle == _TargetAngleLower)
        return HOVER_ON_LOWER;

    return HOVER_RUNNING;
}
