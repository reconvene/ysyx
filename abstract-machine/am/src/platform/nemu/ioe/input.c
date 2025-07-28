#include <am.h>
#include <nemu.h>
#include <stdint.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  uint32_t keyBits=inl(KBD_ADDR);
  kbd->keydown = keyBits & KEYDOWN_MASK;
  kbd->keycode = (uint8_t)keyBits;
}
