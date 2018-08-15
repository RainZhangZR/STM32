#include "stm32f10x.h"
#include "LyzkUsart.h"
#include "LyzkNmeaProtocol.h"
#include "stdlib.h"

//#define printf(str) LyzkUsartFprint (USART1, str, __VA_ARG_)

int main (void)
{
    ErrorStatus eResult;
    LyzkNmeaRmcInfo stNmeaRmcInfo;
    
    USART_InitTypeDef stUsartInit;
    stUsartInit.USART_BaudRate          = 115200;
    stUsartInit.USART_HardwareFlowControl   = USART_HardwareFlowControl_None;
    stUsartInit.USART_Mode              = USART_Mode_Rx | USART_Mode_Tx;
    stUsartInit.USART_Parity            = USART_Parity_No;
    stUsartInit.USART_StopBits          = USART_StopBits_1;
    stUsartInit.USART_WordLength        = USART_WordLength_8b;
    
    LyzkUsartInit (USART1, &stUsartInit);
    
    LyzkUsartItCfg (USART1, LYZK_USART_IT_IDLE | LYZK_USART_IT_RXNE, 0 ,0 ,0);
    
    //LyzkUsartSendPackage (USART1, "0123456789", 10);
    LyzkUsartFprint (USART1, "USART_BaudRate = %-15.5f\r\n", 123456789.123456789);
    LyzkUsartFprint (USART1, "USART_BaudRate = %ld\r\n", 123456789);
    LyzkUsartFprint (USART1, "Print string %s\r\n", "123456789");
   
    //eResult = LyzkNmeaCheckSum ("$GNRMC,235954.794,V,,,,,0.00,0.00,050180,,,N*59");
    eResult = LyzkGetInfoFromNmeaRmcSentence ("$GPRMC,015606.000,A,3150.7584,N,11712.0491,E,0.00,231.36,280715,,,A*67", 
                                              &stNmeaRmcInfo);
//    
//    LyzkUsartFprint (USART1, "Result is: %d\r\n", eResult);
//    LyzkUsartFprint (USART1, "Longitude = %g\r\nLatitude = %g\r\nSpeed = %g\r\nCourse = %g\r\nMagnVari = %g\r\nMode = %c\r\n",
//                            dLongitude, dLatitude, dSpeed, dCourse, dMagnVari, chMode);
//    LyzkTimeToStr (strTime, 13, &stTime);	
//    LyzkDateToStr (strDate, 11, &stDate);
//    LyzkUsartFprint (USART1, "Time = %s %s\r\n", strDate, strTime);
//    
//    LyzkUsartFprint (USART1, "\r\n\r\n");
//							
//	eResult = LyzkGetInfoFromVtgSentence ("$BDVTG,229.71,T,,M,0.37,N,0.68,K,A*29", &dCourse, &dCourseMagn, &dSpeed, &dSpeedInKmPerHour, &chMode);
//	LyzkUsartFprint (USART1, "VTG result is: %d\r\n", eResult);
//	LyzkUsartFprint (USART1, "Course (True) = %g\r\nCourse (magnetic) = %g\r\nSpeed in knots = %g\r\nSpeed in km/h = %g\r\nMode = %c\r\n",
//							 dCourse, dCourseMagn, dSpeed, dSpeedInKmPerHour, chMode);
//    
//    LyzkUsartFprint (USART1, "\r\n\r\n");
//    
//    eResult = LyzkGetInfoFromGgaSentence ("$GPGGA,015606.000,3150.7584,N,11712.0491,E,1,5,2.28,265.0,M,0.0,M,,*65", &stTime, 
//                                          &dLatitude, &dLongitude, &chMode, &iNumOfSatellitesUsed, &dHdop, &dAltitude, &dGeoidSeparation,
//                                          &iAgeOfDgps, &iStationIdOfDgps);
//    LyzkUsartFprint (USART1, "GGA result is: %d\r\n", eResult);
//    LyzkUsartFprint (USART1, "Latitude = %g\r\nLongitude = %g\r\nMode = %c\r\nNumber of Satellites Used = %d\r\nHDOP = %g\r\nAltitude = %g\r\n", 
//                            dLatitude, dLongitude, chMode, iNumOfSatellitesUsed, dHdop, dAltitude);
//    LyzkUsartFprint (USART1, "Geoid Separation = %g\r\nAge Of DGPS = %d\r\nStation ID Of DGPS = %d\r\n", dGeoidSeparation, iAgeOfDgps, iStationIdOfDgps);
//    LyzkUsartFprint (USART1, "Time = %02d:%02d:%02d.%03d\r\n", stTime.m_iHour, stTime.m_iMinute, stTime.m_iSecond, stTime.m_iMilliSecond);
//    
//    LyzkUsartFprint (USART1, "\r\n\r\n");
//    
//    eResult = LyzkGetInfoFromGllSentence ("$BDGLL,3150.7813,N,11711.9212,E,020547.000,A,A*49", 
//                                          &stTime, &dLatitude, &dLongitude, &chMode);
//    LyzkUsartFprint (USART1, "GLL result is %d\r\n", eResult);
//    LyzkUsartFprint (USART1, "Latitude = %g\r\nLongitude = %g\r\nMode = %c\r\n", dLatitude, dLongitude, chMode);
//    LyzkUsartFprint (USART1, "Time = %02d:%02d:%02d.%03d\r\n", stTime.m_iHour, stTime.m_iMinute, stTime.m_iSecond, stTime.m_iMilliSecond);

    return 0;
}
