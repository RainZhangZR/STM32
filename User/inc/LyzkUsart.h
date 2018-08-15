/******************************************************************************
  Copyright (C), Luoyang Institute of Information Technology Industies (LYZK)
                 and Luoyang Sylincom Smart Equipment Co. Ltd. 2018
  File name: LyzkUsart.h
  Author: 张冉
  Version: 1.0.0
  Date: 
  Description: 该程序库封装了STM32的USART外设固件库。
  Others: 如果要从USART端口接收数据，需要在用户应用程序中重写USARTx_IRQHandler()
          函数。
  Function List: 
    1. LyzkUsartInit: USART外设的初始化。
    2. LyzkUsartItCfg：配置中断，使用自定义的LYZK_USART_IT_CTS等宏。
  History: (修改的历史记录)
    1.  Date:
        Author:
        Modification: 
 ******************************************************************************/

#ifndef __LYZK_USART_H__
#define __LYZK_USART_H__

#include "stm32f10x_usart.h"

/* Define the interrupts used. */
#define LYZK_USART_IT_CTS       ((uint8_t) 0x0001)
#define LYZK_USART_IT_LBD       ((uint8_t) 0x0002)
#define LYZK_USART_IT_TXE       ((uint8_t) 0x0004)
#define LYZK_USART_IT_TC        ((uint8_t) 0x0008)
#define LYZK_USART_IT_RXNE      ((uint8_t) 0x0010)
#define LYZK_USART_IT_IDLE      ((uint8_t) 0x0020)
#define LYZK_USART_IT_PE        ((uint8_t) 0x0040)
#define LYZK_USART_IT_ERR       ((uint8_t) 0x0080)

#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

void LyzkUsartInit (USART_TypeDef* pUSARTx, USART_InitTypeDef* pstInitStruct);
void LyzkUsartItCfg (USART_TypeDef* pUSARTx, const uint8_t usIt, 
                     const uint32_t wPrioGrp, const uint8_t byPrePrio, 
                     const uint8_t bySubPrio);

void LyzkUsartPutChar (USART_TypeDef* pUSARTx, const uint8_t ch);
uint8_t LyzkUsartGetChar (USART_TypeDef* pUSARTx);

int LyzkUsartSendPackage (USART_TypeDef* pUSARTx, uint8_t* pbySend, const int iSize);
int LyzkUsartFprint (USART_TypeDef* pUSARTx, char* strFmt, ...);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __LYZK_USART_H__ */
