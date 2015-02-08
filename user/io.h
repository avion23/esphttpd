#ifndef IO_H
#define IO_H


#define LEDGPIO0 14
#define BTNGPIO 0

void ICACHE_FLASH_ATTR ioLed(int ena);
void ioInit(void);

#endif /* header guard */
