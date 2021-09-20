#ifndef BSP_LED_H_
#define BSP_LED_H_

#include <stdbool.h>

void LED_accendi(bool) ;
void LED_cambia(void) ;

#else
#   warning led.h incluso
#endif
