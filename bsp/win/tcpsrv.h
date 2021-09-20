#ifndef TCPSRV_H_
#define TCPSRV_H_

#include <stdbool.h>
#include "cmsis_os_v1.h"

/*
    server tcp

    Apre un socket udp (per i comandi) e un socket tcp di servizio
    sulla porta indicata
*/

#define TCPSRV_MSG_DIM      1500

struct TCP_SRV ;
typedef struct TCP_SRV TCP_SRV ;

typedef struct {
    size_t dim ;
    uint8_t mem[TCPSRV_MSG_DIM] ;
} TCPSRV_MSG ;

typedef struct {
    uint16_t porta ;

    // da qui vengono presi i messaggi TCPSRV_MSG
    osPoolId mp ;

    // callback:
    // connessione
    void (* conn)(const char * ip) ;
    // messaggio: alla fine qualcuno deve rimetterlo in mp
    void (* msg)(
        TCPSRV_MSG *,
        void *) ;
    // sconnessione
    void (* scon)(void) ;
} TCPSRV_CFG ;

TCP_SRV * TCPSRV_beg(
    TCPSRV_CFG *,
    void * /* passato assieme al messaggio */) ;
void TCPSRV_end(TCP_SRV * *) ;

bool TCPSRV_tx(TCP_SRV *, const void *, uint16_t) ;

#endif
