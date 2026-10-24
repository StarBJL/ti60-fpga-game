/* Mock of the Sapphire BSP for tools/host_main_test.c (host build of src/main.c). */
#ifndef MOCK_BSP_H
#define MOCK_BSP_H
#include <stdint.h>
#include <stdarg.h>

#define BSP_CLINT               0
#define BSP_CLINT_HZ            100000000u
#define BSP_UART_TERMINAL       0
extern uint32_t mock_ocr[];                         /* "on-chip RAM" for mem_bench */
#define SYSTEM_RAM_A_CTRL       ((uintptr_t)mock_ocr)
#define SYSTEM_RAM_A_CTRL_SIZE  0x1000

void     bsp_init(void);
void     bsp_printf(const char *fmt, ...);
uint32_t clint_getTimeLow(int clint);
int      uart_readOccupancy(int uart);
char     uart_read(int uart);
int      uart_writeAvailability(int uart);
void     uart_write(int uart, char c);
#endif
