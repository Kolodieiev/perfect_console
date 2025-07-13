#include "debug.h"

#define DEBUG_DATA0_ADDRESS ((volatile uint32_t *)0xE00000F4)
#define DEBUG_DATA1_ADDRESS ((volatile uint32_t *)0xE00000F8)

#ifdef MILLIS
volatile unsigned long __ms = 0;
#endif
#ifdef MICROS
volatile unsigned long __us = 0;
#endif

void initMillis() {
    *DEBUG_DATA0_ADDRESS = 0;

#ifdef MILLIS
    NVIC_EnableIRQ (SysTicK_IRQn);
    SysTick->SR &= ~(1 << 0);
    SysTick->CMP = SystemCoreClock / 1000 - 1;
    SysTick->CNT = 0;
    SysTick->CTLR = 0xF;
#endif

#ifdef MICROS
    RCC->PB1PCENR |= RCC_TIM2EN;

    NVIC_EnableIRQ (TIM2_IRQn);
    TIM2->CTLR1 = 0;                            // TIM_ARPE
    TIM2->PSC = SystemCoreClock / 1000000 - 1;  // 1 MHz
    TIM2->CNT = 0;
    //  TIM2->ATRLR = 0xFFFF;
    TIM2->SWEVGR = TIM_UG;
    TIM2->INTFR = 0;  // ~TIM_UIF
    TIM2->DMAINTENR |= TIM_UIE;
    TIM2->CTLR1 |= TIM_CEN;
#endif
}

#ifdef MILLIS
void delay (uint32_t ms) {
    ms += __ms;
    while ((int32_t)__ms < (int32_t)ms) {
        __WFI();
    }
}
#endif

#ifdef MICROS
uint32_t micros() {
    return __us | TIM2->CNT;
}

void delayMicroseconds (uint32_t us) {
    us += __us;
    while ((int32_t)(__us | TIM2->CNT) < (int32_t)us) { }
}
#endif

#ifdef MILLIS
__attribute__ ((interrupt ("WCH-Interrupt-fast"))) void SysTick_Handler (void) {
    ++__ms;
    SysTick->SR = 0;
}
#endif

#ifdef MICROS
__attribute__ ((interrupt ("WCH-Interrupt-fast"))) void TIM2_IRQHandler (void) {
    if (TIM2->INTFR & TIM_UIF) {
        __us += 0x10000;
        TIM2->INTFR &= ~TIM_UIF;
    }
}
#endif

void initUsartPrintf (uint32_t baudrate) {
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

#if (DEBUG == DEBUG_UART1_NoRemap)
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOD | RCC_PB2Periph_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOD, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART1_Remap1)
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOD | RCC_PB2Periph_USART1 | RCC_PB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig (GPIO_PartialRemap1_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOD, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART1_Remap2)
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOD | RCC_PB2Periph_USART1 | RCC_PB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig (GPIO_PartialRemap2_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOD, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART1_Remap3)
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOC | RCC_PB2Periph_USART1 | RCC_PB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig (GPIO_PartialRemap3_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOC, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART1_Remap4)
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOD | RCC_PB2Periph_USART1 | RCC_PB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig (GPIO_PartialRemap4_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOD, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART1_Remap5)
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOB | RCC_PB2Periph_USART1 | RCC_PB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig (GPIO_PartialRemap5_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOB, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART1_Remap6)
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOC | RCC_PB2Periph_USART1 | RCC_PB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig (GPIO_PartialRemap6_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOC, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART1_Remap7)
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOB | RCC_PB2Periph_USART1 | RCC_PB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig (GPIO_PartialRemap7_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOB, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART1_Remap8)
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOA | RCC_PB2Periph_USART1 | RCC_PB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig (GPIO_PartialRemap8_USART1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART2_NoRemap)
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOA | RCC_PB2Periph_USART2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART2_Remap1)
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOA | RCC_PB2Periph_USART2 | RCC_PB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig (GPIO_PartialRemap1_USART2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART2_Remap2)
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOA | RCC_PB2Periph_USART2 | RCC_PB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig (GPIO_PartialRemap2_USART2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART2_Remap3)
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOD | RCC_PB2Periph_USART2 | RCC_PB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig (GPIO_PartialRemap3_USART2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOD, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART2_Remap4)
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOB | RCC_PB2Periph_USART2 | RCC_PB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig (GPIO_PartialRemap4_USART2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOB, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART2_Remap5)
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOC | RCC_PB2Periph_USART2 | RCC_PB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig (GPIO_PartialRemap5_USART2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOC, &GPIO_InitStructure);

#elif (DEBUG == DEBUG_UART2_Remap6)
    RCC_PB2PeriphClockCmd (RCC_PB2Periph_GPIOA | RCC_PB2Periph_USART2 | RCC_PB2Periph_AFIO, ENABLE);
    GPIO_PinRemapConfig (GPIO_FullRemap_USART2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init (GPIOA, &GPIO_InitStructure);

#endif

    USART_InitStructure.USART_BaudRate = baudrate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx;

#if (DEBUG == DEBUG_UART1_NoRemap) || (DEBUG == DEBUG_UART1_Remap1) || (DEBUG == DEBUG_UART1_Remap2) || (DEBUG == DEBUG_UART1_Remap3) || (DEBUG == DEBUG_UART1_Remap4) || (DEBUG == DEBUG_UART1_Remap5) || (DEBUG == DEBUG_UART1_Remap6) || (DEBUG == DEBUG_UART1_Remap7) || (DEBUG == DEBUG_UART1_Remap8)
    USART_Init (USART1, &USART_InitStructure);
    USART_Cmd (USART1, ENABLE);
#endif

#if (DEBUG == DEBUG_UART2_NoRemap) || (DEBUG == DEBUG_UART2_Remap1) || (DEBUG == DEBUG_UART2_Remap2) || (DEBUG == DEBUG_UART2_Remap3) || (DEBUG == DEBUG_UART2_Remap4) || (DEBUG == DEBUG_UART2_Remap5) || (DEBUG == DEBUG_UART2_Remap6)
    USART_Init (USART2, &USART_InitStructure);
    USART_Cmd (USART2, ENABLE);
#endif
}

/*********************************************************************
 * @fn      _write
 *
 * @brief   Support Printf Function
 *
 * @param   *buf - UART send Data.
 *          size - Data length.
 *
 * @return  size - Data length
 */
__attribute__ ((used)) int _write (int fd, char *buf, int size) {
    int i = 0;
    int writeSize = size;
#if (SDI_PRINT == SDI_PR_OPEN)
    do {
        /**
         * data0  data1 8 bytes
         * data0 The lowest byte storage length, the maximum is 7
         *
         */

        while ((*(DEBUG_DATA0_ADDRESS) != 0u)) {
        }

        if (writeSize > 7) {
            *(DEBUG_DATA1_ADDRESS) = (*(buf + i + 3)) | (*(buf + i + 4) << 8) | (*(buf + i + 5) << 16) | (*(buf + i + 6) << 24);
            *(DEBUG_DATA0_ADDRESS) = (7u) | (*(buf + i) << 8) | (*(buf + i + 1) << 16) | (*(buf + i + 2) << 24);

            i += 7;
            writeSize -= 7;
        } else {
            *(DEBUG_DATA1_ADDRESS) = (*(buf + i + 3)) | (*(buf + i + 4) << 8) | (*(buf + i + 5) << 16) | (*(buf + i + 6) << 24);
            *(DEBUG_DATA0_ADDRESS) = (writeSize) | (*(buf + i) << 8) | (*(buf + i + 1) << 16) | (*(buf + i + 2) << 24);

            writeSize = 0;
        }

    } while (writeSize);

#else

    for (i = 0; i < size; i++) {
#if (DEBUG == DEBUG_UART1_NoRemap) || (DEBUG == DEBUG_UART1_Remap1) || (DEBUG == DEBUG_UART1_Remap2) || (DEBUG == DEBUG_UART1_Remap3) || (DEBUG == DEBUG_UART1_Remap4) || (DEBUG == DEBUG_UART1_Remap5) || (DEBUG == DEBUG_UART1_Remap6) || (DEBUG == DEBUG_UART1_Remap7) || (DEBUG == DEBUG_UART1_Remap8)
        while (USART_GetFlagStatus (USART1, USART_FLAG_TC) == RESET);
        USART_SendData (USART1, *buf++);
#elif (DEBUG == DEBUG_UART2_NoRemap) || (DEBUG == DEBUG_UART2_Remap1) || (DEBUG == DEBUG_UART2_Remap2) || (DEBUG == DEBUG_UART2_Remap3) || (DEBUG == DEBUG_UART2_Remap4) || (DEBUG == DEBUG_UART2_Remap5) || (DEBUG == DEBUG_UART2_Remap6)
        while (USART_GetFlagStatus (USART2, USART_FLAG_TC) == RESET);
        USART_SendData (USART2, *buf++);
#endif
    }


#endif
    return writeSize;
}

/*********************************************************************
 * @fn      _sbrk
 *
 * @brief   Change the spatial position of data segment.
 *
 * @return  size: Data length
 */
__attribute__ ((used)) void *_sbrk (ptrdiff_t incr) {
    extern char _end[];
    extern char _heap_end[];
    static char *curbrk = _end;

    if ((curbrk + incr < _end) || (curbrk + incr > _heap_end))
        return NULL - 1;

    curbrk += incr;
    return curbrk - incr;
}

void printByteBinary (uint8_t byte) {
    for (int i = 7; i >= 0; --i) {
        putchar ((byte >> i) & 1 ? '1' : '0');
    }
}

void printArrayBinary (const uint8_t *array, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        printf ("[%zu] - ", i);
        printByteBinary (array[i]);
        putchar ('\n');
    }
}
