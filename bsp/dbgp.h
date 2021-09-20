#ifndef DBGP_H_
#define DBGP_H_

#include <stdbool.h>

#if defined DBGP_GLOB && defined NDEBUG
#	warning OKKIO
#	undef DBGP_GLOB
#endif

#ifdef DBGP_GLOB
// Abilitate globalmente ...
#   ifdef DBGP_FILE
// ... e anche localmente
#   define DBGP_ABIL    1
#   else
// ... ma disabilitate localmente
#   endif
#else
// Disabilitate globalmente
#endif

#ifdef DBGP_ABIL

extern void DBG_print_hex(
	    const char * titolo,
	    const void * v,
	    const int dim);
extern void DBG_printf(
	    const char * fmt,
	    ...);
extern void DBG_puts(const char * msg);
extern void DBG_putc(char c);

#   define DBG_FUN                  DBG_printf("%s", __func__)
#   define DBG_QUA                  DBG_printf("QUA %s %d", __FILE__, __LINE__)
#   define DBG_ERR                  DBG_printf("ERR %s %d", __FILE__, __LINE__)
#   define DBG_PRINTF(f, ...)       DBG_printf(f, ## __VA_ARGS__)
#   define DBG_PUTS(a)              DBG_puts(a)
#   define DBG_PRINT_HEX(t, x, d)   DBG_print_hex(t, x, d)
#	define DBG_PCHAR(c)				DBG_putc(c)
#else
#   define DBG_FUN
#   define DBG_QUA
#   define DBG_ERR
#   define DBG_PRINTF(f, ...)
#   define DBG_PUTS(a)
#   define DBG_PRINT_HEX(t, x, d)
#	define DBG_PCHAR(c)
#endif

#define CHECK(x)        \
    do {                \
        if ( !(x) ) {   \
            DBG_ERR ;   \
        }               \
    } while (false)


#endif
