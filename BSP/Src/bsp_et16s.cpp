#include "bsp_et16s.hpp"
#include "Launcher.hpp"

static uint8_t _uart_receive_cache[50];
static uint32_t _lastTick;
static ET16S_Data _data;

static constexpr float _ChannelMax = 1695.0f;
static constexpr float _ChannelMin = 353.0f;
static constexpr float _ChannelCenter = (_ChannelMax + _ChannelMin) / 2;

void ET16S_Data::ParseData(const uint8_t pMessage[25]) {
    if (pMessage[0] != 0x0F)
        return;

    channelRaw[0]  = ((pMessage[1]    | pMessage[2]<<8)                    & 0x07FF);
    channelRaw[1]  = ((pMessage[2]>>3 | pMessage[3]<<5)                    & 0x07FF);
    channelRaw[2]  = ((pMessage[3]>>6 | pMessage[4]<<2 | pMessage[5]<<10)  & 0x07FF);
    channelRaw[3]  = ((pMessage[5]>>1 | pMessage[6]<<7)                    & 0x07FF);
    channelRaw[4]  = ((pMessage[6]>>4 | pMessage[7]<<4)                    & 0x07FF);
    channelRaw[5]  = ((pMessage[7]>>7 | pMessage[8]<<1 | pMessage[9]<<9)   & 0x07FF);
    channelRaw[6]  = ((pMessage[9]>>2 | pMessage[10]<<6)                   & 0x07FF);
    channelRaw[7]  = ((pMessage[10]>>5| pMessage[11]<<3)                   & 0x07FF);
    channelRaw[8]  = ((pMessage[12]   | pMessage[13]<<8)                   & 0x07FF);
    channelRaw[9]  = ((pMessage[13]>>3| pMessage[14]<<5)                   & 0x07FF);
    channelRaw[10] = ((pMessage[14]>>6| pMessage[15]<<2| pMessage[16]<<10) & 0x07FF);
    channelRaw[11] = ((pMessage[16]>>1| pMessage[17]<<7)                   & 0x07FF);
    channelRaw[12] = ((pMessage[17]>>4| pMessage[18]<<4)                   & 0x07FF);
    channelRaw[13] = ((pMessage[18]>>7| pMessage[19]<<1| pMessage[20]<<9)  & 0x07FF);
    channelRaw[14] = ((pMessage[20]>>2| pMessage[21]<<6)                   & 0x07FF);
    channelRaw[15] = ((pMessage[21]>>5| pMessage[22]<<3)                   & 0x07FF);

    for (uint8_t i = 0; i < 16; i++)
    {
        channel[i] = static_cast<float>(channelRaw[i] - static_cast<uint16_t>(_ChannelCenter)) / (_ChannelMax - _ChannelCenter);
    }
}

void ET16S::Setup() {
    HAL_UARTEx_ReceiveToIdle_DMA(_RC_Uart, _uart_receive_cache, sizeof(_uart_receive_cache));
    __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
}

void ET16S::UART_Receive_Callback(uint16_t len) {
    if (len == 25) {
        _data.ParseData(_uart_receive_cache);
        Launcher::RC_OnChannelChanged();
        _lastTick = HAL_GetTick();
    }
    Setup();
}

ET16S_Data ET16S::Get() {
    return _data;
}

bool ET16S::isOnline()
{
    uint32_t tick = HAL_GetTick();
    if (tick - _lastTick > 100)
        return false;
    return true;
}


