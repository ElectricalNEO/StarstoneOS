#include "common.h"

#define PIC1 0x20
#define PIC2 0xa0

#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1+1)

#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2+1)

#define PIC1_OFFSET 0x20
#define PIC2_OFFSET 0x28

#define PIC_EOI 0x20

#define ICW1_ICW4 0x01
#define ICW1_SIGNLE 0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL 0x08
#define ICW1_INIT 0x10

#define ICW4_8086 0x01
#define ICW4_AUTO 0x02
#define ICW4_BUF_SLAVE 0x04
#define ICW4_BUF_MASTER 0x08
#define ICW4_SFNM 0x10

void remap_pic();

void pic_master_set_mask(uint8_t mask);
void pic_slave_set_mask(uint8_t mask);

void pic_end_master();
void pic_end_slave();
