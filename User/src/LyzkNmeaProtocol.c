

#include "LyzkNmeaProtocol.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"

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

int LyzkNmeaScanf (const char* strMsg, int iMsgSize, const char* strFmt, ...)
{
}

ErrorStatus LyzkGetInfoFromNmeaRmcSentence (const char* strMsg, LyzkNmeaRmcInfo* pstInfo)
{
    char chEorW;        /* East or West */
    char chEorWMagn;    /* East or West of magnetic variation */
    char chNorS;        /* North or South */
    char pchMsgId [6];  /* Message ID */
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
        /* ------------------------------------- RMC Sentence format -------------------------------------- */
        /*                         $                                                                        *
         *                         |Message Id                                                              *
         *                         |  |  Hour                                                               *
         *                         |  |    | Minute                                                         *
         *                         |  |    |   |Second                                                      *
         *                         |  |    |   |   | MilliSecond                                            *
         *                         |  |    |   |   |    |Data Valid                                         *
         *                         |  |    |   |   |    |  |Latitude                                        *
         *                         |  |    |   |   |    |  |   | NorS                                       *
         *                         |  |    |   |   |    |  |   |  |Longitude                                *
         *                         |  |    |   |   |    |  |   |  |   | EorW                                *
         *                         |  |    |   |   |    |  |   |  |   |  | Speed                            *
         *                         |  |    |   |   |    |  |   |  |   |  |   | Course over ground           *
         *                         |  |    |   |   |    |  |   |  |   |  |   |   |   Day                    *
         *                         |  |    |   |   |    |  |   |  |   |  |   |   |    | Month               *
         *                         |  |    |   |   |    |  |   |  |   |  |   |   |    |   | Year            *
         *                         |  |    |   |   |    |  |   |  |   |  |   |   |    |   |   |Magn Vari    *
         *                         |  |    |   |   |    |  |   |  |   |  |   |   |    |   |   |   | EorW    *
         *                         |  |    |   |   |    |  |   |  |   |  |   |   |    |   |   |   |  |PMode *
         *                         |  |    |   |   |    |  |   |  |   |  |   |   |    |   |   |   |  |  |   */
        if (17 != sscanf (strMsg, "$%5s,%02d%02d%02d.%03d,%c,%lf,%c,%lf,%c,%lf,%lf,%02d%02d%02d,%lf,%c,%c*",
                                  pchMsgId, 
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
            if (pchMsgId[2] != 'R' || pchMsgId[3] != 'M' || pchMsgId[4] != 'C')
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

ErrorStatus LyzkGetInfoFromVtgSentence (char* strMsg, double* pdCourse, double* pdCourseMagn,
										double* pdSpeedInKnots, double* pdSpeedInKmPerHour,
										char* pchMode)
{
	char strTmp [11] = {0};
	int i;
	
	ErrorStatus eResult = LyzkNmeaCheckSum (strMsg);
	
	if (eResult == SUCCESS)
	{
		/* Find the header of '$' */
        while (*strMsg != '$')
        {
            strMsg++;
        }
        
        /* Whether is VTG sentence */
        if (*strMsg == '$' && *(strMsg + 3) == 'V' && *(strMsg + 4) == 'T'
         && *(strMsg + 5) == 'G')
        {
            strMsg += 7;
			/* Course over ground (T) in degree */
			i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                i++;
                strMsg++;
            }
            strTmp [i] = 0;
			
			*pdCourse = atof (strTmp);
			
			/* 'T' */
			strMsg++;
			i = 0;
			while (*strMsg != ',')
			{
				strTmp [i] = *strMsg;
				i++;
				strMsg++;
			}
			
			if (strTmp [0] != 'T')
			{
				eResult = ERROR;
				return eResult;
			}
			
			/* Course over ground (magnetic) in degree */
			strMsg++;
			i = 0;
			while (*strMsg != ',')
			{
				strTmp [i] = *strMsg;
				i++;
				strMsg++;
			}
			strTmp [i] = 0;
			
			*pdCourseMagn = atof (strTmp);
			
			/* 'M' */
			strMsg++;
			i = 0;
			while (*strMsg != ',')
			{
				strTmp [i] = *strMsg;
				i++;
				strMsg++;
			}
			
			if (strTmp [0] != 'M')
			{
				eResult = ERROR;
				return eResult;
			}
			
			/* Speed over ground in knots */
			strMsg++;
			i = 0;
			while (*strMsg != ',')
			{
				strTmp [i] = *strMsg;
				i++;
				strMsg++;
			}
			strTmp [i] = 0;
			
			*pdSpeedInKnots = atof (strTmp);
			
			/* 'N' */
			strMsg++;
			i = 0;
			while (*strMsg != ',')
			{
				strTmp [i] = *strMsg;
				i++;
				strMsg++;
			}
			
			if (strTmp [0] != 'N')
			{
				eResult = ERROR;
				return eResult;
			}
			
			/* Speed over ground in km/h */
			strMsg++;
			i = 0;
			while (*strMsg != ',')
			{
				strTmp [i] = *strMsg;
				i++;
				strMsg++;
			}
			strTmp [i] = 0;
			
			*pdSpeedInKmPerHour = atof (strTmp);
			
			/* 'K' */
			strMsg++;
			i = 0;
			while (*strMsg != ',')
			{
				strTmp [i] = *strMsg;
				i++;
				strMsg++;
			}
			
			if (strTmp [0] != 'K')
			{
				eResult = ERROR;
				return eResult;
			}
			
			/* Position Mode, 'N' - No fix, 'A' - Autonomous GNSS fix,  *
             * 'D' - Differential GNSS fix                              */
            strMsg++;
            *pchMode = *strMsg;
		}
        else
        {
            eResult = ERROR;
        }
	}
	
	return eResult;
}

ErrorStatus LyzkGetInfoFromGgaSentence (char* strMsg, LyzkTime* pstTime,
                                        double* pdLatitude, double* pdLongitude,
                                        char* pchMode, int* piNumOfSatellitesUsed, 
                                        double* pdHdop, double* pdAltitude, 
                                        double* pdGeoidSeparation, int* piAgeOfDgps,
                                        int* piStationIdOfDgps)
{
    char strTmp [11] = {0};
    int i;
    
    /* Decide whether the check sum is correct. */
    ErrorStatus eResult = LyzkNmeaCheckSum (strMsg);
    
    if (eResult == SUCCESS)
    {
        /* Find the header of '$' */
        while (*strMsg != '$')
        {
            strMsg++;
        }
        
        /* Whether is GGA sentence */
        if (*strMsg == '$' && *(strMsg + 3) == 'G' && *(strMsg + 4) == 'G'
         && *(strMsg + 5) == 'A')
        {
            strMsg += 7;
            /* UTC Time, the format is "hhmmss.sss" */
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                i++;
                strMsg++;
            }
            
            pstTime->m_iHour    = (strTmp [0] - '0') * 10 + (strTmp [1] - '0');
            pstTime->m_iMinute  = (strTmp [2] - '0') * 10 + (strTmp [3] - '0');
            pstTime->m_iSecond  = (strTmp [4] - '0') * 10 + (strTmp [5] - '0');
            pstTime->m_iMilliSecond   = (strTmp [7] - '0') * 100
                                      + (strTmp [8] - '0') * 10
                                      + (strTmp [9] - '0');
            
            /* Latitude, the format is "ddmm.mmmm" */
            strMsg++;
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                i++;
                strMsg++;
            }
            strTmp [i] = 0;
            
            *pdLatitude = atof (strTmp) / 100.0;
            
            /* Which hemisphere, 'N' - North (+), 'S' - South (-)  */
            strMsg++;
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                i++;
                strMsg++;
            }
            
            if (strTmp [0] == 'S')
            {
                *pdLatitude = - *pdLatitude;
            }
            
            /* Longitude, the format is "dddmm.mmmm" */
            strMsg++;
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                i++;
                strMsg++;
            }
            strTmp [i] = 0;
            
            *pdLongitude = atof (strTmp) / 100.0;
            
            /* East or west of longitude, 'E' - East (+), 'W' - West (-) */
            strMsg++;
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                i++;
                strMsg++;
            }
            
            if (strTmp [0] == 'W')
            {
                *pdLongitude = - *pdLongitude;
            }
            
            /* Fix status, '0' - Invalid, '1' - GNSS fix, '2' - DPGS fix,   *
             * '6' - Estimated (dead reckoning) mode*/
            strMsg++;
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                i++;
                strMsg++;
            }
            
            *pchMode = strTmp [0];
            
            /* Number of satellites being used (0~24) */
            strMsg++;
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                i++;
                strMsg++;
            }
            strTmp [i] = 0;            
            
            *piNumOfSatellitesUsed = atoi (strTmp);
            
            /* Horizontal dilution of precision */
            strMsg++;
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                i++;
                strMsg++;
            }
            strTmp [i] = 0;
            
            *pdHdop = atof (strTmp);
            
            /* Altitude in meters acrossing to WGS84 ellipsoid */
            strMsg++;
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                i++;
                strMsg++;
            }
            strTmp [i] = 0;
            *pdAltitude = atof (strTmp);
            
            /* 'M' */
            strMsg++;
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                i++;
                strMsg++;
            }
            
            if (strTmp [0] != 'M')
            {
                eResult = ERROR;
                return eResult;
            }
            
            /* Height of Geoid (means sea level) abover WGS84 ellipsoid, meter */
            strMsg++;
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                strMsg++;
                i++;
            }
            strTmp [i] = 0;
            
            *pdGeoidSeparation = atof (strTmp);
            
            /* 'M' */
            strMsg++;
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                i++;
                strMsg++;
            }
            
            if (strTmp [0] != 'M')
            {
                eResult = ERROR;
                return eResult;
            }
            
            /* Age of DGPS data in seconds, empty if DGPS is not used */
            strMsg++;
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                strMsg++;
                i++;
            }
            strTmp [i] = 0;
            
            *piAgeOfDgps = atoi (strTmp);
            
            /* DGPS station ID, empty if DGPS is not used */
            strMsg++;
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                strMsg++;
                i++;
            }
            strTmp [i] = 0;
            
            *piStationIdOfDgps = atoi (strTmp);
        }
        else
        {
            eResult = ERROR;
        }
    }
    
    return eResult;
}

ErrorStatus LyzkGetInfoFromGllSentence (char* strMsg, LyzkTime* pstTime,
                                        double* pdLatitude, double* pdLongitude,
                                        char* pchMode)
{
    char strTmp [11] = {0};
    int i;
    
    /* Decide whether the check sum is correct. */
    ErrorStatus eResult = LyzkNmeaCheckSum (strMsg);
    
    if (eResult == SUCCESS)
    {
        /* Find the header of '$' */
        while (*strMsg != '$')
        {
            strMsg++;
        }
        
        /* Whether is GLL sentence */
        if (*strMsg == '$' && *(strMsg + 3) == 'G' && *(strMsg + 4) == 'L'
         && *(strMsg + 5) == 'L')
        {
            strMsg += 7;
            /* Latitude, the format is "ddmm.mmmm" */
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                i++;
                strMsg++;
            }
            strTmp [i] = 0;
            
            *pdLatitude = atof (strTmp) / 100.0;
            
            /* Which hemisphere, 'N' - North (+), 'S' - South (-)  */
            strMsg++;
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                i++;
                strMsg++;
            }
            
            if (strTmp [0] == 'S')
            {
                *pdLatitude = - *pdLatitude;
            }
            
            /* Longitude, the format is "dddmm.mmmm" */
            strMsg++;
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                i++;
                strMsg++;
            }
            strTmp [i] = 0;
            
            *pdLongitude = atof (strTmp) / 100.0;
            
            /* East or west of longitude, 'E' - East (+), 'W' - West (-) */
            strMsg++;
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                i++;
                strMsg++;
            }
            
            if (strTmp [0] == 'W')
            {
                *pdLongitude = - *pdLongitude;
            }
            
            /* UTC Time, the format is "hhmmss.sss" */
            strMsg++;
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                i++;
                strMsg++;
            }
            
            pstTime->m_iHour    = (strTmp [0] - '0') * 10 + (strTmp [1] - '0');
            pstTime->m_iMinute  = (strTmp [2] - '0') * 10 + (strTmp [3] - '0');
            pstTime->m_iSecond  = (strTmp [4] - '0') * 10 + (strTmp [5] - '0');
            pstTime->m_iMilliSecond   = (strTmp [7] - '0') * 100
                                      + (strTmp [8] - '0') * 10
                                      + (strTmp [9] - '0');
            
            /* Data valid */
            strMsg++;
            i = 0;
            while (*strMsg != ',')
            {
                strTmp [i] = *strMsg;
                i++;
                strMsg++;
            }
            
            if (strTmp [0] != 'A')
            {
                eResult = ERROR;
                return eResult;
            }
            
            /* Positioning mode, 'N' - No fix, 'A' - Autonomous GNSS fix,   *
             * 'D' - Differential GNSS fix.                                 */
            strMsg++;
            *pchMode = *strMsg;
        }
        else
        {
            eResult = ERROR;
        }
    }
    
    return eResult;
}
