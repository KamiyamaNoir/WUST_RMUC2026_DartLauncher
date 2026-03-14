#include "bsp_beep.hpp"
#include "tim.h"
#include "Asynchronous.hpp"

// Set Maunally
static constexpr TIM_HandleTypeDef* _Timer = &htim12;
static constexpr uint32_t _Channel = TIM_CHANNEL_1;
static constexpr float _TimerClock = 84000000.0f;
static constexpr float _TimerPrescaler = 84;

// Calculated automatically
static constexpr float _TimerFreq = _TimerClock / _TimerPrescaler;

static float _vol = 0.5f;

void Beep::Start() {
    HAL_TIM_PWM_Start(_Timer, _Channel);
}

void Beep::Stop() {
    HAL_TIM_PWM_Stop(_Timer, _Channel);
}

void Beep::Set(float freq) {
    __HAL_TIM_SET_AUTORELOAD(_Timer, static_cast<uint32_t>(_TimerFreq / freq) - 1);
    Volume(_vol);
}

void Beep::Volume(float vol) {
    __HAL_TIM_SET_COMPARE(_Timer, _Channel, static_cast<uint32_t>(static_cast<float>(_Timer->Instance->ARR) * vol));
    _vol = vol;
}
