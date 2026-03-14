#include "main.h"
#include "bsp_et16s.hpp"

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart == ET16S::_RC_Uart)
    {
        ET16S::UART_Receive_Callback(Size);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart == ET16S::_RC_Uart)
    {
        ET16S::Setup();
    }
}
