#include "ReloaderControl.hpp"
#include "bsp_peripheral_trailer.hpp"
#include "FeedbackControl.hpp"

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

    _position_pid.output_limit_up = 100.0f;
    _position_pid.output_limit_down = -100.0f;
    _position_pid.integral_limit_up = 100.0f;
    _position_pid.integral_limit_down = -100.0f;

    _position_pid.Kp = 5;
    _position_pid.Kd = 10;
}

void ReloaderControl::SetSpeed(float rpm) {
    _target_speed = rpm;
}

void ReloaderControl::SetPositon(float angle) {
    _target_position = angle;
}

void ReloaderControl::CalcusInvoke() {
    auto& gm6020 = Trailer::GetReloader();

    _position_pid.Calc(_target_position - gm6020.getTotalAngle());
    _target_speed = _position_pid.out;
    _speed_pid.Calc(_target_speed - gm6020.motor_rpm, _target_speed);

    Trailer::SetReloaderVoltage(static_cast<int16_t>(_speed_pid.out));
}
