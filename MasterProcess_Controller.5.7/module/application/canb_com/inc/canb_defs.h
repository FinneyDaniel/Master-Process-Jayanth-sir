/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  oi_canb.h
 @author DEEPTI.K
 @date 06-Sep-2021

 @brief Description
==============================================================================*/

#ifndef MODULE_CANB_COM_INC_CANB_DEFS_H_
#define MODULE_CANB_COM_INC_CANB_DEFS_H_

/*==============================================================================
 Includes
==============================================================================*/

#include "F28x_Project.h"              // Standard libraries headers
#include "F2837xS_Device.h"
#include "math.h"
#include "stdlib.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "hal/driverlib/can.h"

/*==============================================================================
 Defines
==============================================================================*/


/*==============================================================================
 Defines
 ==============================================================================*/


/*==============================================================================
 Enums
==============================================================================*/

/*==============================================================================
 Structures
==============================================================================*/



typedef struct CANB_zSITERXREGS
{
    uint16_t Start_cmd;
    uint16_t RUTHandShake;
    uint16_t Start_H2Cmd;
    uint16_t StartSpectroscopy;
    uint16_t ImpAmpVar;
    uint16_t ImpFreqVar;
    uint16_t StateChngStandBy;
    uint16_t StateChngFault;

    uint16_t uiVoltInt;
    uint16_t uiVoltFrac;
    float32_t f32VoltSet;
    float32_t H2Percent;

    uint16_t uiCurrInt;
    uint16_t uiCurrFrac;
    float32_t f32CurrSet;



    uint16_t queryfaults;


} CANB_tzSITERXREGS;


typedef struct CANB_zSITEREGS
{

   uint16_t MsgID7;
   uint16_t MBox7;
   uint16_t MBox8;
   uint16_t MsgID8;


} CANB_tzSITEREGS;

/*==============================================================================
 Macros
==============================================================================*/

/*==============================================================================
 Extern/Public Function Prototypes
==============================================================================*/

extern void CANB_fnSite_Event(void);
extern void canb_fnReadMBox_Site(void);

/*==============================================================================
 Extern/Public Variables
==============================================================================*/

extern CANB_tzSITERXREGS CANB_tzSiteRxRegs;
extern CANB_tzSITEREGS CANB_tzSiteRegs;

extern uint16_t ui16txMsgDataSite[8];
extern uint16_t ui16txMsgDataSite[8];
extern uint16_t ui16RxMsgDataSite2[8];
extern uint16_t ui16RxMsgDataSite3[8];
extern uint16_t ui16RxMsgDataSite4[8];
extern uint16_t ui16RxMsgDataSite5[8];

extern uint16_t ui16RxMsgDataSite6[8];
extern uint16_t ui16RxMsgDataSite7[8];
extern uint16_t ui16RxMsgDataPSU8[8];
extern uint16_t ui16RxMsgDataSite9[8];
extern uint16_t ui16RxMsgDataSite10[8];
extern uint16_t ui16RxMsgDataSite11[8];

extern uint16_t ui16RxMsgDataSite12[8];





extern uint16_t ui16txMsgDataSite2[8];
extern uint16_t ui16txMsgDataSite3[8];
extern uint16_t ui16txMsgDataSite4[8];
extern uint16_t ui16txMsgDataSite5[8];
extern uint16_t ui16txMsgDataSite6[8];
extern uint16_t ui16txMsgDataSite7[8];


extern uint16_t ui16txMsgDataSite8[8];
extern uint16_t ui16txMsgDataSite9[8];

extern uint16_t ui16nableSensorReadings;

/*==============================================================================
 Extern/Public Constants
==============================================================================*/


#endif /* MODULE_CANB_COM_INC_CANB_DEFS_H_ */
