#ifndef REMOTE_CONTROLLER_H
#define REMOTE_CONTROLLER_H

#include "main.h"
#include "usart.h"

struct ET16S_Data
{
    uint16_t channelRaw[16];
    float channel[16];

    void ParseData(const uint8_t pMessage[25]);
};

class ET16S
{
public:
    static constexpr UART_HandleTypeDef* _RC_Uart = &huart1;

    static bool isOnline();

    static void Setup();

    static void UART_Receive_Callback(uint16_t len);

    static ET16S_Data Get();
};

#endif