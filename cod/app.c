#define DBGP_FILE
#include "includimi.h"

#include "cmsis_os_v1.h"

/*
 * Creare un progetto C/C++ per mingw
 *
 * Aggiungere gli include
 *
 * Aggiungere le librerie winmm e ws2_32
 */

static void stampa(void * v)
{
    static int conta = 0 ;

    INUTILE(v) ;

    conta += 1 ;
    DBG_PRINTF("%s %d", __func__, conta) ;
}

void app(void)
{
    osTimerDef(led, stampa) ;
    osTimerId tid = osTimerCreate(osTimer(led), osTimerPeriodic, NULL) ;
    CHECK( osOK == osTimerStart(tid, 1000) ) ;

    /* Start scheduler */
    osKernelStart() ;

    /* We should never get here as control is now taken by the scheduler */
    /* Infinite loop */
    while ( 1 ) {}
}
