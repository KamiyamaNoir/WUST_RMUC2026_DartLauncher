#include "LaunchControl.hpp"
#include "TrailerControl.hpp"
#include "ReloaderControl.hpp"
#include "bsp_peripheral_trailer.hpp"
#include <cmath>

// 扳机相对拖车的偏移
static constexpr float _CarTriggerOffset = 580.0f;

// 装载位置
static constexpr float _CarLoadPosition = 450.0f;

enum LaunchStage
{
    LAUNCH_STAGE_1,
    LAUNCH_STAGE_2,
    LAUNCH_STAGE_3,
    LAUNCH_STAGE_4,
    LAUNCH_STAGE_STOP,
};

static LaunchStage _stage = LAUNCH_STAGE_1;

void LaunchControl::Reset() {
    _stage = LAUNCH_STAGE_1;
    TrailerControl::ZeroPosCaliberate();
}

void LaunchControl::Next() {
    if (_stage > LAUNCH_STAGE_4) {
        return;
    }
    // 从第二发开始，需要换弹
    if (_stage > LAUNCH_STAGE_1) {
        // 下调转盘
        ReloaderControl::HoverDown();
        HAL_Delay(500);
        // 转盘换弹
        ReloaderControl::GoStage(static_cast<ReloaderStage>(_stage));
        HAL_Delay(1000);
        // 撤回转盘
        ReloaderControl::HoverUp();
    }
    // 到达装载位
    TrailerControl::CarGoToPosition(_CarLoadPosition);
    while (fabsf(TrailerControl::GetCarPosition() - _CarLoadPosition) > 10) {
        HAL_Delay(10);
    }
    // 下调转盘
    ReloaderControl::HoverDown();
    HAL_Delay(800);

    // 快速校准
    FastCarCaliberate();

    // 撤回转盘
    ReloaderControl::HoverUp();

    _stage = static_cast<LaunchStage>(_stage + 1);

    /** 以下为自动发射逻辑 可跳过 **/
    // 到达发射位
    TrailerControl::CarGoToPosition(TrailerControl::GetTriggerPositon() + _CarTriggerOffset);
    while (fabsf(TrailerControl::GetCarPosition() - (TrailerControl::GetTriggerPositon() + _CarTriggerOffset)) > 10) {
        HAL_Delay(10);
    }
    // 回撤拖车
    FastCarCaliberate();
}

void LaunchControl::FastCarCaliberate() {
    constexpr uint16_t sample_exp = 200;

    uint16_t sample_count = 0;
    float average_rpm = 0.0f;
    TrailerControl::SetCarSpeed(-100.0f);
    for (;;)
    {
        average_rpm += Trailer::GetLeft().motor_rpm / sample_exp;
        sample_count++;
        if (sample_count == sample_exp)
        {
            if (fabsf(average_rpm) < 15)
            {
                TrailerControl::SetCarSpeed(0.0f);
                TrailerControl::ResetCarPosition();
                break;
            }
            average_rpm = 0.0f;
            sample_count = 0;
        }
        HAL_Delay(1);
    }
}
