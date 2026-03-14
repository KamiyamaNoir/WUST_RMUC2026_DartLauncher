#include "ReloaderControl.hpp"
#include "bsp_drive.hpp"
#include "bsp_peripheral_trailer.hpp"
#include "FeedbackControl.hpp"

// Hover 舵机
static constexpr PWM_Channel _HoverServoChannel = PWM_CHANNEL_Z;
static constexpr float _HoverServoUpperAngle = 135;
static constexpr float _HoverServoLowerAngle = 90;

// 角度信息
static constexpr float _ReloaderBaseAngle = 0;

PID _position_pid, _speed_pid;
static volatile float _target_speed, _target_position;

void ReloaderControl::Setup() {
    _speed_pid.output_limit_up = 25000;
    _speed_pid.output_limit_down = -25000;
    _speed_pid.integral_limit_up = 3000;
    _speed_pid.integral_limit_down = -3000;

    _speed_pid.Kp = 150;
    _speed_pid.Ki = 1;
    _speed_pid.Kf = 200;

    _position_pid.output_limit_up = 5.0f;
    _position_pid.output_limit_down = -5.0f;
    _position_pid.integral_limit_up = 5.0f;
    _position_pid.integral_limit_down = -5.0f;

    _position_pid.Kp = 5;
    _position_pid.Kd = 10;

    CorePWM::SetAngle(_HoverServoChannel, _HoverServoUpperAngle);
    CorePWM::Start(_HoverServoChannel);

    GoStage(RELOADER_STAGE_1);
}

void ReloaderControl::PowerOn() {
    PWR_CTR1_GPIO_Port->BSRR = PWR_CTR1_Pin;
    PWR_CTR2_GPIO_Port->BSRR = PWR_CTR2_Pin;
}

void ReloaderControl::HoverUp() {
    CorePWM::SetAngle(_HoverServoChannel, _HoverServoUpperAngle);
}

void ReloaderControl::HoverDown() {
    CorePWM::SetAngle(_HoverServoChannel, _HoverServoLowerAngle);
}

void ReloaderControl::GoStage(ReloaderStage stage) {
    _target_position = _ReloaderBaseAngle + static_cast<float>(stage) * 90.0f;
}

void ReloaderControl::CalcusInvoke() {
    auto& gm6020 = Trailer::GetReloader();

    _position_pid.Calc(_target_position - gm6020.getTotalAngle());
    _target_speed = _position_pid.out;
    _speed_pid.Calc(_target_speed - gm6020.motor_rpm, _target_speed);

    Trailer::SetReloaderVoltage(static_cast<int16_t>(_speed_pid.out));
}
