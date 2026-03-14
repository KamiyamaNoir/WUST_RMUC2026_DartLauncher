#ifndef BSP_DRIVE_H
#define BSP_DRIVE_H

enum PWM_Channel
{
    // Timer1
    PWM_CHANNEL_H = 0, // CH1
    PWM_CHANNEL_G,
    PWM_CHANNEL_F,
    PWM_CHANNEL_E, // CH4
    // Timer2
    PWM_CHANNEL_D, // CH1
    PWM_CHANNEL_C,
    PWM_CHANNEL_B,
    PWM_CHANNEL_A, // CH4
    // Timer3
    PWM_CHANNEL_W, // CH1
    PWM_CHANNEL_X,
    PWM_CHANNEL_Y,
    PWM_CHANNEL_Z, // CH4
};

class CorePWM
{
public:
    static void Start(PWM_Channel channel);

    static void Stop(PWM_Channel channel);

    static void SetTimeMS(PWM_Channel channel, float timeMS);

    static void SetAngle(PWM_Channel channel, float angle);
};

#endif
