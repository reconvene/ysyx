#include <device/mmio.h>
#include <stdint.h>
#include <stdlib.h>
#include <macro.h>
#include <string.h>
#include <difftest/diiftest.h>

static mmioDevice deviceMap[MAX_DEVICES] = {0};
static uint32_t deviceNum = 0;

// 判断是否在指定内存范围内
static inline _Bool inField(uint32_t targetAddr, uint32_t baseAddr, uint32_t size) {
    return targetAddr - baseAddr < size;
}

// 创建对齐页
void *createPage(uint32_t size) {
    size = (size + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1);
    void *newPage = malloc(size);
    if (!newPage)
        panic("Failed to create the device's page\n");
    return newPage;
}

// 检查地址地址在哪个设备的范围
mmioDevice *checkDevice(vaddr_t addr) {
    for (uint32_t i = 0; i < deviceNum; i++) {
        if (inField(addr, deviceMap[i].base, deviceMap[i].size)) {
            return deviceMap + i;
        }
    }
    return NULL;
}

// 添加设备
void addDevice(mmioDevice newDevice) {
    uint32_t deviceLeft = newDevice.base;
    uint32_t deviceRight = newDevice.base + newDevice.size;

    // 判断声明地址是否跟其它设备重叠
    if (inField(deviceLeft,CONFIG_MBASE,MEM_SIZE) || inField(deviceRight,CONFIG_MBASE,MEM_SIZE)) {
        panic("The address [0x%08X,0x%08X] at '%s' overlaps with '%s'!\n", deviceLeft, deviceRight, newDevice.name, "pmem");
    }
    for (uint32_t i = 0; i < deviceNum; i++) {
        if (inField(deviceLeft, deviceMap[i].base, deviceMap[i].size) || inField(deviceRight, deviceMap[i].base, deviceMap[i].size)) {
            panic("The address [0x%08X,0x%08X] at '%s' overlaps with '%s'!\n", deviceLeft, deviceRight, newDevice.name, deviceMap[i].name);
        }
    }

    printf("Add device '%s' at address [0x%08X, 0x%08X]\n", newDevice.name, deviceLeft, deviceRight);
    deviceMap[deviceNum++] = newDevice;
}

// 读取设备
_Bool mmioRead(word_t *raw, vaddr_t addr, uint8_t len) {
    mmioDevice *currentDevice = checkDevice(addr);
    if (!currentDevice) return 0;
    IFDEF(CONFIG_DIFFTEST, difftest_skip_ref());
    uint32_t offset = addr - currentDevice->base;
    currentDevice->callback(offset, len, 0);
    memcpy(raw, (uint8_t *) currentDevice->devicePage + offset, len);
    return 1;
}

// 写入设备
_Bool mmioWrite(word_t data, vaddr_t addr, uint8_t len) {
    mmioDevice *currentDevice = checkDevice(addr);
    if (!currentDevice) return 0;
    IFDEF(CONFIG_DIFFTEST, difftest_skip_ref());
    uint32_t offset = addr - currentDevice->base;
    memcpy((uint8_t *) currentDevice->devicePage + offset, &data, len);
    currentDevice->callback(offset, len, 1);
    return 1;
}

void serialInit();
void timerInit();

// 初始化mmio
void mmioDeviceInit() {
    serialInit();
    timerInit();
}

// 销毁mmio
void mmioDeviceDestroy() {
    for (uint32_t i = 0; !deviceMap[i].valid; i++) {
        free(deviceMap[i].devicePage);
    }
}
