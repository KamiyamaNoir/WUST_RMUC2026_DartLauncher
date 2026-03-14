#include "bsp_core_timer.hpp"

#include "Launcher.hpp"
#include "main.h"
#include "tim.h"

void CoreTimer::SetupTimebase() {
    HAL_TIM_Base_Start_IT(&htim7);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim7) {
        // 1ms timebase
        Launcher::Timebase_1ms();
    }
}