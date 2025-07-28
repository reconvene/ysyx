#include <am.h>
#include <nemu.h>
#include <stdint.h>

#define AUDIO_FREQ_ADDR      (AUDIO_ADDR + 0x00)
#define AUDIO_CHANNELS_ADDR  (AUDIO_ADDR + 0x04)
#define AUDIO_SAMPLES_ADDR   (AUDIO_ADDR + 0x08)
#define AUDIO_SBUF_SIZE_ADDR (AUDIO_ADDR + 0x0c)
#define AUDIO_INIT_ADDR      (AUDIO_ADDR + 0x10)
#define AUDIO_COUNT_ADDR     (AUDIO_ADDR + 0x14)

void __am_audio_init() {
}

void __am_audio_config(AM_AUDIO_CONFIG_T *cfg) {
  // 声明声卡存在并获取缓冲区大小
  cfg->present = true;
  cfg->bufsize = inl(AUDIO_SBUF_SIZE_ADDR);
}

void __am_audio_ctrl(AM_AUDIO_CTRL_T *ctrl) {
  // 设置频率、音轨数、采样数
  outl(AUDIO_FREQ_ADDR,ctrl->freq);
  outl(AUDIO_CHANNELS_ADDR,ctrl->channels);
  outl(AUDIO_SAMPLES_ADDR,ctrl->samples);
  outl(AUDIO_INIT_ADDR, 1);
}

void __am_audio_status(AM_AUDIO_STATUS_T *stat) {
  stat->count = inl(AUDIO_COUNT_ADDR);
}

void __am_audio_play(AM_AUDIO_PLAY_T *ctl) {
  // 获取缓冲区大小和音频大小
  uint32_t sbufSize=inl(AUDIO_SBUF_SIZE_ADDR);
  uint32_t audioLen=(uintptr_t)ctl->buf.end-(uintptr_t)ctl->buf.start;
  // 获取音频数据
  uint8_t *audioBuf=(uint8_t *)ctl->buf.start;

  // 等待，直至剩余区域足够文件写入缓冲区
  while (audioLen > sbufSize-inl(AUDIO_COUNT_ADDR));

  // 获取写入缓冲区的基址
  uintptr_t baseAddr=AUDIO_SBUF_ADDR+inl(AUDIO_COUNT_ADDR);
  uint32_t multipleFour=audioLen/4;
  uint32_t remainingFour=audioLen%4;
  // 批量写入4字节数据
  for (uint32_t i=0;i<multipleFour;i++) outl(baseAddr+i*4,((uint32_t *)audioBuf)[i]);
  // 写入剩余部分
  baseAddr+=4*multipleFour;
  for (uint32_t i=0;i<remainingFour;i++) outb(baseAddr+i,audioBuf[i+4*multipleFour]);

  // 更新缓冲区写入数
  outl(AUDIO_COUNT_ADDR,inl(AUDIO_COUNT_ADDR)+audioLen);
}
