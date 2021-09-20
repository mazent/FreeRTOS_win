#include "dbgp.h"

#ifdef DBGP_GLOB

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef X_WIN
#	include <string.h>
#endif
#include "cmsis_os.h"

#if 0
// Vedo le stampe mescolate
#   define ENTER_CS     ose_EnterCS()
#   define LEAVE_CS     ose_LeaveCS()
#else
// Le stampe sono giuste
#   define ENTER_CS
#   define LEAVE_CS
#endif

static void sbarre(char * stringa)
{
    char * barra = strchr(stringa, '\\') ;
    while ( barra ) {
        *barra = '/' ;
        barra = strchr(barra + 1, '\\') ;
    }
}

void DBG_print_hex(
    const char * titolo,
    const void * v,
    const int dim)
{
    ENTER_CS ;

    if ( (0 == dim) || (NULL == v) ) {
        // Vuoto!
    }
    else {
        const uint8_t * msg = (const uint8_t *) v ;

        if ( NULL != titolo ) {
            printf(titolo) ;
        }

        char tmp[8] ;
        (void) sprintf(tmp, "[%d] ", dim) ;
        printf(tmp) ;

        for ( int i = 0 ; i < dim ; i++ ) {
            (void) sprintf(tmp, "%02X ", msg[i]) ;
            printf(tmp) ;
        }

        puts("") ;
#ifdef X_WIN
        fflush(stdout) ;
#endif
    }

    LEAVE_CS ;
}

void DBG_printf(
    const char * fmt,
    ...)
{
    va_list args ;

    va_start(args, fmt) ;

    ENTER_CS ;

#if 1
    // Alloco e libero
    int dim = vsnprintf(NULL, 0, fmt, args) ;

    char * buf = malloc(dim + 1) ;
    if ( buf ) {
        (void) vsnprintf(buf, dim + 1, fmt, args) ;

        sbarre(buf);

        printf(buf) ;
        puts("") ;
        free(buf) ;
    }
#else
    // Uso uno spazio fisso
    static char buf[200] ;

    (void) vsnprintf(buf, sizeof(buf), fmt, args) ;

    sbarre(buf);

    printf(buf) ;
    puts("") ;
#endif
#ifdef X_WIN
    fflush(stdout) ;
#endif

    LEAVE_CS ;

    va_end(args) ;
}

void DBG_puts(const char * msg)
{
    ENTER_CS ;

    puts(msg) ;
#ifdef X_WIN
    fflush(stdout) ;
#endif

    LEAVE_CS ;
}

void DBG_putc(char c)
{
    ENTER_CS ;

    putchar(c) ;
#ifdef X_WIN
    fflush(stdout) ;
#endif

    LEAVE_CS ;
}

#endif
