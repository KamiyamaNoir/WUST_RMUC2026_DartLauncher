#include "bsp_drive.hpp"
#include "tim.h"

// Set Manually
static constexpr TIM_HandleTypeDef* _Timer1 = &htim4;
static constexpr TIM_HandleTypeDef* _Timer2 = &htim5;
static constexpr TIM_HandleTypeDef* _Timer3 = &htim8;
static constexpr float _TimerClock = 84000000.0f;
static constexpr float _TimerPrescaler = 840.0f;

// Calculated automatically
static constexpr float _TimerFreq = _TimerClock / _TimerPrescaler;
static constexpr float _TimerBase = 1.0f / _TimerFreq;
static constexpr float _PWM_Base = 0.0001f / _TimerBase; // 0.1ms
static constexpr float _PWM_Start_Time = 0.5f;
static constexpr float _PWM_Coef_Time = 2.0f;

static TIM_HandleTypeDef * GetHandler(PWM_Channel channel) {
    if (PWM_CHANNEL_H <= channel && channel <= PWM_CHANNEL_E) {
        return _Timer1;
    }
    if (PWM_CHANNEL_D <= channel && channel <= PWM_CHANNEL_A) {
        return _Timer2;
    }
    return _Timer3;
}

static uint32_t GetChannel(PWM_Channel channel) {
    if (channel == PWM_CHANNEL_H ||
        channel == PWM_CHANNEL_D ||
        channel == PWM_CHANNEL_W) {
        return TIM_CHANNEL_1;
    }
    if (channel == PWM_CHANNEL_G ||
        channel == PWM_CHANNEL_C ||
        channel == PWM_CHANNEL_X) {
        return TIM_CHANNEL_2;
    }
    if (channel == PWM_CHANNEL_F ||
        channel == PWM_CHANNEL_B ||
        channel == PWM_CHANNEL_Y) {
        return TIM_CHANNEL_3;
    }
    return TIM_CHANNEL_4;
}

void CorePWM::Start(PWM_Channel channel) {
    HAL_TIM_PWM_Start(GetHandler(channel), GetChannel(channel));
}

void CorePWM::Stop(PWM_Channel channel) {
    HAL_TIM_PWM_Stop(GetHandler(channel), GetChannel(channel));
}

void CorePWM::SetTimeMS(PWM_Channel channel, float ms) {
    auto ccr = static_cast<uint16_t>(ms * _PWM_Base * 10);

    __HAL_TIM_SET_COMPARE(GetHandler(channel), GetChannel(channel), ccr);
}

void CorePWM::SetAngle(PWM_Channel channel, float degree) {
    if (degree < 0.0f) degree = 0.0f;
    else if (degree > 180.0f) degree = 180.0f;
    auto time = _PWM_Start_Time + degree / 180.0f * _PWM_Coef_Time;
    SetTimeMS(channel, time);
}

