
#ifndef __LYZK_TIME_H__
#define __LYZK_TIME_H__

typedef struct
{
    int m_iHour;            /* Hours in a day, 0~23 */
    int m_iMinute;          /* Minutes in an hour, 0~59 */
    int m_iSecond;          /* Seconds in a minute, 0~59 */
    int m_iMilliSecond;     /* Milliseconds in a second, 0~999 */
} LyzkTime;

typedef struct
{
    int m_iYear;            /* Year of the calendar, such as 2018 */
    int m_iMonth;           /* Month, 1~12 */
    int m_iDay;             /* Day, 1~31 */
} LyzkDate;

void LyzkTimeToStr (char* strTime, int iSize, const LyzkTime* pstTime);
void LyzkDateToStr (char* strDate, int iSize, const LyzkDate* pstDate);

#endif /* __LYZK_TIME_H__ */
