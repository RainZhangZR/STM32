
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "misc.h"
#include "LyzkUsart.h"
#include "stdio.h"
#include "stdarg.h"

void LyzkUsartInit (USART_TypeDef* pUSARTx, USART_InitTypeDef* pstInit)
{
    GPIO_InitTypeDef   stGpioInit;
    
    if (pUSARTx == USART1)
    {
        /* Initialize the clock of GPIOA and USART1 */
        RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1, ENABLE);
        
        /* Initialize GPIO of Rx and Tx pin */
        /* Rx */
        stGpioInit.GPIO_Pin     = GPIO_Pin_10;
        stGpioInit.GPIO_Mode    = GPIO_Mode_IN_FLOATING;
        stGpioInit.GPIO_Speed   = GPIO_Speed_50MHz;
        GPIO_Init (GPIOA, &stGpioInit);
        
        /* Tx */
        stGpioInit.GPIO_Pin     = GPIO_Pin_9;
        stGpioInit.GPIO_Mode    = GPIO_Mode_AF_PP;
        GPIO_Init (GPIOA, &stGpioInit);
    }    
    else if (pUSARTx == USART2)
    {
        /* Initialize the clock of GPIOA and USART2 */
        RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART2, ENABLE);
        
        /* Initialize GPIO of Rx and Tx pin */
        /* Rx */
        stGpioInit.GPIO_Pin     = GPIO_Pin_3;
        stGpioInit.GPIO_Mode    = GPIO_Mode_IN_FLOATING;
        stGpioInit.GPIO_Speed   = GPIO_Speed_50MHz;
        GPIO_Init (GPIOA, &stGpioInit);
        
        /* Tx */
        stGpioInit.GPIO_Pin     = GPIO_Pin_2;
        stGpioInit.GPIO_Mode    = GPIO_Mode_AF_PP;
        GPIO_Init (GPIOA, &stGpioInit);
    }
    else if (pUSARTx == USART3)
    {
        /* Initialize the clock of GPIOB and USART3 */
        RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOB, ENABLE);
        RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART3, ENABLE);
        
        /* Initialize GPIO of Rx and Tx pin */
        /* Rx */
        stGpioInit.GPIO_Pin     = GPIO_Pin_11;
        stGpioInit.GPIO_Mode    = GPIO_Mode_IN_FLOATING;
        stGpioInit.GPIO_Speed   = GPIO_Speed_50MHz;
        GPIO_Init (GPIOB, &stGpioInit);
        
        /* Tx */
        stGpioInit.GPIO_Pin     = GPIO_Pin_10;
        stGpioInit.GPIO_Mode    = GPIO_Mode_AF_PP;
        GPIO_Init (GPIOB, &stGpioInit);
    }
    else if (pUSARTx == UART4)
    {
        /* Initialize the clock of GPIOC and UART4 */
        RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOC, ENABLE);
        RCC_APB1PeriphClockCmd (RCC_APB1Periph_UART4, ENABLE);
        
        /* Initialize GPIO of Rx and Tx pin */
        /* Rx */
        stGpioInit.GPIO_Pin     = GPIO_Pin_11;
        stGpioInit.GPIO_Mode    = GPIO_Mode_IN_FLOATING;
        stGpioInit.GPIO_Speed   = GPIO_Speed_50MHz;
        GPIO_Init (GPIOC, &stGpioInit);
        
        /* Tx */
        stGpioInit.GPIO_Pin     = GPIO_Pin_10;
        stGpioInit.GPIO_Mode    = GPIO_Mode_AF_PP;
        GPIO_Init (GPIOC, &stGpioInit);
    }
    else if (pUSARTx == UART5)
    {
        /* Initialize the clock of GPIOC, GPIOD and UART4 */
        RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE);
        RCC_APB1PeriphClockCmd (RCC_APB1Periph_UART5, ENABLE);
        
        /* Initialize GPIO of Rx and Tx pin */
        /* Rx */
        stGpioInit.GPIO_Pin     = GPIO_Pin_2;
        stGpioInit.GPIO_Mode    = GPIO_Mode_IN_FLOATING;
        stGpioInit.GPIO_Speed   = GPIO_Speed_50MHz;
        GPIO_Init (GPIOD, &stGpioInit);
        
        /* Tx */
        stGpioInit.GPIO_Pin     = GPIO_Pin_12;
        stGpioInit.GPIO_Mode    = GPIO_Mode_AF_PP;
        GPIO_Init (GPIOD, &stGpioInit);
    }
    
    /* Initialize USART */
    USART_Init (pUSARTx, pstInit);
    
    /* Enable USART */
    USART_Cmd (pUSARTx, ENABLE);
}

void LyzkUsartItCfg (USART_TypeDef* pUSARTx, const uint8_t usIt, 
                     const uint32_t wPrioGrp, const uint8_t byPrePrio, 
                     const uint8_t bySubPrio)
{
    NVIC_InitTypeDef stInit;
    
    /* Configurate the priority grouping. */
    NVIC_PriorityGroupConfig (wPrioGrp);
    
    /* Initialize NVIC. */
    if (pUSARTx == USART1)
    {
        stInit.NVIC_IRQChannel  = USART1_IRQn;
    }
    else if (pUSARTx == USART2)
    {
        stInit.NVIC_IRQChannel  = USART2_IRQn;
    }
    else if (pUSARTx == USART3)
    {
        stInit.NVIC_IRQChannel  = USART3_IRQn;
    }
    else if (pUSARTx == UART4)
    {
        stInit.NVIC_IRQChannel  = UART4_IRQn;
    }
    else if (pUSARTx == UART5)
    {
        stInit.NVIC_IRQChannel  = UART5_IRQn;
    }
    else
    {
        return;
    }
    
    stInit.NVIC_IRQChannelCmd   = ENABLE;
    stInit.NVIC_IRQChannelPreemptionPriority = byPrePrio;
    stInit.NVIC_IRQChannelSubPriority   = bySubPrio;    
    NVIC_Init (&stInit);
    
    /* Enable the interrupt */
    if (usIt & LYZK_USART_IT_TXE)
    {
        USART_ITConfig (pUSARTx, USART_IT_TXE, ENABLE);
    }
    if (usIt & LYZK_USART_IT_TC)
    {
        USART_ITConfig (pUSARTx, USART_IT_TC, ENABLE);
    }
    if (usIt & LYZK_USART_IT_RXNE)
    {
        USART_ITConfig (pUSARTx, USART_IT_RXNE, ENABLE);
    }
    if (usIt & LYZK_USART_IT_IDLE)
    {
        USART_ITConfig (pUSARTx, USART_IT_IDLE, ENABLE);
    }
    if (usIt & LYZK_USART_IT_CTS)
    {
        USART_ITConfig (pUSARTx, USART_IT_CTS, ENABLE);
    }
    if (usIt & LYZK_USART_IT_LBD)
    {
        USART_ITConfig (pUSARTx, USART_IT_LBD, ENABLE);
    }
    if (usIt & LYZK_USART_IT_PE)
    {
        USART_ITConfig (pUSARTx, USART_IT_PE, ENABLE);
    }
    if (usIt & LYZK_USART_IT_ERR)
    {
        USART_ITConfig (pUSARTx, USART_IT_ERR, ENABLE);
    }
}

void LyzkUsartPutChar (USART_TypeDef* pUSARTx, const uint8_t ch)
{
    USART_SendData (pUSARTx, (uint16_t) ch);
    
    /* Waiting till the transmission completed                              *
     * Do not use the flag of TC, or your first character will be lost when *
     * you first output your characters. The flag of TC can be used when    *
     * sending a package of data. After sending a package, you can use the  *
     * flag of TC to decide whether the sending-package operation is completed.*/
    while (USART_GetFlagStatus (pUSARTx, USART_FLAG_TXE) == RESET);
}

uint8_t LyzkUsartGetChar (USART_TypeDef* pUSARTx)
{
    /* Waiting till the Received Data Register is not empty */
    while (USART_GetFlagStatus (pUSARTx, USART_FLAG_RXNE) == RESET);

    return ((uint8_t) USART_ReceiveData (pUSARTx));
}

int LyzkUsartSendPackage (USART_TypeDef* pUSARTx, uint8_t* pbySend, const int iSize)
{
    int iCnt = 0;
    
    while (iCnt < iSize)
    {
        LyzkUsartPutChar (pUSARTx, *pbySend);
        iCnt++;
        pbySend++;
    }
    
    /* Wait until all the data of the package have been sent */
    while (USART_GetFlagStatus (pUSARTx, USART_FLAG_TC) == RESET);
    
    return iCnt;
}

int LyzkUsartFprint (USART_TypeDef* pUSARTx, char* strFmt, ...)
{
    #define print_ch(ch) LyzkUsartPutChar (pUSARTx, ch);
    #define MAX_BUFFER_SIZE 509
    
    /* The number of characters printed to pUSARTx port */
    int iCnt = 0;
    
    /* The result string converted using vsnprintf () function in standard  *
     * library "stdio.h". */
    char    strRslt [MAX_BUFFER_SIZE] = {0};
    int     i;

    /* Declare a variable that will be refer to each argument in turn.      *
     * Type va_list is defined in "stdarg.h" file.                          */
    va_list argList;
    
    /* Macro va_start initializes "varList" to point to the first unnamed   *
     * argument. It must be called once before "varList" is used.           */
    va_start (argList, strFmt);
    
    iCnt = vsnprintf (strRslt, MAX_BUFFER_SIZE, strFmt, argList);
    
    for (i = 0; i < iCnt; i++)
    {
        print_ch (strRslt [i]);
    }
 
    /* va_end does whatever clearup is necessary. It must be called before  *
     * the program returns.                                                 */
    va_end (argList);
    
    /* Wait until all the characters of the converted string have been sent */
    while (USART_GetFlagStatus (pUSARTx, USART_FLAG_TC) == RESET);
    
    return iCnt;
}
