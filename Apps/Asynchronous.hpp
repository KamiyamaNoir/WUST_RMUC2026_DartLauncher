#ifndef _ASYNCHRONOUS_H
#define _ASYNCHRONOUS_H

#include <functional>
#include "FreeRTOS.h"
#include "task.h"

class Asynchronous
{
public:
    Asynchronous(std::function<void()> callable, const char* name, uint32_t priority, uint16_t stack_size = 256) :
        _func(std::move(callable)),
        _on_running(false),
        _handler(nullptr)
    {
        BaseType_t ct = xTaskCreate(call_member, name, stack_size, this, priority, &_handler);
        if (ct != pdPASS)
        {
            _handler = nullptr;
        }
    }

    // 禁用拷贝，防止 TaskHandle 被浅拷贝导致双重删除
    Asynchronous(const Asynchronous&) = delete;
    Asynchronous& operator=(const Asynchronous&) = delete;

    // 不支持移动语义
    Asynchronous(Asynchronous&& other) = delete;

    ~Asynchronous()
    {
        if (_handler != nullptr)
        {
            eTaskState state = eTaskGetState(_handler);
            if (state != eDeleted)
            {
                vTaskDelete(_handler);
            }
            _handler = nullptr;
        }
    }

    bool await(uint16_t timeout) const
    {
        if (_handler == nullptr) return false;

        while (timeout > 0 && _on_running)
        {
            vTaskDelay(1);
            --timeout;
        }
        return _on_running;
    }

    void cancel()
    {
        if (_handler != nullptr && _on_running)
        {
            eTaskState state = eTaskGetState(_handler);
            if (state == eReady || state == eBlocked || state == eRunning)
            {
                vTaskSuspend(_handler);
            }
        }
    }

private:
    static void call_member(void* argument)
    {
        auto* self = static_cast<Asynchronous*>(argument);
        if (self)
        {
            self->call();
        }
        // 任务函数结束，FreeRTOS 会自动清理部分资源，但 TCB 需由创建者删除或配置自动删除
    }

    void call()
    {
        _on_running = true;

        _func();

        _on_running = false;
    }

    std::function<void()> _func;
    volatile bool _on_running;
    TaskHandle_t _handler;
};

#endif