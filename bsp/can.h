#ifndef BSP_CAN_H_
#define BSP_CAN_H_

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define MAX_DATI_CAN        8
#define MAX_DATI_CAN_FD     64

// 0 == nessun errore
typedef uint16_t CAN_ERR ;

typedef void * CAN_HANDLE ;

typedef struct {
    bool ext ;
    uint32_t id ;

    size_t dim ;
} CAN_RX_H ;

typedef void (*PF_CAN_RX)(const CAN_RX_H *, const uint8_t *) ;

typedef void (*PF_CAN_TX)(void) ;

typedef struct {
    uint32_t id1 ;
    uint32_t id2 ;
    enum {
        UF_TIPO_RANGE = 0,
        UF_TIPO_DUAL,
        UF_TIPO_MASK
    } tipo ;
    bool scarta ;
} UN_FILTRO ;

#define MAX_FLT_STD        28
#define MAX_FLT_EST         8

typedef struct {
    // Parametri
    bool fd ;
    int compensa ;

    // Filtri
    size_t numFstd ;
    UN_FILTRO fStd[MAX_FLT_STD] ;

    size_t numFest ;
    UN_FILTRO fEst[MAX_FLT_EST] ;

    // Callback
    // Ricevuto messaggio
    PF_CAN_RX rx_cb ;
    // Trasmissione conclusa
    PF_CAN_TX tx_cb ;
} S_CAN_PRM ;

// Inizializza e attiva il can1 (lato vci, parametri fissi)
CAN_HANDLE CAN_vci(const S_CAN_PRM *) ;

// Ottiene l'handle dei can lato automobile (parametri variabili)
CAN_HANDLE CAN_auto(int /* 2 o 3 */) ;

typedef struct {
    uint32_t Kbs ;
    uint8_t perc ;
} BASE_CAN ;

// Imposta i parametri base
void CAN_base_std(CAN_HANDLE, BASE_CAN *) ;
void CAN_base_fd(CAN_HANDLE, BASE_CAN *, BASE_CAN *) ;

// Inizia con questa
CAN_ERR CAN_iniz(CAN_HANDLE, const S_CAN_PRM *) ;

// Poi imposta i filtri (se ne hai)
CAN_ERR CAN_filtri(CAN_HANDLE, const S_CAN_PRM *) ;

// Se hai filtri, digli cosa fare dei messaggi che sono svicolati (primo bool)
// e dei remote (secondo bool)
// Se veri li scarta
CAN_ERR CAN_glob_filt(CAN_HANDLE, bool, bool) ;

// Finalmente puoi accendere ...
CAN_ERR CAN_cfg(CAN_HANDLE) ;

uint16_t CAN_compensa(CAN_HANDLE ch) ;

// ... e abilitare le interruzioni
CAN_ERR CAN_attiva(CAN_HANDLE, const S_CAN_PRM *) ;

typedef struct {
    bool ide ;
    uint32_t id ;

    uint8_t dim ;
    uint8_t * dati ;
} CAN_DATI ;

void CAN_tx(CAN_HANDLE, const CAN_DATI *) ;

bool CAN_txabile(CAN_HANDLE) ;
bool CAN_attivo(CAN_HANDLE) ;

void CAN_disab(CAN_HANDLE) ;

#endif
