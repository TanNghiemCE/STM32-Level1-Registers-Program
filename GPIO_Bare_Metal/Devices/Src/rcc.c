#include "rcc.h"
#include "stm32f103xb.h"
#include <stdint.h>

//ms Tick
static __IO uint32_t msTicks = 0; // w/r permission (volatile)

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
 // ==============================================================================
// CLOCK CONFIGURATION SEQUENCE:
// 1. Enable HSE -> 2. Wait HSE Ready -> 3. Flash Latency (2WS) 
// -> 4. Prescalers & PLL Config (PLLOFF) -> 5. Enable PLL -> 6. Wait PLL Ready 
// -> 7. Switch SYSCLK to PLL -> 8. Wait SWS Switch Complete
// ==============================================================================
    RCC->CR |= RCC_CR_HSEON;
    while (!(RCC->CR & RCC_CR_HSERDY));
    /*
    *Flash Latency là số chu kỳ CPU phải chờ để đọc được data từ flash , 72MHz -> 2 wait states
    */
    FLASH->ACR &= ~(FLASH_ACR_LATENCY); //reset
    FLASH->ACR |= FLASH_ACR_LATENCY_2;
    FLASH->ACR |= FLASH_ACR_PRFTBE; // trong khi cpu đang execute thì flash lưu tiến trình tiếp theo vào buffer -> tránh để cpu chờ
    RCC->CR &= ~RCC_CR_PLLON; // đảm bảo tắt

    RCC->CFGR &= ~RCC_CFGR_USBPRE;
    
    RCC->CFGR &= ~RCC_CFGR_ADCPRE;
    RCC->CFGR |= RCC_CFGR_ADCPRE_DIV6;

    RCC->CFGR &= ~RCC_CFGR_HPRE;//reset AHB
    RCC->CFGR |= RCC_CFGR_HPRE_DIV1;// /1

    RCC->CFGR &= ~RCC_CFGR_PPRE1;//reset APB1
    RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;// /2

    RCC->CFGR &= ~RCC_CFGR_PPRE2;//reset APB1
    RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;// /2

    RCC->CFGR &= ~RCC_CFGR_PLLSRC;
    RCC->CFGR |= RCC_CFGR_PLLSRC;
    RCC->CFGR &= ~RCC_CFGR_PLLMULL; //reset
    RCC->CFGR |= RCC_CFGR_PLLMULL9;//PLL*9
    RCC->CR |= RCC_CR_PLLON;

    while(!(RCC->CR & RCC_CR_PLLRDY));

    RCC->CFGR &= ~(RCC_CFGR_SW); //reset
    RCC->CFGR |= (RCC_CFGR_SW_PLL);
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL);
}

void rcc_SysTick_config(uint32_t arr){ 
    /*
    * Enable -> VAL = LOAD -> VAL - 1 -> VAL = 0 -> bật cờ COUNTFLAG, Ngắt NVIC -> VAL = LOAD
    */
  SysTick->CTRL = 0; //reset control register 
  SysTick->LOAD = arr - 1; // set load value
  NVIC_SetPriority(SysTick_IRQn,0); // set NVIC
  SysTick->VAL = 0; // reset val
  SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk; //enable CLKSOURCE
  SysTick->CTRL |= SysTick_CTRL_TICKINT_Msk; //enable TICKINT
  SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk; // enable
}

//Increment ms Ticks
void rcc_msIncTicks(void){
    msTicks++;
}

//Get ms Tick
uint32_t rcc_msGetTicks(void){
    return msTicks;
}

//ms Delay
void rcc_msDelay(uint32_t ms){
    uint32_t startTicks = rcc_msGetTicks();
    while (rcc_msGetTicks()-startTicks <ms);
}

void SysTick_Handler(void){
    NVIC_ClearPendingIRQ(SysTick_IRQn);
    rcc_msIncTicks();
}