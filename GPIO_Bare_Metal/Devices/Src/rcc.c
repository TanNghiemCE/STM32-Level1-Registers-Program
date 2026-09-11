#include "rcc.h"

void rcc_HSE_config(void){
/*
 * Configuration parameters --> STM32F103 Clock Tree
 *
 * HSE = 8MHz
 * PLL_M = 9
 * USB prescaler = 1.5
 * AHB prescaler = 1
 * Cortex prescaler = 1
 * --> 72MHz System Clock
 *
 * APB1 prescaler = 2 --> 36, 72 MHz
 * APB2 prescaler = 1 --> 72MHz
 * ADC prescaler = 6 --> 12MHz
 */
 RCC->CFGR &= ~(RCC_CFGR_PLLSRC)
}