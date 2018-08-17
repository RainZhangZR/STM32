#include "stm32f10x.h"
#include "LyzkUsart.h"
#include "LyzkNmeaProtocol.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

//#define printf(str) LyzkUsartFprint (USART1, str, __VA_ARG_)

#define print_ch(ch) LyzkUsartPutChar (USART1, ch);
//#define printf(strFmt,...) LyzkUsartPrintf (USART1, strFmt, ...)

uint8_t pbyGpsBuff [2000];
int iCntRecv = 0;
bool bReceived = false;

int main (void)
{
    ErrorStatus eResult;
    LyzkNmeaRmcInfo stNmeaRmcInfo;
    LyzkNmeaVtgInfo stNmeaVtgInfo;
    LyzkNmeaGgaInfo stNmeaGgaInfo;
    LyzkNmeaGsaInfo stNmeaGsaInfo;
    LyzkNmeaGsvInfo stNmeaGsvInfo;
    //char strSentence [200] = "$GPRMC,015606.000,A,3150.7584,N,11712.0491,E,0.00,231.36,280715,,,A*67";
    //char strSentence [200] = "$BDVTG,229.71,T,,M,0.37,N,0.68,K,A*29";
    //char strSentence [200] = "$GPGGA,015606.000,3150.7584,N,11712.0491,E,1,5,2.28,265.0,M,0.0,M,,*65";
    //char strSentence [200] = "$GPGSA,A,3,03,17,11,23,193,,,,,,,,3.72,2.85,2.39*3C";
    char strSentence [200] = "$GPGSV,3,1,11,193,69,099,30,17,62,354,36,06,47,272,,03,40,054,30*4E";
    
    USART_InitTypeDef stUsartInit;
    stUsartInit.USART_BaudRate          = 115200;
    stUsartInit.USART_HardwareFlowControl   = USART_HardwareFlowControl_None;
    stUsartInit.USART_Mode              = USART_Mode_Rx | USART_Mode_Tx;
    stUsartInit.USART_Parity            = USART_Parity_No;
    stUsartInit.USART_StopBits          = USART_StopBits_1;
    stUsartInit.USART_WordLength        = USART_WordLength_8b;
    
    LyzkUsartInit (USART1, &stUsartInit);    
    LyzkUsartItCfg (USART1, LYZK_USART_IT_IDLE | LYZK_USART_IT_RXNE, 0 ,0 ,0);
    
    stUsartInit.USART_BaudRate          = 9600;
    LyzkUsartInit (USART2, &stUsartInit);
    LyzkUsartItCfg (USART2, LYZK_USART_IT_IDLE | LYZK_USART_IT_RXNE, 0, 0 ,0);
    
    //LyzkUsartSendPackage (USART1, "0123456789", 10);
//    LyzkUsartFprint (USART1, "USART_BaudRate = %-15.5f\r\n", 123456789.123456789);
//    LyzkUsartFprint (USART1, "USART_BaudRate = %ld\r\n", 123456789);
//    LyzkUsartFprint (USART1, "Print string %s\r\n", "123456789");
   
    //eResult = LyzkNmeaCheckSum ("$GNRMC,235954.794,V,,,,,0.00,0.00,050180,,,N*59");
    //eResult = LyzkGetInfoFromNmeaRmcSentence (strSentence, strlen (strSentence), &stNmeaRmcInfo);
    //eResult = LyzkGetInfoFromNmeaVtgSentence (strSentence, strlen (strSentence), &stNmeaVtgInfo);
    //eResult = LyzkGetInfoFromNmeaGgaSentence (strSentence, strlen (strSentence), &stNmeaGgaInfo);
    //eResult = LyzkGetInfoFromNmeaGsaSentence (strSentence, strlen (strSentence), &stNmeaGsaInfo);
    eResult = LyzkGetInfoFromNmeaGsvSentence (strSentence, strlen (strSentence), &stNmeaGsvInfo);
    
    while (1)
    {
        if (bReceived)
        {
            LyzkUsartSendPackage (USART1, pbyGpsBuff, iCntRecv);
            
            iCntRecv = 0;
            bReceived = false;
            
            LyzkUsartPrintf (USART1, "\r\n\r\n");
        }
    }

    return 0;
}

void USART1_IRQHandler (void)
{
    uint8_t chRecv;

    if (USART_GetITStatus (USART1, USART_IT_RXNE) != RESET)
    {
        chRecv = LyzkUsartGetChar (USART1);
        print_ch (chRecv);
    }
}

void USART2_IRQHandler (void)
{
    uint8_t chRecv;
    
    if (USART_GetITStatus (USART2, USART_IT_RXNE) != RESET)
    {
        pbyGpsBuff [iCntRecv++] = LyzkUsartGetChar (USART2);
    }
    
    if (USART_GetITStatus (USART2, USART_IT_IDLE) != RESET)
    {
        /* It is indicated that the frame transmitting is ended. */
		/* Clear the interrupt of IDLE. */
		uint16_t clear = clear;
		clear = USART2->SR;
		clear = USART2->DR;
        
        bReceived = true;
    }
}
