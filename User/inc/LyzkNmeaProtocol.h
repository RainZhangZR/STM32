/******************************************************************************
  Copyright (C), Luoyang Institute of Information Technology Industies (LYZK)
                 and Luoyang Sylincom Smart Equipment Co. Ltd. 2018
  File name: LyzkNmeaProtocol.h
  Author: 张冉
  Version: 1.0.0
  Date: 
  Description: 本程序库封装了对NMEA协议的解析。
  Others: 
  Function List: 
    1. LyzkUsartInit: USART外设的初始化。
    2. LyzkUsartItCfg：配置中断，使用自定义的LYZK_USART_IT_CTS等宏。
  History: (修改的历史记录)
    1.  Date:
        Author:
        Modification: 
 ******************************************************************************/
 
#ifndef __LYZK_NMEA_PROTOCOL_H__
#define __LYZK_NMEA_PROTOCOL_H__

#include "LyzkTime.h"

#ifdef USE_STDPERIPH_DRIVER
  #include "stm32f10x.h"
#else
  typedef enum {ERROR = 0, SUCCESS = !ERROR} ErrorStatus;
#endif /* USE_STDPERIPH_DRIVER */

#define NMEA_MAXSAT     12
#define NMEA_SATINPACK  4

typedef struct
{
    LyzkTime    m_stUtcTime;        /**< UTC of position*/
    LyzkDate    m_stUtcDate;        
    char        m_chStatus;         /**< Status (Data Valid) (A = active or V = void) */
    double      m_dLatitude;        /**< Latitude in degree */
    double      m_dLongitude;       /**< Longitude in degree */
    double      m_dSpeed;           /**< Speed over the ground in knots */
    double      m_dCourse;          /**< Track angle (course) in degrees True */
    double      m_dMagnVariation;   /**< Magnetic variation degrees (Easterly var. *
                                     * subtracts from true course) */
    char        m_chMode;           /**< Mode indicator of fix type (A = autonomous, *
                                     * D = differential, E = estimated, N = not valid,*
                                     * S = simulator) */
} LyzkNmeaRmcInfo;

typedef struct
{
    double      m_dCourseTrue;      /**< Course over ground (true) in degree */
    double      m_dCourseMagn;      /**< Course over ground (magnetic) in degree */
    double      m_dSpeedInKnots;    /**< Speed over ground in knots */
    double      m_dSpeedInKmPerHour;/**< Speed over ground in km/h */
    char        m_chMode;           /**< Positioning Mode, N - No fix; A - Autonomous GNSS fix *
                                     * D - Differential GNSS fix */
} LyzkNmeaVtgInfo;

typedef struct
{
    LyzkTime    m_stUtcTime;        /**< UTC of the position    */
    double      m_dLatitude;        /**< Latitude in degree     */
    double      m_dLongitude;       /**< Longitude in degree    */
    int         m_iSig;             /**< Fix statuse (GPS quality indicator, 0 - Invalid; *
                                     * 1 - GNSS fix, 2 - DGPS fix, 3 - Sensitive    *
                                     * 6 - Estimated (dead reckoning) Mode          */
    int         m_iNumOfSateUsed;   /**< Number of satellites being used (0~24)     */
    double      m_dHdop;            /**< Horizontal dilution of precision           */
    double      m_dAltitude;        /**< Altitude in meters according to WGS84 ellipsoid                */
    double      m_dGeoSeparation;   /**< Height of Geoid (means sea level) above WGS84 ellipsoid, meter */
    double      m_dDgpsAge;         /**< Age of DGPS data in seconds, empty if DGPS is not used         */
    int         m_dDgpsSationId;    /**< DGPS station ID number */
} LyzkNmeaGgaInfo;

typedef struct
{
    char        m_chFixMode;                /**< Auto selection of 2D or 3D fix.    *
                                             * M - Manual, forced to switch 2D/3D mode*
                                             * A - Allowed to automatically switch 2D/3D mode */
    int         m_iFixType;                 /**< 1 - No fix; 2 - 2D fix; 3 - 3D fix */
    int         m_aStaUsed[NMEA_MAXSAT];    /**< Satellite used in channel i */
    double      m_dPdop;                    /**< Position dilution of precision */
    double      m_dHdop;                    /**< Horizontal dilution of precision */
    double      m_dVdop;                    /**< Vertical dilution of precision */
} LyzkNmeaGsaInfo;

typedef struct
{
    int         m_iId;              /**< Satellite ID */
    double      m_dElevation;       /**< Elevation in degree (0~90) */
    double      m_dAzimuth;         /**< Azimuth in degree (0~359)  */
    double      m_dSnr;             /**< Signal to noise ratio in dB-Hz (0~99), empty if not tracking   */
} LyzkNmeaSatelliteInfo;

typedef struct
{
    int         m_iNumOfMsg;        /**< Number of messages, total number of GSV messages   *
                                     * being output (1~4)   */
    int         m_iSeqNum;          /**< Sequence number of this entry (1~4)    */
    int         m_iSatInView;       /**< Total satellites in view   */
    LyzkNmeaSatelliteInfo   m_stSatInfo [NMEA_SATINPACK];   /**< Satellite information  */
} LyzkNmeaGsvInfo;
 
#ifdef __cplusplus
extern "c" {
#endif /* __cplusplus */

/* Get information from RMC sentence. RMC - Recommended Minimum Position Data *
 * (including position, velocity and time).                                   */
//ErrorStatus LyzkGetInfoFromRmcSentence (char* strMsg, LyzkTime* pstTime, LyzkDate* pstDate,
//                                        double* pdLatitude, double* pdLongitude, 
//                                        double* pdSpeed, double* pdCourse, 
//                                        double* pdMagnVari, char* pchMode);
ErrorStatus LyzkGetInfoFromNmeaRmcSentence (const char* strMsg, LyzkNmeaRmcInfo* pstInfo);

/* Get information from VTG sentence. VTG - Track Made Good and Ground Speed. */
ErrorStatus LyzkGetInfoFromVtgSentence (char* strMsg, double* pdCourse, double* pdCourseMagn,
										double* pdSpeedInKnots, double* pdSpeedInKmPerHour,
									    char* pchMode);

/* Get information from GGA sentence. GGA - Global Position System Fix Data,  *
 * the essential fix data which provides 3D location and accuracy data.       */
ErrorStatus LyzkGetInfoFromGgaSentence (char* strMsg, LyzkTime* pstTime,
                                        double* pdLatitude, double* pdLongitude,
                                        char* pchMode, int* piNumOfSatellitesUsed, 
                                        double* pdHdop, double* pdAltitude, 
                                        double* pdGeoidSeparation, int* piAgeOfDgps,
                                        int* piStationIdOfDgps);

/* Get information from GLL sentence. GLL - Geographic Latitude and Longitude,*
 * which contains position information, time of position fix and status.      */
ErrorStatus LyzkGetInfoFromGllSentence (char* strMsg, LyzkTime* pstTime,
                                        double* pdLatitude, double* pdLongitude,
                                        char* pchMode);
#ifdef __cplusplus
}
#endif /* __cplusplus */
 
#endif /* __LYZK_NMEA_PROTOCOL_H__ */
