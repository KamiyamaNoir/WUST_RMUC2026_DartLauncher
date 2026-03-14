#include "FreeRTOS.h"
#include "portable.h"

void* operator new(size_t size) {
    return pvPortMalloc(size);
}

void operator delete(void* p) noexcept {
    vPortFree(p);
}
