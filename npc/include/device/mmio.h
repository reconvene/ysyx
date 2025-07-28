#ifndef MMIO_H
#define MMIO_H
#include <stdint.h>
#include <macro.h>

#define MAX_DEVICES 16
#define PAGE_SIZE 4096

typedef void(*deviceCallback)(uint32_t offset, uint8_t len, _Bool writeIF);
typedef struct {
    const char *name;
    vaddr_t base;
    uint32_t size;
    void *devicePage;
    _Bool valid;
    deviceCallback callback;
} mmioDevice;

void *createPage(uint32_t size);
void mmioDeviceInit();
void mmioDeviceDestroy();

void addDevice(mmioDevice newDevice);
_Bool mmioRead(word_t *raw, vaddr_t addr, uint8_t len);
_Bool mmioWrite(word_t data, vaddr_t addr, uint8_t len);

#endif //MMIO_H
