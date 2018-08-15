
#include "LyzkTime.h"
#include "stdio.h"

void LyzkTimeToStr (char* strTime, int iSize, const LyzkTime* pstTime)
{
    snprintf (strTime, iSize, "%02d:%02d:%02d:%03d", pstTime->m_iHour,
                                                     pstTime->m_iMinute,
                                                     pstTime->m_iSecond,
                                                     pstTime->m_iMilliSecond);
}

void LyzkDateToStr (char* strDate, int iSize, const LyzkDate* pstDate)
{
    snprintf (strDate, iSize, "%04d-%02d-%02d", pstDate->m_iYear,
                                                pstDate->m_iMonth,
                                                pstDate->m_iDay);
}
