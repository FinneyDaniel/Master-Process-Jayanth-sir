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

typedef struct CANB_zRX_CIRCULAR_BUF
{
    uint16_t uiDataBuffer[CANA_mRX_BUFFER_SIZE];
    uint32_t u32_msgID;
    int16_t i_DataLength;

} CANB_tzRX_CIRCULAR_BUF;

typedef struct canb_zcirc_buff
{
    CANB_tzRX_CIRCULAR_BUF canB_tzRxC_buffer[100];
    int16_t i_head;
    int16_t i_tail;
    const int16_t i_maxlen;
} canb_tzcirc_buff;


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




} CANB_tzSITERXREGS;


typedef struct CANB_zSITEREGS
{

   uint16_t MsgID7;
   uint16_t MBox7;
   uint16_t MBox8;
   uint16_t MsgID8;

   uint16_t uiVoltInt;
   uint16_t uiVoltFrac;
   float32_t f32VoltSet;
   float32_t H2Percent;
   uint16_t numofCells;


   uint16_t uiCurrInt;
   uint16_t uiCurrFrac;
   float32_t f32CurrSet;

   uint16_t StartCmd;


   uint16_t queryfaults;
   uint16_t ui16MeasVolt[29];
   uint16_t ui16MeasCurr[29];
   uint16_t ui16PFCLFlts[29];
   uint16_t ui16PFCHFlts[29];
   uint16_t ui16DCDCFlts[29];
   uint16_t ui16ACFreq[27];
   uint16_t ui16ACVolt[27];
   uint16_t ui16ACCurr[27];
   uint16_t VFDFreqSet;
   uint16_t uiNumCells;

   uint16_t txHBCnt;
   uint16_t WaterDemand;
   uint16_t TurnONLCC;
   uint16_t uiMsgtype;
   uint32_t SPComFailCnt;
  bool btMSComStart;
} CANB_tzSITEREGS;

/*==============================================================================
 Macros
==============================================================================*/


#define CIRC1_BUF_DEF(x,y)                \
    canb_tzcirc_buff x = {                 \
        .i_head = 0,                      \
        .i_tail = 0,                      \
        .i_maxlen = y                     \
    }

/*==============================================================================
 Extern/Public Function Prototypes
==============================================================================*/

extern void CANB_fnTX_SiteEvent(void);
extern void CANB_fnRX_SiteEvent(void);
extern void canb_fnReadMBox_Site(void);
extern void canb_fnParamsUpdate(void);
extern void CANB_fnRXevent(void);
extern void CANB_fnTask(void);

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
