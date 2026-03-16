#include "TrailerControl.hpp"
#include "FeedbackControl.hpp"
#include "bsp_peripheral_trailer.hpp"
#include "Asynchronous.hpp"
#include "bsp_beep.hpp"
#include "bsp_drive.hpp"
extern "C"{
#include "arm_math.h"
}

static constexpr PWM_Channel _Trigger_Channel = PWM_CHANNEL_A;
static constexpr float _Trigger_OnAngle = 0.0f;
static constexpr float _Trigger_OffAngle = 60.0f;

enum PID_TrackMode
{
    PID_TRACK_SPEED,
    PID_TRACK_POSITION,
};

static PID _left_speed_pid, _right_speed_pid, _trigger_speed_pid;
static PID _car_position_pid, _trigger_position_pid;

static volatile PID_TrackMode _car_track_mode, _trigger_track_mode;

static volatile float _car_target_rpm = 0.0f, _car_target_pos = 0.0f, _car_position_offset = 0.0f;
static volatile float _trigger_target_rpm = 0.0f, _trigger_target_pos = 0.0f, _trigger_position_offset = 0.0f;

void TrailerControl::ZeroPosCaliberate() {
    Asynchronous beep_task([] {
        Beep::Set(1000);
        Beep::Volume(0.8);
        for (;;) {
            Beep::Start();
            HAL_Delay(100);
            Beep::Stop();
            HAL_Delay(900);
        }
    }, "beep", 1);
    // Caliberate Car
    SetCarSpeed(-20.0f);

    uint16_t sample_count = 0;
    float average_rpm = 0.0f;
    for (;;)
    {
        average_rpm += Trailer::GetLeft().motor_rpm / 500.0f;
        sample_count++;

        if (sample_count == 500)
        {
            if (fabsf(average_rpm) < 1)
            {
                SetCarSpeed(0.0f);
                ResetCarPosition();
                break;
            }
            average_rpm = 0.0f;
            sample_count = 0;
        }

        HAL_Delay(1);
    }

    TriggerPowerLimit(1.0f, -1.0f);
    SetTriggerSpeed(-200.0f);
    sample_count = 0;
    average_rpm = 0.0f;
    for (;;)
    {
        average_rpm += Trailer::GetCenter().motor_rpm / 500.0f;
        sample_count++;

        if (sample_count == 500)
        {
            if (fabsf(average_rpm) < 1)
            {
                SetTriggerSpeed(0);
                ResetTriggerPosition();
                break;
            }
            average_rpm = 0.0f;
            sample_count = 0;
        }

        HAL_Delay(1);
    }
    TriggerPowerLimit(10.0f, -10.0f);

    beep_task.cancel();
    Beep::Start();
    HAL_Delay(1000);
    Beep::Stop();
}

void TrailerControl::Setup() {
    _left_speed_pid.output_limit_up = 20.0f;
    _left_speed_pid.output_limit_down = -3.0f;
    _right_speed_pid.output_limit_up = 3.0f;
    _right_speed_pid.output_limit_down = -20.0f;

    _left_speed_pid.integral_limit_up = 20.0f;
    _left_speed_pid.integral_limit_down = -3.0f;
    _right_speed_pid.integral_limit_up = 3.0f;
    _right_speed_pid.integral_limit_down = -20.0f;

    _right_speed_pid.Kp = _left_speed_pid.Kp = 0.43;
    _right_speed_pid.Ki = _left_speed_pid.Ki = 0.03;

    _car_position_pid.output_limit_up = 100.0f;
    _car_position_pid.output_limit_down = -100.0f;
    _car_position_pid.Kp = 10.0f;

    _trigger_speed_pid.output_limit_up = 10.0f;
    _trigger_speed_pid.output_limit_down = -10.0f;
    _trigger_speed_pid.Kp = 0.1f;

    _trigger_position_pid.output_limit_up = 400.0f;
    _trigger_position_pid.output_limit_down = -400.0f;
    _trigger_position_pid.Kp = 100.0f;

    CorePWM::SetAngle(_Trigger_Channel, _Trigger_OffAngle);
    CorePWM::Start(_Trigger_Channel);
}

void TrailerControl::TriggerOn() {
    CorePWM::SetAngle(_Trigger_Channel, _Trigger_OnAngle);
}

void TrailerControl::TriggerOff() {
    CorePWM::SetAngle(_Trigger_Channel, _Trigger_OffAngle);
}

void TrailerControl::CarPowerLimit(float load, float release) {
    _left_speed_pid.output_limit_up = load;
    _left_speed_pid.output_limit_down = -release;
    _right_speed_pid.output_limit_up = release;
    _right_speed_pid.output_limit_down = -load;
}

void TrailerControl::SetCarSpeed(float rpm) {
    _car_target_rpm = rpm;
    _car_track_mode = PID_TRACK_SPEED;
}

void TrailerControl::CarGoToPosition(float pos) {
    _car_target_pos = pos;
    _car_track_mode = PID_TRACK_POSITION;
}

float TrailerControl::GetCarPosition() {
    return (Trailer::GetLeft().getTotalAngle() * (187.0f / 3591.0f) * PI / 180.0f * 16.81f) - _car_position_offset;
}

void TrailerControl::ResetCarPosition() {
    _car_position_offset = (Trailer::GetLeft().getTotalAngle() * (187.0f / 3591.0f) * PI / 180.0f * 16.81f);
}

void TrailerControl::TriggerPowerLimit(float up, float down) {
    _trigger_speed_pid.output_limit_up = up;
    _trigger_speed_pid.output_limit_down = down;
}

void TrailerControl::SetTriggerSpeed(float rpm) {
    _trigger_target_rpm = rpm;
    _trigger_track_mode = PID_TRACK_SPEED;
}

void TrailerControl::TriggerGoToPosition(float pos) {
    _trigger_target_pos = pos;
    _trigger_track_mode = PID_TRACK_POSITION;
}

float TrailerControl::GetTriggerPositon() {
    return Trailer::GetCenter().getTotalAngle() / 12960.0f * 2.0f - _trigger_position_offset;
}

void TrailerControl::ResetTriggerPosition() {
    _trigger_position_offset = (Trailer::GetCenter().getTotalAngle() / 12960.0f * 2.0f);
}

void TrailerControl::CalcusInvoke() {
    if (_car_track_mode == PID_TRACK_POSITION)
    {
        _car_position_pid.Calc(_car_target_pos - GetCarPosition());
        _car_target_rpm = _car_position_pid.out;
    }

    if (_trigger_track_mode == PID_TRACK_POSITION)
    {
        float err = _trigger_target_pos - GetTriggerPositon();
        if (fabsf(err) < 0.1f)
        {
            _trigger_track_mode = PID_TRACK_SPEED;
            _trigger_target_rpm = 0.0f;
        }
        else
        {
            _trigger_position_pid.Calc(err);
            _trigger_target_rpm = _trigger_position_pid.out;
        }
    }

    _left_speed_pid.Calc(_car_target_rpm - Trailer::GetLeft().motor_rpm / 20.0f);
    _right_speed_pid.Calc(-_car_target_rpm - Trailer::GetRight().motor_rpm / 20.0f);
    _trigger_speed_pid.Calc(_trigger_target_rpm - Trailer::GetCenter().motor_rpm / 36.0f);

    Trailer::SetCurrent(_left_speed_pid.out, _right_speed_pid.out, _trigger_speed_pid.out);
}



