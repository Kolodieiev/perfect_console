#ifndef __DEBUG_H
#define __DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ch32v00X.h>
#include <stdio.h>

/* UART Printf Definition */
#define DEBUG_UART1_NoRemap 1  // Tx-PD5
#define DEBUG_UART1_Remap1 2   // Tx-PD6
#define DEBUG_UART1_Remap2 3   // Tx-PD0
#define DEBUG_UART1_Remap3 4   // Tx-PC0
#define DEBUG_UART1_Remap4 5   // Tx-PD1
#define DEBUG_UART1_Remap5 6   // Tx-PB3
#define DEBUG_UART1_Remap6 7   // Tx-PC5
#define DEBUG_UART1_Remap7 8   // Tx-PB5
#define DEBUG_UART1_Remap8 9   // Tx-PA0

/* USART2 print function only for V005,V006,V007,M007 series*/
#if defined(CH32V005) || defined(CH32V006) || defined(CH32V007_M007)
#define DEBUG_UART2_NoRemap 10  // Tx-PA7
#define DEBUG_UART2_Remap1 11   // Tx-PA4
#define DEBUG_UART2_Remap2 12   // Tx-PA2
#define DEBUG_UART2_Remap3 13   // Tx-PD2
#define DEBUG_UART2_Remap4 14   // Tx-PB0
#define DEBUG_UART2_Remap5 15   // Tx-PC4
#define DEBUG_UART2_Remap6 16   // Tx-PA6
#endif
/* DEBUG UATR Definition */
#ifndef DEBUG
#define DEBUG DEBUG_UART1_NoRemap
#endif

/* SDI Printf Definition */
#define SDI_PR_CLOSE 0
#define SDI_PR_OPEN 1

#ifndef SDI_PRINT
#define SDI_PRINT SDI_PR_CLOSE
#endif

#define MILLIS
// #define MICROS

#ifdef MILLIS
extern volatile unsigned long __ms;

#define millis() __ms
#endif

void initMillis (void);
#ifdef MICROS
uint32_t micros (void);
void delayMicroseconds (uint32_t us);
#endif
#ifdef MILLIS
void delay (uint32_t ms);
#endif

void initUsartPrintf (uint32_t baudrate);

void printArrayBinary(const uint8_t *array, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* __DEBUG_H */
