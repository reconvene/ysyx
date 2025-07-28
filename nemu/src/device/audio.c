/***************************************************************************************
* Copyright (c) 2014-2024 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <common.h>
#include <device/map.h>
#include <SDL2/SDL.h>

enum {
  reg_freq,
  reg_channels,
  reg_samples,
  reg_sbuf_size,
  reg_init,
  reg_count,
  nr_reg
};

static uint8_t *sbuf = NULL;
static uint32_t *audio_base = NULL;

// 播放音频
static void audioPlayer(void *userdata, Uint8 * stream, int len) {
  assert(audio_base[reg_init]);
  IFDEF(CONFIG_AUDIO_LOCK, SDL_LockAudio());
  // 如果缓冲区里没数据了则静音
  if (audio_base[reg_count] <= 0) {
    SDL_memset(stream, 0, len);
    IFDEF(CONFIG_AUDIO_LOCK, SDL_UnlockAudio());
    return;
  }

  // 读取缓冲区数据
  uint32_t copyLen = len>audio_base[reg_count]?audio_base[reg_count]:len;
  SDL_memcpy(stream, sbuf, copyLen);
  // 如果长度不够，则剩下部分静音
  if (copyLen < len) SDL_memset(stream + copyLen, 0, len - copyLen);
  // 剩余部分数据以供下次读取
    memmove(sbuf, sbuf + copyLen, audio_base[reg_count] - copyLen);
  audio_base[reg_count]-=copyLen;
  IFDEF(CONFIG_AUDIO_LOCK, SDL_UnlockAudio());
}

// 初始化SDL声卡
static void init_SDLAudio() {
  assert(audio_base[reg_init]);
  // 填入音频参数
  SDL_AudioSpec s = {};
  s.format = AUDIO_S16SYS;
  s.userdata = NULL;
  s.freq = (int)audio_base[reg_freq];
  s.channels = audio_base[reg_channels];
  s.samples = audio_base[reg_samples];
  s.callback = audioPlayer;

  // 初始化
  SDL_InitSubSystem(SDL_INIT_AUDIO);
  SDL_OpenAudio(&s, NULL);
  SDL_PauseAudio(0);
}

static void audio_io_handler(uint32_t offset, int len, bool is_write) {
  // 如果初始化音频参数成功，则初始化声卡
  if (is_write && offset == 0x10 && audio_base[reg_init]) init_SDLAudio();
}

void init_audio() {
  uint32_t space_size = sizeof(uint32_t) * nr_reg;
  audio_base = (uint32_t *)new_space(space_size);
#ifdef CONFIG_HAS_PORT_IO
  add_pio_map ("audio", CONFIG_AUDIO_CTL_PORT, audio_base, space_size, audio_io_handler);
#else
  add_mmio_map("audio", CONFIG_AUDIO_CTL_MMIO, audio_base, space_size, audio_io_handler);
#endif

  sbuf = (uint8_t *)new_space(CONFIG_SB_SIZE);
  audio_base[reg_sbuf_size] = CONFIG_SB_SIZE;
  audio_base[reg_count] = 0;
  add_mmio_map("audio-sbuf", CONFIG_SB_ADDR, sbuf, CONFIG_SB_SIZE, NULL);
}
