#ifndef INC_RCC_H_
#define INC_RCC_H_

#include "main.h"
#include "stm32f103xb.h"
#include <stdint.h>
//HSE Configuration
void rcc_HSE_config(void);
//SysTick Configuration
void rcc_SysTick_config(uint32_t arr);

//Increment ms Ticks
void rcc_msIncTicks(void);

//Get ms Tick
uint32_t rcc_msGetTicks(void);

//ms Delay
void rcc_msDelay(uint32_t ms);
#endif