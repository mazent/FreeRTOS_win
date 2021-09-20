# FreeRTOS_win

Esempio d'uso del port di FreeRTOS per windows

## Info

Creare un progetto C/C++ per mingw

Aggiungere gli include

Aggiungere le librerie winmm e ws2_32

Personalizzare la cartella `cod`

Aggiungere in `FreeRTOS/port` il porting per la scheda e in `bsp` la
relativa bsp

## Cartelle

### bsp

Contiene gli include della bsp

La sottocartella `win` contiene l'implementazione per windows

### cfg

Impostazioni

### cmsis.rtos

Interfaccia omonima

### FreeRTOS

Versione: FreeRTOSv202107.00

### cod

Codice
