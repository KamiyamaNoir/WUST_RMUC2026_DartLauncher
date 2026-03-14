#ifndef __FEEDBACK_CONTROL_H_
#define __FEEDBACK_CONTROL_H_

class PID
{
public:
    void Calc(float err)
    {
        float Po = Kp * err;
        _integral += err;
        float Do = Kd * (err - err_last);

        err_last = err;

        if (Ki * _integral > integral_limit_up)
            _integral = integral_limit_up;
        if (Ki * _integral < integral_limit_down)
            _integral = integral_limit_down;

        float pid_out = Po + Ki * _integral + Do;

        if (pid_out > output_limit_up)
            pid_out = output_limit_up;
        if (pid_out < output_limit_down)
            pid_out = output_limit_down;
        out = pid_out;
    }

    float out = 0.0f;

    float Kp = 0.0f, Ki = 0.0f, Kd = 0.0f;
    float output_limit_up = 0.0f, output_limit_down = 0.0f;
    float integral_limit_up = 0.0f, integral_limit_down = 0.0f;
private:
    float _integral = 0.0f;
    float err_last = 0.0f;
};


#endif
