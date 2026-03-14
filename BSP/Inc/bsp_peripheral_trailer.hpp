#ifndef LAUNCHERA_BSP_PERIPHERAL_TRAILER_HPP
#define LAUNCHERA_BSP_PERIPHERAL_TRAILER_HPP

#include "main.h"

struct C620_Data
{
    float getTotalAngle() const
    {
        return 360.0f * static_cast<float>(yaw_count) + motor_angle;
    }

    void ParseData(const uint8_t pak[8]);

    int32_t yaw_count = 0;

    // unit: degree ° (Relative)
    float motor_angle = 0.0f;
    // unit: Rotate per Minitus (Signed)
    float motor_rpm = 0;
    // unit: Ampere (-20A ~ 20A)
    float motor_current = 0.0f;
    // unit: Celsius °C
    uint8_t motor_temperture = 0;
};

struct C610_Data
{
    float getTotalAngle() const
    {
        return 360.0f * static_cast<float>(yaw_count) + motor_angle;
    }

    void ParseData(const uint8_t pak[8]);

    int32_t yaw_count = 0;

    // unit: degree ° (Relative)
    float motor_angle = 0.0f;
    // unit: Rotate per Minitus (Signed)
    float motor_rpm = 0;
    // unit: Ampere (-10A ~ 10A)
    float motor_current = 0.0f;
};

class Trailer
{
public:
    static constexpr uint16_t CAN_ID_L = 0x201;
    static constexpr uint16_t CAN_ID_R = 0x202;
    static constexpr uint16_t CAN_ID_C = 0x203;

    static void ParseData(uint16_t can_id, uint8_t pak[8]);

    static void SetCurrent(float ampere_left, float ampere_right, float ampere_center);

    static C620_Data GetLeft();
    static C620_Data GetRight();
    static C610_Data GetCenter();
};

#endif //LAUNCHERA_BSP_PERIPHERAL_TRAILER_HPP