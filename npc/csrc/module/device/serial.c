#include <device/mmio.h>

uint8_t *serialBuf = NULL;

// 输出字符至stderr
void serialHandler(uint32_t offset, uint8_t len, _Bool writeIF) {
    assert(writeIF || offset==0 || len==1);
    putc(*serialBuf, stderr);
}

// 初始化串口设备
void serialInit() {
    serialBuf = (uint8_t *) createPage(1);
    mmioDevice serialDevice = {
        .name = "serial",
        .base = CONFIG_SERIAL_MMIO,
        .size = 1,
        .devicePage = serialBuf,
        .valid = 1,
        .callback = serialHandler
    };
    addDevice(serialDevice);
}
