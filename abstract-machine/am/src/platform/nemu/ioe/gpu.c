#include <am.h>
#include <nemu.h>
#include <stdint.h>
// #include <stdlib.h>
#include <string.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)

void __am_gpu_init() {
  /*int i;
  int w = io_read(AM_GPU_CONFIG).width;
  int h = io_read(AM_GPU_CONFIG).height;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = i;
  outl(SYNC_ADDR, 1);*/
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = inw(VGACTL_ADDR+2), .height = inw(VGACTL_ADDR),
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  // 获取屏幕宽度和像素数据
  int screenWidth = inw(VGACTL_ADDR+2);
  uint32_t *pixelsData=ctl->pixels;
  // 初始化填充基址
  uintptr_t baseAddr=FB_ADDR+screenWidth*ctl->y*4+ctl->x*4;

  // 按行填充每一个色块
  for (int y=0;y<ctl->h;y++) {
    for (int x=0;x<ctl->w;x++) {
      outl(baseAddr+screenWidth*y*4+x*4, pixelsData[y*ctl->w+x]);
    }
  }
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
