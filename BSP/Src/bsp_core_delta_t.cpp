#include "bsp_core_delta_t.hpp"
#include "tim.h"

static constexpr TIM_HandleTypeDef* _Timer = &htim2;

DeltaT::DeltaT() {
    lastTick = _Timer->Instance->CNT;
}

void DeltaT::Setup() {
    HAL_TIM_Base_Start(_Timer);
}

uint32_t DeltaT::us() {
    uint32_t tick = _Timer->Instance->CNT;
    // 能计时一个小时，不考虑溢出的情况为好
    // if (tick < lastTick)
    //     dt = 0xFFFFFFFF - lastTick + tick;
    // else
    //     dt = tick - lastTick;
    uint32_t dt = tick - lastTick;
    lastTick = tick;
    return dt;
}
