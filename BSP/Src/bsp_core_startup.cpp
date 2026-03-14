#include "bsp_core_can.hpp"
#include "bsp_core_delta_t.hpp"
#include "bsp_drive.hpp"
#include "cmsis_os.h"
#include "usb_device.h"
#include "Launcher.hpp"
#include "bsp_et16s.hpp"

extern "C"{
    void StartDefaultTask(void const * argument);
}

void HAL_Delay(uint32_t Delay)
{
    osDelay(Delay);
}

extern uint8_t _user_heap_region;

void StartDefaultTask(void const * argument)
{
    // Initialize Heap
    const HeapRegion_t xHeapRegions[] =
    {
        {&_user_heap_region, 0x10000},
        { nullptr, 0 }
    };
    vPortDefineHeapRegions(xHeapRegions);
    // Initialize USB
    MX_USB_DEVICE_Init();
    // Delta T
    DeltaT::Setup();
    // Initialize CAN
    CoreCAN::Setup();
    // ET16S
    ET16S::Setup();

    Launcher::Run();
}
