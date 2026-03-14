#include "bsp_peripheral_trailer.hpp"
#include "can.h"

static constexpr CAN_HandleTypeDef* _Can = &hcan1;

static C620_Data _left, _right;
static C610_Data _center;
static GM6020_Data _reloader;

void Trailer::ParseData(uint16_t can_id, uint8_t pak[8]) {
    switch (can_id) {
    case CAN_ID_L:
        _left.ParseData(pak);break;
    case CAN_ID_R:
        _right.ParseData(pak);break;
    case CAN_ID_C:
        _center.ParseData(pak);break;
    case CAN_ID_GM6020:
        _reloader.ParseData(pak);break;
    default:
        break;
    }
}

void Trailer::SetCurrent(float ampere_left, float ampere_right, float ampere_center) {
    const CAN_TxHeaderTypeDef _header = {
        .StdId = 0x200,
        .ExtId = 0,
        .IDE = CAN_ID_STD,
        .RTR = CAN_RTR_DATA,
        .DLC = 8,
        .TransmitGlobalTime = DISABLE,
    };

    auto am_left = static_cast<int16_t>(ampere_left * (16384.0f / 20.0f));
    auto am_right = static_cast<int16_t>(ampere_right * (16384.0f / 20.0f));
    auto am_center = static_cast<int16_t>(ampere_center * (10000.0f / 10.0f));

    uint8_t buffer[8] = {
        static_cast<uint8_t>(am_left >> 8),
        static_cast<uint8_t>(am_left & 0xFF),
        static_cast<uint8_t>(am_right >> 8),
        static_cast<uint8_t>(am_right & 0xFF),
        static_cast<uint8_t>(am_center >> 8),
        static_cast<uint8_t>(am_center & 0xFF),
        0x00,
        0x00,
    };

    uint32_t mail;
    HAL_CAN_AddTxMessage(_Can, &_header, buffer, &mail);
}

void Trailer::SetReloaderCurrent(float ampere) {
    const CAN_TxHeaderTypeDef _header = {
        .StdId = 0x1FE,
        .ExtId = 0,
        .IDE = CAN_ID_STD,
        .RTR = CAN_RTR_DATA,
        .DLC = 8,
        .TransmitGlobalTime = DISABLE,
    };

    auto am = static_cast<int16_t>(ampere * (16384.0f / 3.0f));

    uint8_t buffer[8] = {
        static_cast<uint8_t>(am >> 8),
        static_cast<uint8_t>(am & 0xFF),
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
        0x00,
    };

    uint32_t mail;
    HAL_CAN_AddTxMessage(_Can, &_header, buffer, &mail);
}

void C620_Data::ParseData(const uint8_t pak[8]) {
    float angle = static_cast<float>(static_cast<uint16_t>(pak[0] << 8 | pak[1])) * (360.0f / 8192.0f);
    motor_rpm = static_cast<float>(static_cast<int16_t>(pak[2] << 8 | pak[3]));
    motor_current = static_cast<float>(static_cast<int16_t>(pak[4] << 8 | pak[5])) * (20.0f / 16384.0f);
    motor_temperture = pak[6];

    float delta_angle = angle - motor_angle;
    if (delta_angle > 180.0f)
        yaw_count--;
    else if (delta_angle < -180.0f)
        yaw_count++;
    motor_angle = angle;
}

void C610_Data::ParseData(const uint8_t pak[8]) {
    float angle = static_cast<float>(static_cast<uint16_t>(pak[0] << 8 | pak[1])) * (360.0f / 8192.0f);
    motor_rpm = static_cast<float>(static_cast<int16_t>(pak[2] << 8 | pak[3]));
    motor_current = static_cast<float>(static_cast<int16_t>(pak[4] << 8 | pak[5])) * (10.0f / 10000.0f);

    float delta_angle = angle - motor_angle;
    if (delta_angle > 180.0f)
        yaw_count--;
    else if (delta_angle < -180.0f)
        yaw_count++;
    motor_angle = angle;
}

void GM6020_Data::ParseData(const uint8_t pak[8]) {
    float angle = static_cast<float>(static_cast<uint16_t>(pak[0] << 8 | pak[1])) * (360.0f / 8192.0f);
    motor_rpm = static_cast<float>(static_cast<int16_t>(pak[2] << 8 | pak[3]));
    motor_current = static_cast<float>(static_cast<int16_t>(pak[4] << 8 | pak[5])) * (3.0f / 16384.0f);

    float delta_angle = angle - motor_angle;
    if (delta_angle > 180.0f)
        yaw_count--;
    else if (delta_angle < -180.0f)
        yaw_count++;
    motor_angle = angle;
}

C620_Data& Trailer::GetLeft() {
    return _left;
}

C620_Data& Trailer::GetRight() {
    return _right;
}

C610_Data& Trailer::GetCenter() {
    return _center;
}

GM6020_Data& Trailer::GetReloader() {
    return _reloader;
}

