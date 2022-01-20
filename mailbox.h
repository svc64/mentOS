#include <stdint.h>

extern volatile uint32_t  __attribute__((aligned(16))) mbox[36];

#define MBOX_BASE       0xB880
#define MBOX_READ       (MBOX_BASE + 0x00)
#define MBOX_STATUS     (MBOX_BASE + 0x18)
#define MBOX_WRITE      (MBOX_BASE + 0x20)

#define MBOX_RESPONSE           0x80000000
#define MBOX_FULL               0x80000000
#define MBOX_EMPTY              0x40000000

#define MBOX_TAG_GETSERIAL      0x10004
#define MBOX_TAG_SETPOWER       0x28001
#define MBOX_TAG_SETCLKRATE     0x38002
#define MBOX_TAG_LAST           0

#define MBOX_CH_POWER           0
#define MBOX_CH_FB              1
#define MBOX_CH_VUART           2
#define MBOX_CH_VCHIQ           3
#define MBOX_CH_LEDS            4
#define MBOX_CH_BTNS            5
#define MBOX_CH_TOUCH           6
#define MBOX_CH_COUNT           7
#define MBOX_CH_PROP            8

uint32_t mbox_send(uint8_t channel);
