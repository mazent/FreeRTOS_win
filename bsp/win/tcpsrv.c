#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#ifdef __gnu_linux__
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#endif
#ifdef _WIN32
#include <ws2tcpip.h>
#include <windows.h>
#endif
#include "tcpsrv.h"

#define DBGP_FILE
#include "includimi.h"

#define STACK   2000

struct TCP_SRV {
    void * arg ;

    TCPSRV_CFG cfg ;

    osThreadId tid ;

    int cln ;
} ;

#ifdef _WIN32
// MZ
static bool wsa_ini = false ;
static int leggi(
    int sockfd,
    void * buf,
    size_t len)
{
    return recv(sockfd, buf, len, 0) ;
}

static int scrivi(
    int sockfd,
    const void * buf,
    size_t len)
{
    return send(sockfd, buf, len, 0) ;
}

#endif

#define CMD_ESCI        ( (uint32_t) 0x5C72EC95 )

static void opz_riusabile(int sock)
{
    int optval = 1 ;
    CHECK( 0 == setsockopt( sock, SOL_SOCKET, SO_REUSEADDR,
                            (const char *) &optval, sizeof(int) ) ) ;
}

static void opz_rxto(
    int sock,
    uint32_t milli)
{
    CHECK( 0 ==
           setsockopt( sock, SOL_SOCKET, SO_RCVTIMEO, (const char *) &milli,
                       sizeof(milli) ) ) ;
}

static void opz_nodel(int sock)
{
    int flag = 1 ;
    CHECK( 0 ==
           setsockopt( sock, IPPROTO_TCP, TCP_NODELAY,
                       (const char *) &flag,
                       sizeof(int) ) ) ;
}

// Il comando si invia al socket UDP

static bool invia_comando(
    TCP_SRV * pS,
    uint32_t cmd)
{
    bool esito = false ;
    int soc = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP) ;

    do {
        if ( soc < 0 ) {
            DBG_ERR ;
            break ;
        }

        opz_rxto(soc, 1000) ;

        struct sockaddr_in server = {
            0
        } ;
        server.sin_family = AF_INET ;
        server.sin_port = htons(pS->cfg.porta) ;
        server.sin_addr.s_addr = htonl(INADDR_LOOPBACK) ;

        if ( sendto( soc,
#ifdef __gnu_linux__
                     &cmd,
#elif defined _WIN32
                     (const char *) &cmd,
#endif
                     sizeof(cmd), 0,
                     (struct sockaddr *) &server, sizeof(server) ) < 0 ) {
            break ;
        }

        uint32_t rsp ;
        int n = recvfrom(soc,
#ifdef __gnu_linux__
                         &rsp,
#elif defined _WIN32
                         (char *) &rsp,
#endif
                         sizeof(rsp), 0, NULL, 0) ;
        if ( n != sizeof(rsp) ) {
            DBG_ERR ;
            break ;
        }

        esito = rsp == cmd ;
    } while ( false ) ;

    if ( soc >= 0 ) {
        close(soc) ;
    }

    return esito ;
}

static void tcpThd(void * v)
{
    int srvE, srvI ;
    fd_set active_fd_set, read_fd_set ;
    TCP_SRV * pSrv = v ;
    TCPSRV_MSG * msg = (TCPSRV_MSG *) osPoolAlloc(pSrv->cfg.mp) ;

    // Nessuno connesso
    pSrv->cln = -1 ;

    DBG_PRINTF("%s: %04X", __func__, pSrv->cfg.porta) ;

    do {
        FD_ZERO(&active_fd_set) ;

        struct sockaddr_in name = {
            0
        } ;
        name.sin_family = AF_INET ;
        name.sin_port = htons(pSrv->cfg.porta) ;

        // socket interno udp
        srvI = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP) ;
        if ( srvI < 0 ) {
            DBG_ERR ;
            break ;
        }

        name.sin_addr.s_addr = htonl(INADDR_LOOPBACK) ;

        opz_riusabile(srvI) ;

        if ( bind( srvI, (struct sockaddr *) &name, sizeof (name) ) < 0 ) {
            DBG_ERR ;
            break ;
        }

        FD_SET(srvI, &active_fd_set) ;

        /* Create the socket and set it up to accept connections. */
        srvE = socket(AF_INET, SOCK_STREAM, IPPROTO_IP) ;
        if ( srvE < 0 ) {
            DBG_ERR ;
            break ;
        }

        opz_riusabile(srvE) ;
        opz_nodel(srvE) ;

        //name.sin_addr.s_addr = htonl(INADDR_ANY);
        name.sin_addr.s_addr = htonl(INADDR_LOOPBACK) ;

        if ( bind( srvE, (struct sockaddr *) &name, sizeof (name) ) < 0 ) {
            DBG_ERR ;
            break ;
        }

        if ( listen(srvE, 1) < 0 ) {
            DBG_ERR ;
            break ;
        }

        // DA FARE: registrarsi con mDNS

        FD_SET(srvE, &active_fd_set) ;

        bool continua = true ;
        while ( continua ) {
            /* Block until input arrives on one or more active sockets. */
            read_fd_set = active_fd_set ;

            int sel = select(FD_SETSIZE, &read_fd_set, NULL, NULL, NULL) ;
            if ( sel < 0 ) {
                DBG_ERR ;
                break ;
            }

            if ( 0 == sel ) {
                // ?timeout?
                DBG_ERR ;
                continue ;
            }

            /* Service all the sockets with input pending. */
            if ( FD_ISSET(srvI, &read_fd_set) ) {
                // Comando al server
                uint32_t cmd ;
                int nbytes = leggi( srvI, &cmd, sizeof(cmd) ) ;

                DBG_QUA ;

                do {
                    if ( nbytes != sizeof(cmd) ) {
                        DBG_ERR ;
                        break ;
                    }

                    if ( CMD_ESCI != cmd ) {
                        DBG_ERR ;
                        break ;
                    }

                    if ( pSrv->cln >= 0 ) {
                        close(pSrv->cln) ;
                        pSrv->cln = -1 ;
                    }

                    close(srvE) ;

                    // conferma al comando
                    (void) sendto(srvI,
#ifdef __gnu_linux__
                                  &cmd,
#elif defined _WIN32
                                  (const char *) &cmd,
#endif
                                  sizeof(cmd),
                                  0,
                                  NULL,
                                  0) ;

                    osDelay(100) ;
                    close(srvI) ;

                    continua = false ;
                } while ( false ) ;

                if ( !continua ) {
                    break ;
                }
            }

            if ( FD_ISSET(srvE, &read_fd_set) ) {
                /* Connection request on original socket. */
                struct sockaddr_in clientname ;
                socklen_t size = sizeof (clientname) ;
                pSrv->cln = accept(srvE,
                                   (struct sockaddr *) &clientname,
                                   &size) ;
                if ( pSrv->cln >= 0 ) {
                    const char * ip = inet_ntoa(clientname.sin_addr) ;

                    FD_SET(pSrv->cln, &active_fd_set) ;

                    pSrv->cfg.conn(ip) ;

                    DBG_PRINTF("TCP %d <-> %s", pSrv->cln, ip) ;
                }
            }

            if ( -1 == pSrv->cln ) {
                continue ;
            }

            if ( FD_ISSET(pSrv->cln, &read_fd_set) ) {
                /* Data arriving on an already-connected socket. */
                do {
                    if ( NULL == msg ) {
                        // Riprovo
                        msg = (TCPSRV_MSG *) osPoolAlloc(pSrv->cfg.mp) ;

                        if ( NULL == msg ) {
                            DBG_ERR ;
                            break ;
                        }
                    }

                    int nbytes = leggi(pSrv->cln, msg->mem, TCPSRV_MSG_DIM) ;
                    if ( nbytes <= 0 ) {
                        // sconnesso!
                        close(pSrv->cln) ;
                        FD_CLR(pSrv->cln, &active_fd_set) ;

                        pSrv->cln = -1 ;

                        pSrv->cfg.scon() ;
                    }
                    else {
                        /* Data read. */
                        DBG_PRINT_HEX("<- TCP", msg->mem, nbytes) ;

                        msg->dim = nbytes ;

                        pSrv->cfg.msg(msg, pSrv->arg) ;

                        msg = (TCPSRV_MSG *) osPoolAlloc(pSrv->cfg.mp) ;
                    }
                } while ( false ) ;
            }
        }
    } while ( false ) ;

    pSrv->tid = NULL ;
    DBG_PRINTF("fine %s (%04X)", __func__, pSrv->cfg.porta) ;
    CHECK( osOK == osThreadTerminate(NULL) ) ;
}

TCP_SRV * TCPSRV_beg(
    TCPSRV_CFG * pCfg,
    void * arg)
{
    TCP_SRV * srv = NULL ;

    do {
        assert(pCfg) ;
        if ( NULL == pCfg ) {
            break ;
        }

        assert(pCfg->mp) ;
        if ( NULL == pCfg->mp ) {
            break ;
        }

        assert(pCfg->conn) ;
        if ( NULL == pCfg->conn ) {
            break ;
        }

        assert(pCfg->scon) ;
        if ( NULL == pCfg->scon ) {
            break ;
        }

        assert(pCfg->msg) ;
        if ( NULL == pCfg->msg ) {
            break ;
        }

#ifdef _WIN32
        if ( !wsa_ini ) {
            WSADATA wsaData ;
            int iResult ;

            // Initialize Winsock
            iResult = WSAStartup(MAKEWORD(2, 2), &wsaData) ;
            if ( iResult != 0 ) {
                DBG_PRINTF("WSAStartup failed: %d", iResult) ;
                break ;
            }

            wsa_ini = true ;
        }
#endif

        srv = (TCP_SRV *) ose_malloc( sizeof(TCP_SRV) ) ;
        assert(srv) ;
        if ( NULL == srv ) {
            DBG_ERR ;
            break ;
        }

        srv->arg = arg ;
        srv->cfg = *pCfg ;
        srv->cln = -1 ;

        osThreadDef(tcpThd, osPriorityNormal, 1, STACK) ;
        srv->tid = osThreadCreate(osThread(tcpThd), srv) ;
        assert(srv->tid) ;
        if ( NULL == srv->tid ) {
            DBG_ERR ;
            ose_free(srv) ;
            srv = NULL ;
            break ;
        }
    } while ( false ) ;

    return srv ;
}

void TCPSRV_end(TCP_SRV * * x)
{
    do {
        if ( NULL == x ) {
            break ;
        }

        TCP_SRV * pS = *x ;
        if ( NULL == pS ) {
            break ;
        }

        *x = NULL ;

        while ( pS->tid ) {
            (void) invia_comando(pS, CMD_ESCI) ;

            osDelay(100) ;
        }

        ose_free(pS) ;
    } while ( false ) ;
}

bool TCPSRV_tx(
    TCP_SRV * pS,
    const void * buf,
    uint16_t count)
{
    if ( NULL == pS ) {
        DBG_ERR ;
        return false ;
    }
    else if ( pS->cln < 0 ) {
        DBG_ERR ;
        return false ;
    }
    else {
        DBG_PRINT_HEX("-> TCP", buf, count) ;

        int s = scrivi(pS->cln, buf, count) ;
        if ( s < 0 ) {
            DBG_ERR ;
            return false ;
        }
        else {
            return s == count ;
        }
    }
}
