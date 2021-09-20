#ifndef BSP_UTILI_H_
#define BSP_UTILI_H_

#include <stdint.h>

// Utili
// ==========================================
#define INUTILE(x)      (void) ( sizeof(x) )
#define NOT(x)          ( ~(unsigned int) (x) )
#define MINI(a, b)      ( (a) < (b) ? (a) : (b) )
#define ABS(x)          ( (x) < 0 ? -(x) : (x) )
#define DIM_VETT(a)     (sizeof(a) / sizeof(a)[0])

// Varie
// ==========================================

// Conversione a stringa (token stringification)
//    #define foo 4
//    STRINGA(foo)  -> "foo"
//    STRINGA2(foo) -> "4"
#ifndef STRINGA
#	define STRINGA(a)       # a
#	define STRINGA2(a)      STRINGA(a)
#endif

// Se manca la define
#define SIZEOF_MEMBER(STRUCT, MEMBER) sizeof( ( (STRUCT *) NULL )->MEMBER )

// https://en.wikipedia.org/wiki/Offsetof
#ifndef offsetof
#define offsetof(STRUCT, MEMBER)     \
    ( (size_t)& ( ( (STRUCT *) NULL )->MEMBER ) )
#endif
#define container_of(MEMBER_PTR, STRUCT, MEMBER)    \
    ( (STRUCT *) ( (char *) (MEMBER_PTR) -offsetof(STRUCT, MEMBER) ) )

static inline void * CONST_CAST(const void * cv)
{
    union {
        const void * cv ;
        void * v ;
    } u ;
    u.cv = cv ;

    return u.v ;
}

static inline const void * CPOINTER(uint32_t cv)
{
    union {
        uint32_t cv ;
        const void * v ;
    } u ;
    u.cv = cv ;

    return u.v ;
}

static inline void * POINTER(uint32_t cv)
{
    union {
        uint32_t cv ;
        void * v ;
    } u ;
    u.cv = cv ;

    return u.v ;
}

static inline uint32_t UINTEGER(const void * v)
{
    union {
        uint32_t cv ;
        const void * v ;
    } u ;
    u.v = v ;

    return u.cv ;
}


#else
#	warning utili.h incluso
#endif
