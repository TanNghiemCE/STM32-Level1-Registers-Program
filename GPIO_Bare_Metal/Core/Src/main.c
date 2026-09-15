#include "main.h"
#include "rcc.h"
#include "stm32f103xb.h"

int main(void){
    rcc_HSE_config();
    rcc_SysTick_config(720000);
    while (1) {
    
    }
}
