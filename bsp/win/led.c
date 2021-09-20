#include "led.h"

//#define DBGP_FILE
#include "dbgp.h"

static bool led = false ;

void LED_accendi(bool x)
{
    led = x ;
    DBG_PRINTF("%s", led ? "VERDE" : "verde") ;
}

void LED_cambia(void)
{
    led = !led ;
    DBG_PRINTF("%s", led ? "VERDE" : "verde") ;
}
