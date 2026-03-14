#include "bsp_core_can.hpp"
#include "bsp_peripheral_trailer.hpp"
#include "can.h"

void CoreCAN::Setup() {
    constexpr CAN_FilterTypeDef _filer = {
        // 0x201, 0x202为C620
        .FilterIdHigh = 0x201 << 5,
        .FilterIdLow = 0x202 << 5,
        // 0x203为C610
        .FilterMaskIdHigh = 0x203 << 5,
        .FilterMaskIdLow = 0x0000,

        .FilterFIFOAssignment = CAN_FilterFIFO0,
        .FilterBank = 0,
        .FilterMode = CAN_FILTERMODE_IDLIST,
        .FilterScale = CAN_FILTERSCALE_16BIT,
        .FilterActivation = CAN_FILTER_ENABLE,
    };

    HAL_CAN_ConfigFilter(&hcan1, &_filer);
    HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    uint8_t buffer[8];
    CAN_RxHeaderTypeDef header;
    HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &header, buffer);
    if (header.DLC == 8) {
        Trailer::ParseData(header.StdId, buffer);
    }
}

