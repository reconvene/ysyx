#include <device/mmio.h>
#include <time.h>


uint32_t *timerBuf = NULL;
static uint64_t startTime = 0;

// 获取时间
void timerHandler(uint32_t offset, uint8_t len, _Bool writeIF) {
    assert((offset == 0 || offset == 4) && len==4);
    if (!writeIF && offset == 0) {
        // 获取当前时间
        struct timespec time;
        clock_gettime(CLOCK_MONOTONIC, &time);

        uint64_t currentTime = time.tv_sec * 1000000 + time.tv_nsec / 1000;
        if (!startTime) startTime = currentTime;

        uint64_t us = currentTime - startTime;
        timerBuf[0] = (uint32_t) us;
        timerBuf[1] = us >> 32;
    }
}

// 初始化时钟设备
void timerInit() {
    timerBuf = (uint32_t *) createPage(8);
    mmioDevice timerDevice = {
        .name = "timer",
        .base = CONFIG_RTC_MMIO,
        .size = 8,
        .devicePage = timerBuf,
        .valid = 1,
        .callback = timerHandler
    };
    addDevice(timerDevice);
}
