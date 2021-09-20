#define DBGP_FILE
#include "includimi.h"

extern void app(void) ;

int main(void)
{
    app() ;
}

void panic(void)
{
	while(1){}
}

void Error_Handler(void)
{
    panic() ;
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(
    uint8_t * file,
    uint32_t line)
{
    DBG_PRINTF("ASSERT_FAILED: %s %u\r\n", file, line) ;
    panic();
}

#endif /* USE_FULL_ASSERT */

