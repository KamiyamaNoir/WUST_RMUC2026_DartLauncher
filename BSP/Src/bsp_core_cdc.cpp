#include "bsp_core_cdc.hpp"
#include "usbd_cdc_if.h"
#include <cstring>

extern "C"{
    void CoreCDCReceiveCallback(uint8_t* Buf, uint32_t Len);
}

__PACKED_STRUCT DataFrame
{
    int32_t msg;
    float x;
    float y;
    float dis;
    uint32_t sum;
};

void CoreCDCReceiveCallback(uint8_t* Buf, uint32_t Len) {
    if (Len != sizeof(DataFrame))
        return;

    auto* pData = reinterpret_cast<DataFrame*>(Buf);
    // 校验和
    {
        uint32_t sum = 0;
        for (int i = 0; i < sizeof(DataFrame) - 4; ++i) {
            sum += Buf[i];
        }
        if (sum != pData->sum) {
            return;
        }
    }
    // 校验成功

}

void CDC::Transmit(const uint8_t* data, uint32_t len) {
    CDC_Transmit_FS(const_cast<uint8_t*>(data), len);
}
