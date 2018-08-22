

#include "LyzkNmeaProtocol.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdarg.h"
#include "ctype.h"

#define NMEA_MAX_TEMP_STRING_LENGTH 50

#define NMEA_TOKS_COMPARE   (1)
#define NMEA_TOKS_PERCENT   (2)
#define NMEA_TOKS_WIDTH     (3)
#define NMEA_TOKS_TYPE      (4)

uint32_t LyzkStrToHex (char* str)
{
    /* The result converted from string */
    uint32_t uiResult = 0;
    
    while ('0' > *str || ('9' < *str && *str < 'A') 
        || ('F' < *str && *str < 'a') || *str > 'f' )
    {
        str++;
    }
    
    if (*str == '0' && (*(str + 1) == 'x' || * (str + 1) == 'X'))
    {
        str += 2;
    }
    
    while (('0' <= *str && *str <= '9') || ('a' <= *str && *str >= 'f')
        || ('A' <= *str && *str <= 'F'))
    {
        uiResult *= 16;
        
        if ('0' <= *str && *str <= '9')
        {
            uiResult += *str - '0';
        }
        else if ('a' <= *str && *str >= 'f')
        {
            uiResult += (*str - 'a') + 10;
        }
        else if ('A' <= *str && *str <= 'F')
        {
            uiResult += (*str - 'A') + 10;
        }
        
        str++;
    }
    
    return uiResult;
}

ErrorStatus LyzkNmeaCheckSum (const char* strMsg)
{
    ErrorStatus eResult = ERROR;
    char chSumCalc; /* Check sum calculated */
    char chSumGet;  /* Check sum get from the message to check */
    char strSumGet [3]; /*The string form of check sum get from the message to check */
    
    /* Find the header of '$' */
    while (*strMsg != '$')
    {
        strMsg++;
    }
    
    /* Calcuate check sum of the message to check */
    if (*strMsg == '$')
    {
        chSumCalc = 0;
        strMsg++;
        while (*strMsg != '*')
        {
            chSumCalc ^= *strMsg;
            strMsg++;
        }

        strSumGet [0]   = *(++strMsg);
        strSumGet [1]   = *(++strMsg);
        strSumGet [2]   = 0;
        chSumGet = (char) LyzkStrToHex (strSumGet);
        
        if (chSumCalc == chSumGet)
        {
            eResult = SUCCESS;
        }
    }
    
    return eResult;
}

int LyzkNmeaScanf (const char* strMsg, const int iMsgSize, const char* strFmt, ...)
{
    const char* pchMsgEnd   = strMsg + iMsgSize;
    int iArgCnt             = 0;        /* Arguments that converted */
    char strMsgTmp [NMEA_MAX_TEMP_STRING_LENGTH]     = {0};
    char strFmtTmp [NMEA_MAX_TEMP_STRING_LENGTH]     = {0};
    int i;
    int iWidth;
    void* pArgu;
    int iTokType            = NMEA_TOKS_COMPARE;
    
    /* Declare a variable that will be refer to each argument in turn.      *
     * Type va_list is defined in "stdarg.h" file.                          */
    va_list argList;
    
    /* Find the beginning character '$' */
    while (('$' != *strMsg) && (*strMsg) && (strMsg < pchMsgEnd))
    {
        strMsg++;
    }
    
    /* Cannot find the '$' character in strMsg or the first character of    *
     * strFmt is not '$'. That is, the first character of strMsg and strFmt *
     * must be '$', align strMsg and strFmt.                                */
    if (('$' != *strMsg) || ('$' != *strFmt))
    {
        goto FAIL;
    }
    
    /* Macro va_start initializes "argList" to point to the first unnamed   *
     * argument. It must be called once before "argList" is used.           */
    va_start (argList, strFmt);

    strMsg++;
    strFmt++;
    while (*strFmt)
    {
        switch (iTokType)
        {
        case NMEA_TOKS_COMPARE:
            if ('%' == *strFmt)
            {
                iTokType = NMEA_TOKS_PERCENT;
            }
            else if (*strFmt != *strMsg)
            {
                goto FAIL;
            }
            else
            {
                strMsg++;
            }            
            break;
            
        case NMEA_TOKS_PERCENT:
            iWidth          = 0;
            i               = 0;            
            iTokType        = NMEA_TOKS_WIDTH;
        case NMEA_TOKS_WIDTH:
            if (isdigit (*strFmt))
            {
                strFmtTmp [i] = *strFmt;
                i++;
                break;
            }
            
            iTokType        = NMEA_TOKS_TYPE;
            
            /* Read part of string to convert */
            strFmtTmp [i]   = '\0';
            
            /* The number of character to read is specified */
            if (i > 0)
            {
                iWidth = atoi (strFmtTmp);
                
                /* Read part of strMsg, the number of characters to read    *
                 * was specified by iWidth.                                 */
                memcpy (strMsgTmp, strMsg, iWidth);                
                strMsgTmp [iWidth] = '\0';
                strMsg += iWidth;
            }
            /* The number of characters to read is not specified */
            else
            {
                /* Read part of strMsg before the ',' character. */
                i = 0;
                while ((',' != *strMsg) && (strMsg < pchMsgEnd))
                {
                    strMsgTmp [i] = *strMsg;
                    strMsg++;
                    i++;
                }
                strMsgTmp [i] = '\0';
            }
        case NMEA_TOKS_TYPE:
            switch (*strFmt)
            {
            case 'c':
            case 'C':
                pArgu = (void*) va_arg (argList, char*);
                if (strlen (strMsgTmp))
                {
                    *((char*) pArgu) = strMsgTmp [0];
                }
                else
                {
                    *((char*) pArgu) = '\0';
                }
                iArgCnt++;
                break;
            
            case 's':
            case 'S':
                pArgu = (void*) va_arg (argList, char*);
                if (0 != (iWidth = strlen (strMsgTmp)))
                {
                    memcpy (pArgu, strMsgTmp, iWidth + 1);
                }
                else
                {
                    *((char*) pArgu) = '\0';
                }
                iArgCnt++;
                break;
            
            case 'd':
            case 'i':
                pArgu = (void*) va_arg (argList, int*);
                if (strlen (strMsgTmp))
                {
                    *((int*) pArgu) = atoi (strMsgTmp);
                }
                else
                {
                    *((int*) pArgu) = 0;
                }
                iArgCnt++;
                break;
            
            case 'u':
            case 'o':
            case 'x':
            case 'X':
                pArgu = (void*) va_arg (argList, uint32_t*);
                if (strlen (strMsgTmp))
                {
                    *((uint32_t*) pArgu) = atoi (strMsgTmp);
                }
                else
                {
                    *((uint32_t*) pArgu) = 0;
                }
                iArgCnt++;
                break;
            
            case 'f':
            case 'F':
            case 'e':
            case 'E':
            case 'g':
            case 'G':
                pArgu = (void*) va_arg (argList, double*);
                if (strlen (strMsgTmp))
                {
                    *((double*) pArgu) = atof (strMsgTmp);
                }
                else
                {
                    *((double*) pArgu) = 0.0;
                }
                iArgCnt++;
                break;
            
            default:
                goto FAIL;
            }
            
            iTokType = NMEA_TOKS_COMPARE;
            break;
        }
        
        strFmt++;
    }

FAIL:
    /* va_end does whatever clearup is necessary. It must be called before  *
     * the program returns.                                                 */
    va_end (argList);
    
    return iArgCnt;
}

ErrorStatus LyzkGetInfoFromNmeaRmcSentence (const char* strMsg, 
                                            const int iMsgSize, 
                                            LyzkNmeaRmcInfo* pstInfo)
{
    char chEorW;                /* East or West */
    char chEorWMagn;            /* East or West of magnetic variation */
    char chNorS;                /* North or South */
    char achMsgId [6] = {0};    /* Message ID */
    int  iDegree;
    double dMinute;
    
    ErrorStatus eResult = LyzkNmeaCheckSum (strMsg);
    
    if (eResult == SUCCESS)
    {
        /* Find the header of '$' */
        while (*strMsg != '$')
        {
            strMsg++;
        }
        /* --------------------------------------- RMC Sentence format ---------------------------------------- */
        /*                                          $                                                           *
         *                                          |Message Id                                                 *
         *                                          |  | Hour                                                   *
         *                                          |  |   |Minute                                              *
         *                                          |  |   |  |Second                                           *
         *                                          |  |   |  |  | MilliSecond                                  *
         *                                          |  |   |  |  |   |Data Valid                                *
         *                                          |  |   |  |  |   |  |Latitude                               *
         *                                          |  |   |  |  |   |  |  | NorS                               *
         *                                          |  |   |  |  |   |  |  |  |Longitude                        *
         *                                          |  |   |  |  |   |  |  |  |  | EorW                         *
         *                                          |  |   |  |  |   |  |  |  |  |  |Speed                      *
         *                                          |  |   |  |  |   |  |  |  |  |  |  |Course over ground      *
         *                                          |  |   |  |  |   |  |  |  |  |  |  |  |  Day                *
         *                                          |  |   |  |  |   |  |  |  |  |  |  |  |   |Month            *
         *                                          |  |   |  |  |   |  |  |  |  |  |  |  |   |  |Year          *
         *                                          |  |   |  |  |   |  |  |  |  |  |  |  |   |  |  |Magn Vari  *
         *                                          |  |   |  |  |   |  |  |  |  |  |  |  |   |  |  |  | EorW   *
         *                                          |  |   |  |  |   |  |  |  |  |  |  |  |   |  |  |  |  |PMode*
         *                                          |  |   |  |  |   |  |  |  |  |  |  |  |   |  |  |  |  |  |  */
        if (18 != LyzkNmeaScanf (strMsg, iMsgSize, "$%5s,%2d%2d%2d.%3d,%c,%f,%c,%f,%c,%f,%f,%2d%2d%2d,%f,%c,%c*",
                                                   achMsgId, 
                                                   &(pstInfo->m_stUtcTime.m_iHour), 
                                                   &(pstInfo->m_stUtcTime.m_iMinute),
                                                   &(pstInfo->m_stUtcTime.m_iSecond), 
                                                   &(pstInfo->m_stUtcTime.m_iMilliSecond),
                                                   &(pstInfo->m_chStatus),
                                                   &(pstInfo->m_dLatitude),
                                                   &chNorS,
                                                   &(pstInfo->m_dLongitude),
                                                   &chEorW,
                                                   &(pstInfo->m_dSpeed),
                                                   &(pstInfo->m_dCourse),
                                                   &(pstInfo->m_stUtcDate.m_iDay),
                                                   &(pstInfo->m_stUtcDate.m_iMonth),
                                                   &(pstInfo->m_stUtcDate.m_iYear),
                                                   &(pstInfo->m_dMagnVariation),
                                                   &chEorWMagn,
                                                   &(pstInfo->m_chMode)))
        {
            eResult = ERROR;
        }
        else
        {
            if (achMsgId[2] != 'R' || achMsgId[3] != 'M' || achMsgId[4] != 'C')
            {
                eResult = ERROR;
            }
            else
            {
                /* Original Latitude format is "ddmm.mmmm", then translate it *
                 * to value in degree.                                        */
                iDegree = (int) (pstInfo->m_dLatitude) / 100;
                dMinute = pstInfo->m_dLatitude - iDegree * 100;
                pstInfo->m_dLatitude = (double) iDegree + dMinute / 60.0;
                
                /* If chNorS == 'S', Latitude is negative.                      */                
                if (chNorS == 'S')
                {
                    pstInfo->m_dLatitude = - pstInfo->m_dLatitude;
                }
                /* If chNorS == 'N', Latitude is positive. Or error occurred.   */
                else if (chNorS != 'N')
                {
                    eResult = ERROR;
                    return eResult;
                }
                
                /* Original Longitude format is "dddmm.mmmm", then translate it *
                 * to value in degree.                                          */
                iDegree = (int) (pstInfo->m_dLongitude) / 100;
                dMinute = pstInfo->m_dLongitude - iDegree * 100;
                pstInfo->m_dLongitude = (double) iDegree + dMinute / 60.0;
                
                /* If chEorW == 'W', Longitude is negative.                     */
                if (chEorW == 'W')
                {
                    pstInfo->m_dLongitude = - pstInfo->m_dLongitude;
                }
                /* If chEorW == 'E', Longitude is positive. Or error occurred.  */
                else if (chEorW != 'E')
                {
                    eResult = ERROR;
                    return eResult;
                }
                
                /* If chEorWMagn == 'W', Magnetic variation is negative.        */
                if (chEorWMagn == 'W')
                {
                    pstInfo->m_dMagnVariation = - pstInfo->m_dMagnVariation;
                }
                /* If chEorWMagn == 'E', Magnetic variation is positive. Or error occurred.*/
                else if (chEorWMagn != 'E')
                {
                    eResult = ERROR;
                    return eResult;
                }
            }
        }
    }
    
    return eResult;
}

ErrorStatus LyzkGetInfoFromNmeaVtgSentence (const char* strMsg,
                                        const int iMsgSize,
                                        LyzkNmeaVtgInfo* pstInfo)
{
    char achMsgId [6] = {0};    /* Message ID */
    
	ErrorStatus eResult = LyzkNmeaCheckSum (strMsg);
    
    if (SUCCESS == eResult)
    {
        /* Find the header of '$' */
        while ('$' != *strMsg)
        {
            strMsg++;
        }
        
        /* ----------------------------- VTG Sentence Format ------------------------------ */
        /*                                         $                                        *
         *                                         |MsgID                                   *
         *                                         |  |COS(T)                               *
         *                                         |  |  |  COS(M)                          *
         *                                         |  |  |    |SpeedInKnots                 *
         *                                         |  |  |    |    |SpeedInKm/h             *
         *                                         |  |  |    |    |    |Positioning Mode   *
         *                                         |  |  |    |    |    |    |              */
        if (6 != LyzkNmeaScanf (strMsg, iMsgSize, "$%5s,%f,T,%f,M,%f,N,%f,K,%c*",
                                                  achMsgId,
                                                  &(pstInfo->m_dCourseTrue),
                                                  &(pstInfo->m_dCourseMagn),
                                                  &(pstInfo->m_dSpeedInKnots),
                                                  &(pstInfo->m_dSpeedInKmPerHour),
                                                  &(pstInfo->m_chMode)))
        {
            eResult = ERROR;
        }
        else
        {
            if (achMsgId [2] != 'V' || achMsgId [3] != 'T' || achMsgId [4] != 'G')
            {
                eResult = ERROR;
            }
        }
    }
	
	return eResult;
}

ErrorStatus LyzkGetInfoFromNmeaGgaSentence (const char* strMsg,
                                        const int iMsgSize,
                                        LyzkNmeaGgaInfo* pstInfo)
{
    char chEorW;                /* East or West */
    char chNorS;                /* North or South */
    char achMsgId [6] = {0};    /* Message ID */
    int  iDegree;
    double dMinute;
    
    ErrorStatus eResult = LyzkNmeaCheckSum (strMsg);
    
    if (SUCCESS == eResult)
    {
        /* Find the header of '$' */
        while ('$' != *strMsg)
        {
            strMsg++;
        }
        
        /* --------------------------------------- GGA Sentence Format ---------------------------------------- */
        /*                                          $                                                           *
         *                                          |MsgId                                                      *
         *                                          |  | Hour                                                   *
         *                                          |  |   |Minute                                              *
         *                                          |  |   |  |Second                                           *
         *                                          |  |   |  |  |MilliSecond                                   *
         *                                          |  |   |  |  |   |Latitude                                  *
         *                                          |  |   |  |  |   |  |NorS                                   *
         *                                          |  |   |  |  |   |  |  |Longitude                           *
         *                                          |  |   |  |  |   |  |  |  |EorW                             *
         *                                          |  |   |  |  |   |  |  |  |  |FixStatus                     *
         *                                          |  |   |  |  |   |  |  |  |  |  |NumOfSV                    *
         *                                          |  |   |  |  |   |  |  |  |  |  |  |HDOP                    *
         *                                          |  |   |  |  |   |  |  |  |  |  |  |  |Altitude             *
         *                                          |  |   |  |  |   |  |  |  |  |  |  |  |  |GeoidSeparation   *
         *                                          |  |   |  |  |   |  |  |  |  |  |  |  |  |    | DGPS Age    *
         *                                          |  |   |  |  |   |  |  |  |  |  |  |  |  |    |    |DGPS Station ID *
         *                                          |  |   |  |  |   |  |  |  |  |  |  |  |  |    |    |  |     */
        if (16 != LyzkNmeaScanf (strMsg, iMsgSize, "$%5s,%2d%2d%2d.%3d,%f,%c,%f,%c,%d,%d,%f,%f,M,%f,M,%f,%d*",
                                                   achMsgId,
                                                   &(pstInfo->m_stUtcTime.m_iHour),
                                                   &(pstInfo->m_stUtcTime.m_iMinute),
                                                   &(pstInfo->m_stUtcTime.m_iSecond),
                                                   &(pstInfo->m_stUtcTime.m_iMilliSecond),
                                                   &(pstInfo->m_dLatitude),
                                                   &chNorS,
                                                   &(pstInfo->m_dLongitude),
                                                   &chEorW,
                                                   &(pstInfo->m_iSig),
                                                   &(pstInfo->m_iNumOfSateUsed),
                                                   &(pstInfo->m_dHdop),
                                                   &(pstInfo->m_dAltitude),
                                                   &(pstInfo->m_dGeoSeparation),
                                                   &(pstInfo->m_dDgpsAge),
                                                   &(pstInfo->m_iDgpsStationId)))
        {
            eResult = ERROR;
        }
        else
        {
            if (achMsgId [2] != 'G' || achMsgId [3] != 'G' || achMsgId [4] != 'A')
            {
                eResult = ERROR;
            }
            else
            {
                /* Original Latitude format is "ddmm.mmmm", then translate it *
                 * to value in degree.                                        */
                iDegree = (int) (pstInfo->m_dLatitude) / 100;
                dMinute = pstInfo->m_dLatitude - iDegree * 100;
                pstInfo->m_dLatitude = (double) iDegree + dMinute / 60.0;
                
                /* If chNorS == 'S', Latitude is negative.                      */                
                if (chNorS == 'S')
                {
                    pstInfo->m_dLatitude = - pstInfo->m_dLatitude;
                }
                /* If chNorS == 'N', Latitude is positive. Or error occurred.   */
                else if (chNorS != 'N')
                {
                    eResult = ERROR;
                    return eResult;
                }
                
                /* Original Longitude format is "dddmm.mmmm", then translate it *
                 * to value in degree.                                          */
                iDegree = (int) (pstInfo->m_dLongitude) / 100;
                dMinute = pstInfo->m_dLongitude - iDegree * 100;
                pstInfo->m_dLongitude = (double) iDegree + dMinute / 60.0;
                
                /* If chEorW == 'W', Longitude is negative.                     */
                if (chEorW == 'W')
                {
                    pstInfo->m_dLongitude = - pstInfo->m_dLongitude;
                }
                /* If chEorW == 'E', Longitude is positive. Or error occurred.  */
                else if (chEorW != 'E')
                {
                    eResult = ERROR;
                    return eResult;
                }
            }
        }
    }
    
    return eResult;
}

ErrorStatus LyzkGetInfoFromNmeaGsaSentence (const char* strMsg,
                                        const int iMsgSize,
                                        LyzkNmeaGsaInfo* pstInfo)
{
    char achMsgId [6] = {0};    /* Message ID */
    
    ErrorStatus eResult = LyzkNmeaCheckSum (strMsg);
    
    if (SUCCESS == eResult)
    {
        /* Find the header of '$' */
        while ('$' != *strMsg)
        {
            strMsg++;
        }
        
        /* ------------------------------------- GSA Sentence Format -------------------------------------- */
        /*                                          $                                                       *
         *                                          |MsgId                                                  *
         *                                          |  |Mode                                                *
         *                                          |  |  |Fix Statuse                                      *
         *                                          |  |  |  |SatInCh1                                      *
         *                                          |  |  |  |  |SatInCh2                                   *
         *                                          |  |  |  |  |  |SatInCh3                                *
         *                                          |  |  |  |  |  |  |SatInCh4                             *
         *                                          |  |  |  |  |  |  |  |SatInCh5                          *
         *                                          |  |  |  |  |  |  |  |  |SatInCh6                       *
         *                                          |  |  |  |  |  |  |  |  |  |SatInCh7                    *
         *                                          |  |  |  |  |  |  |  |  |  |  |SatInCh8                 *
         *                                          |  |  |  |  |  |  |  |  |  |  |  |SatInCh9              *
         *                                          |  |  |  |  |  |  |  |  |  |  |  |  |SatInCh10          *
         *                                          |  |  |  |  |  |  |  |  |  |  |  |  |  |SatInCh11       *
         *                                          |  |  |  |  |  |  |  |  |  |  |  |  |  |  |SatInCh12    *
         *                                          |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |PDOP      *
         *                                          |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |HDOP   *
         *                                          |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | VDOP *
         *                                          |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  | */
        if (18 != LyzkNmeaScanf (strMsg, iMsgSize, "$%5s,%c,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%f,%f*",
                                                   achMsgId,
                                                   &(pstInfo->m_chFixMode),
                                                   &(pstInfo->m_iFixType),
                                                   &(pstInfo->m_aiStaUsed [0]),
                                                   &(pstInfo->m_aiStaUsed [1]),
                                                   &(pstInfo->m_aiStaUsed [2]),
                                                   &(pstInfo->m_aiStaUsed [3]),
                                                   &(pstInfo->m_aiStaUsed [4]),
                                                   &(pstInfo->m_aiStaUsed [5]),
                                                   &(pstInfo->m_aiStaUsed [6]),
                                                   &(pstInfo->m_aiStaUsed [7]),
                                                   &(pstInfo->m_aiStaUsed [8]),
                                                   &(pstInfo->m_aiStaUsed [9]),
                                                   &(pstInfo->m_aiStaUsed [10]),
                                                   &(pstInfo->m_aiStaUsed [11]),
                                                   &(pstInfo->m_dPdop),
                                                   &(pstInfo->m_dHdop),
                                                   &(pstInfo->m_dVdop)))
        {
            eResult = ERROR;
        }
        else
        {
            if ('G' != achMsgId [2] || 'S' != achMsgId [3] || 'A' != achMsgId [4])
            {
                eResult = ERROR;
            }
        }
    }
    
    return eResult;
}

ErrorStatus LyzkGetInfoFromNmeaGsvSentence (const char* strMsg,
                                        const int iMsgSize,
                                        LyzkNmeaGsvInfo* pstInfo)
{
    char achMsgId [6] = {0};    /* Message ID */
    ErrorStatus eResult = LyzkNmeaCheckSum (strMsg);
    
    if (SUCCESS == eResult)
    {
        /* Find the header of '$' */
        while ('$' != *strMsg)
        {
            strMsg++;
        }
        
        /* ----------------------------------------- GSV Sentence Format ------------------------------------------ */
        /*                                          $                                                               *
         *                                          |MsgId                                                          *
         *                                          |  |Msgs                                                        *
         *                                          |  |  | SN                                                      *
         *                                          |  |  |  |SatInView                                             *
         *                                          |  |  |  |  |SatId1                                             *
         *                                          |  |  |  |  |  |Elev1                                           *
         *                                          |  |  |  |  |  |  |Azi1                                         *
         *                                          |  |  |  |  |  |  |  |SNR1                                      *
         *                                          |  |  |  |  |  |  |  |  |SatId2                                 *
         *                                          |  |  |  |  |  |  |  |  |  |Elev2                               *
         *                                          |  |  |  |  |  |  |  |  |  |  |Azi2                             *
         *                                          |  |  |  |  |  |  |  |  |  |  |  |SNR2                          *
         *                                          |  |  |  |  |  |  |  |  |  |  |  |  |SatId3                     *
         *                                          |  |  |  |  |  |  |  |  |  |  |  |  |  |Elev3                   *
         *                                          |  |  |  |  |  |  |  |  |  |  |  |  |  |  |Azi3                 *
         *                                          |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |SNR3              *
         *                                          |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |SatId4         *
         *                                          |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |Elev4       *
         *                                          |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |Azi4     *
         *                                          |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |SNR4  *
         *                                          |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |  |   */
        if (20 != LyzkNmeaScanf (strMsg, iMsgSize, "$%5s,%d,%d,%d,%d,%f,%f,%f,%d,%f,%f,%f,%d,%f,%f,%f,%d,%f,%f,%f*",
                                                   achMsgId,
                                                   &(pstInfo->m_iNumOfMsg),
                                                   &(pstInfo->m_iSeqNum),
                                                   &(pstInfo->m_iSatInView),
                                                   &(pstInfo->m_stSatInfo [0].m_iId),
                                                   &(pstInfo->m_stSatInfo [0].m_dElevation),
                                                   &(pstInfo->m_stSatInfo [0].m_dAzimuth),
                                                   &(pstInfo->m_stSatInfo [0].m_dSnr),
                                                   &(pstInfo->m_stSatInfo [1].m_iId),
                                                   &(pstInfo->m_stSatInfo [1].m_dElevation),
                                                   &(pstInfo->m_stSatInfo [1].m_dAzimuth),
                                                   &(pstInfo->m_stSatInfo [1].m_dSnr),
                                                   &(pstInfo->m_stSatInfo [2].m_iId),
                                                   &(pstInfo->m_stSatInfo [2].m_dElevation),
                                                   &(pstInfo->m_stSatInfo [2].m_dAzimuth),
                                                   &(pstInfo->m_stSatInfo [2].m_dSnr),
                                                   &(pstInfo->m_stSatInfo [3].m_iId),
                                                   &(pstInfo->m_stSatInfo [3].m_dElevation),
                                                   &(pstInfo->m_stSatInfo [3].m_dAzimuth),
                                                   &(pstInfo->m_stSatInfo [3].m_dSnr)))
        {
            eResult = ERROR;
        }
        else
        {
            if ('G' != achMsgId [2] || 'S' != achMsgId [3] || 'V' != achMsgId [4])
            {
                eResult = ERROR;
            }
        }        
    }
    
    return eResult;
}

ErrorStatus LyzkGetInfoFromNmeaGllSentence (const char* strMsg,
                                        const int iMsgSize,
                                        LyzkNmeaGllInfo* pstInfo)
{
    char chEorW;
    char chNorS;
    char achMsgId [6] = {0};    /* Message ID */
    int iDegree;
    double dMinute;
    
    ErrorStatus eResult = LyzkNmeaCheckSum (strMsg);
    
    if (SUCCESS == eResult)
    {
        /* Find the header of '$' */
        while ('$' != *strMsg)
        {
            strMsg++;
        }
        
        if (11 != LyzkNmeaScanf (strMsg, iMsgSize, "$%5s,%f,%c,%f,%c,%2d%2d%2d.%3d,%c,%c*",
                                                   achMsgId,
                                                   &(pstInfo->m_dLatitude),
                                                   &chNorS,
                                                   &(pstInfo->m_dLongitude),
                                                   &chEorW,
                                                   &(pstInfo->m_stUtcTime.m_iHour),
                                                   &(pstInfo->m_stUtcTime.m_iMinute),
                                                   &(pstInfo->m_stUtcTime.m_iSecond),
                                                   &(pstInfo->m_stUtcTime.m_iMilliSecond),
                                                   &(pstInfo->m_chStatus),
                                                   &(pstInfo->m_chMode)))
        {
            eResult = ERROR;
        }
        else
        {
            if (achMsgId [2] != 'G' || achMsgId [3] != 'L' || achMsgId [4] != 'L')
            {
                eResult = ERROR;
            }
            else
            {
                /* Original Latitude format is "ddmm.mmmm", then translate it *
                 * to value in degree.                                        */
                iDegree = (int) (pstInfo->m_dLatitude) / 100;
                dMinute = pstInfo->m_dLatitude - iDegree * 100;
                pstInfo->m_dLatitude = (double) iDegree + dMinute / 60.0;
                
                /* If chNorS == 'S', Latitude is negative.                      */                
                if (chNorS == 'S')
                {
                    pstInfo->m_dLatitude = - pstInfo->m_dLatitude;
                }
                /* If chNorS == 'N', Latitude is positive. Or error occurred.   */
                else if (chNorS != 'N')
                {
                    eResult = ERROR;
                    return eResult;
                }
                
                /* Original Longitude format is "dddmm.mmmm", then translate it *
                 * to value in degree.                                          */
                iDegree = (int) (pstInfo->m_dLongitude) / 100;
                dMinute = pstInfo->m_dLongitude - iDegree * 100;
                pstInfo->m_dLongitude = (double) iDegree + dMinute / 60.0;
                
                /* If chEorW == 'W', Longitude is negative.                     */
                if (chEorW == 'W')
                {
                    pstInfo->m_dLongitude = - pstInfo->m_dLongitude;
                }
                /* If chEorW == 'E', Longitude is positive. Or error occurred.  */
                else if (chEorW != 'E')
                {
                    eResult = ERROR;
                    return eResult;
                }
            }
        }
    }
    
    return eResult;
}
