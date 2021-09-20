#ifndef BSP_TRANS_H_
#define BSP_TRANS_H_

#include <stdbool.h>

typedef void (*PF_TRANS)(bool);

/*
 * Le scelte dei canali non sono equivalenti, infatti non esiste un TJA1059TK
 * dedicato per i pin 1-9 e 6-14
 */

// TJA1059TK stb

void TRANS_1_9(bool abil) ;
void TRANS_6_14(bool abil) ;
void TRANS_3_11_2(bool abil) ;
void TRANS_3_11_3(bool abil) ;
void TRANS_12_13_2(bool abil) ;
void TRANS_12_13_3(bool abil) ;

#else
#   warning trans.h incluso
#endif
