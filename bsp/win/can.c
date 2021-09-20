#include "can.h"
#include "tcpsrv.h"
#include <string.h>

#define DBGP_FILE
#include "includimi.h"

/*
 * Ad ogni can corrisponde una porta su localhost
 *
 * I messaggi scambiati hanno il formato che segue
 */

#pragma pack(1)
typedef struct {
    // 0 <= standard <= 0x3FF
    // 0x3FF < esteso <= 0x1FFFFFFF
    uint32_t id ;

    uint8_t dim ;
    uint8_t dati[MAX_DATI_CAN_FD] ;
} FINTO_MSG_CAN ;
#pragma pack()

#define PORTA_VCI   0xC725
#define PORTA_A_2   0xC726
#define PORTA_A_3   0xC727

osPoolDef(canPool, 100, TCPSRV_MSG) ;

typedef struct {
    const S_CAN_PRM * prm ;
    TCPSRV_CFG cfg ;
    TCP_SRV * srv ;
} UN_CAN ;

#ifdef DBGP_ABIL
static void stampa_uc(
    const char * titolo,
    UN_CAN * uc)
{
//    DBG_PRINTF("%s: %p", titolo, uc) ;
//
//    DBG_PRINTF("\tprm %p", uc->prm) ;
//    if ( uc->prm ) {
//        DBG_PRINTF("\t\tprm.fd=%s", uc->prm->fd ? "VERO" : "FALSO") ;
//        DBG_PRINTF("\t\tprm.numFstd=%d", uc->prm->numFstd) ;
//        DBG_PRINTF("\t\tprm.numFest=%d", uc->prm->numFest) ;
//        DBG_PRINTF("\t\tprm.rx_cb=%p", uc->prm->rx_cb) ;
//        DBG_PRINTF("\t\tprm.tx_cb=%p", uc->prm->tx_cb) ;
//    }
//
//    DBG_PRINTF("\tcfg.porta=%04X", uc->cfg.porta) ;
//    DBG_PRINTF("\tcfg.mp=%p", uc->cfg.mp) ;
//    DBG_PRINTF("\tcfg.conn=%p", uc->cfg.conn) ;
//    DBG_PRINTF("\tcfg.msg=%p", uc->cfg.msg) ;
//    DBG_PRINTF("\tcfg.scon=%p", uc->cfg.scon) ;
//
//    DBG_PRINTF("\tsrv=%p", uc->srv) ;
}

#else
static void stampa_uc(
    const char * titolo,
    UN_CAN * uc){}
#endif

#define VCI_1       0
#define AUTO_2      1
#define AUTO_3      2

static void a2_conn_cb(const char * ip)
{
    DBG_PRINTF("A2: conn %s", ip) ;
}

static void x_msg_cb(
    TCPSRV_MSG * msg,
    void * v)
{
    FINTO_MSG_CAN * fc = (FINTO_MSG_CAN *) msg->mem ;
    UN_CAN * pUC = v ;

    stampa_uc(__func__, pUC) ;
    DBG_PRINT_HEX(__func__, msg->mem, msg->dim) ;

    CAN_RX_H h = {
        .ext = fc->id > 0x3FF,
        .id = fc->id,
        .dim = fc->dim
    } ;

    pUC->prm->rx_cb(&h, fc->dati) ;

    // Adesso posso liberare
    CHECK( osOK == osPoolFree(pUC->cfg.mp, msg) ) ;
}

static void a2_scon_cb(void)
{
    DBG_PUTS("A2: scon") ;
}

static void a3_conn_cb(const char * ip)
{
    DBG_PRINTF("A3: conn %s", ip) ;
}

static void a3_scon_cb(void)
{
    DBG_PUTS("A3: scon") ;
}

static void vci_conn_cb(const char * ip)
{
    DBG_PRINTF("VCI: conn %s", ip) ;
}

static void vci_scon_cb(void)
{
    DBG_PUTS("VCI: scon") ;
}

static UN_CAN vUC[3] = {
    {
        .cfg = {
            .porta = PORTA_VCI,

            .conn = vci_conn_cb,
            .msg = x_msg_cb,
            .scon = vci_scon_cb
        }
    },
    {
        .cfg = {
            .porta = PORTA_A_2,

            .conn = a2_conn_cb,
            .msg = x_msg_cb,
            .scon = a2_scon_cb
        }
    },
    {
        .cfg = {
            .porta = PORTA_A_3,

            .conn = a3_conn_cb,
            .msg = x_msg_cb,
            .scon = a3_scon_cb
        }
    }
} ;

CAN_HANDLE CAN_vci(const S_CAN_PRM * cp)
{
    // Questa e' la prima chiamata: alloco
    // anche i pool degli altri due
    vUC[VCI_1].cfg.mp = osPoolCreate( osPool(canPool) ) ;
    vUC[AUTO_2].cfg.mp = osPoolCreate( osPool(canPool) ) ;
    vUC[AUTO_3].cfg.mp = osPoolCreate( osPool(canPool) ) ;

    vUC[VCI_1].prm = cp ;
    vUC[VCI_1].srv = TCPSRV_beg(&vUC[VCI_1].cfg, &vUC[VCI_1]) ;

    stampa_uc(__func__, &vUC[VCI_1]) ;

    return vUC + VCI_1 ;
}

CAN_HANDLE CAN_auto(int x)
{
    if ( 2 == x ) {
        return vUC + AUTO_2 ;
    }

    return vUC + AUTO_3 ;
}

void CAN_base_std(
    CAN_HANDLE ch,
    BASE_CAN * bc) {
	DBG_PRINTF("%s: ch %d",__func__,ch);
	DBG_PRINTF("\t Kb %d, perc %d",bc->Kbs, bc->perc);
}

void CAN_base_fd(
    CAN_HANDLE ch,
    BASE_CAN * n,
    BASE_CAN * d) {
	DBG_PRINTF("%s: ch %d",__func__,ch);
	DBG_PRINTF("\tN: Kb %d, perc %d",n->Kbs, n->perc);
	DBG_PRINTF("\tD: Kb %d, perc %d",d->Kbs, d->perc);
}

CAN_ERR CAN_iniz(
    CAN_HANDLE ch,
    const S_CAN_PRM * cp) { return 0 ; }

CAN_ERR CAN_filtri(
    CAN_HANDLE ch,
    const S_CAN_PRM * cp) { return 0 ; }

CAN_ERR CAN_glob_filt(
    CAN_HANDLE ch,
    bool a,
    bool b) { return 0 ; }

CAN_ERR CAN_cfg(CAN_HANDLE ch) { return 0 ; }

CAN_ERR CAN_attiva(
    CAN_HANDLE ch,
    const S_CAN_PRM * cp)
{
    UN_CAN * pUC = ch ;

    pUC->prm = cp ;
    pUC->srv = TCPSRV_beg(&pUC->cfg, pUC) ;

    stampa_uc(__func__, pUC) ;

    return 0 ;
}

void CAN_tx(
    CAN_HANDLE ch,
    const CAN_DATI * cd)
{
    FINTO_MSG_CAN fc = {
        .id = cd->id,
        .dim = cd->dim
    } ;
    UN_CAN * pUC = ch ;

    if ( fc.dim ) {
        memcpy(fc.dati, cd->dati, fc.dim) ;
    }

    stampa_uc(__func__, pUC) ;

    CHECK( TCPSRV_tx( pUC->srv, &fc, sizeof(fc)-MAX_DATI_CAN_FD+ fc.dim) ) ;
    pUC->prm->tx_cb() ;
}

bool CAN_txabile(CAN_HANDLE ch) { return true ; }

bool CAN_attivo(CAN_HANDLE ch)
{
    UN_CAN * pUC = ch ;

    return NULL != pUC->srv ;
}

void CAN_disab(CAN_HANDLE ch)
{
    UN_CAN * pUC = ch ;

    TCPSRV_end(&pUC->srv) ;
}
