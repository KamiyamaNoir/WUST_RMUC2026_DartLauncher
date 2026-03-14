#include "bsp_peripheral_stepper.hpp"
#include "usart.h"
#include <cmath>

static constexpr UART_HandleTypeDef * _Uart = &huart6;

void ZDT_X42::Enable(bool enable)
{
    uint8_t en = enable ? 0x01 : 0x00;
    const uint8_t cmd[] = {
        0x01,
        0xF3,
        0xAB,
        en,
        0x00,
        0x6B
    };
    HAL_UART_Transmit(_Uart, cmd, sizeof(cmd), 100);
}

void ZDT_X42::SetSpeed(float rpm)
{
    uint8_t cw = rpm >= 0 ? 0x00 : 0x01;
    auto rpm_byte = static_cast<uint16_t>(fabsf(rpm));
    const uint8_t cmd[] = {
        0x01,
        0xF6,
        cw,
        0x03,
        0xE8,
        static_cast<uint8_t>(rpm_byte >> 8),
        static_cast<uint8_t>(rpm_byte),
        0x00,
        0x6B
    };
    HAL_UART_Transmit(_Uart, cmd, sizeof(cmd), 100);
}

void ZDT_X42::SetCurrent(float current)
{
    uint8_t cw = current >= 0 ? 0x00 : 0x01;
    auto current_byte = static_cast<uint16_t>(fabsf(current));
    const uint8_t cmd[] = {
        0x01,
        0xF5,
        cw,
        0x03,
        0xE8,
        static_cast<uint8_t>(current_byte >> 8),
        static_cast<uint8_t>(current_byte),
        0x00,
        0x6B
    };
    HAL_UART_Transmit(_Uart, cmd, sizeof(cmd), 100);
}

void ZDT_X42::ZeroPointSet(bool save)
{
    const uint8_t cmd[] = {
        0x01,
        0x93,
        0x88,
        static_cast<uint8_t>(save ? 0x00 : 0x01),
        0x6B
    };
    HAL_UART_Transmit(_Uart, cmd, sizeof(cmd), 100);
}

