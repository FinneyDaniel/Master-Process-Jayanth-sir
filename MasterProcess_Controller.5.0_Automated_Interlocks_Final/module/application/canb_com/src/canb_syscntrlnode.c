/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  oi_canb.c
 @author DEEPTI.K
 @date 06-Sep-2021

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include <state_machine.h>
#include "F28x_Project.h"              // Standard libraries headers
#include "F2837xS_Device.h"
#include "math.h"
#include "stdlib.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "hal/driverlib/can.h"
#include "cana_defs.h"
#include "cana_PSUCom.h"

#include "canb_defs.h"

#include "scheduler.h"
#include "control_defs.h"
#include "mathcalc.h"
#include "cana_vsc.h"

/*==============================================================================
 Defines
 ==============================================================================*/

CIRC1_BUF_DEF(uiRxbufferSP, 100);

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

CANB_tzSITERXREGS CANB_tzSiteRxRegs;
CANB_tzSITEREGS CANB_tzSiteRegs;

/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void CANB_fnTX_SiteEvent(void);
void canb_fnParamsUpdate(void);
void CANB_fnRXevent(void);

void CANB_fnTask(void);
void CAN_fnComFailChk(void);

static void canb_fnmsgPrcsSP(uint16_t uiMsgtype, uint16_t *msgDataSP);

bool canb_fnEnquedata(canb_tzcirc_buff *ptr, uint16_t *data, uint32_t msgID,
                      uint16_t DLC);
bool canb_fndequedata(canb_tzcirc_buff *ptr, uint16_t *data, uint32_t *msgID,
                      uint16_t *DLC);

/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t ui16RxMsgDataSite7[8] = { 0 };
uint16_t ui16RxMsgDataSite8[8] = { 0 };

uint16_t ui16txMsgDataSite1[8] = { 0 };
uint16_t ui16txMsgDataSite2[8] = { 0 };
uint16_t ui16txMsgDataSite3[8] = { 0 };
uint16_t ui16txMsgDataSite4[8] = { 0 };

uint16_t ui16rxMsgSP[8] = { 0 };
uint16_t uirxPrcsMsgSP[8] = { 0 };

uint16_t ui16CANBTxCntPSU = 0, ui16CANBTxNodeCntPSU = 0;

uint16_t ui16nodeIDVS = 0, ui16CANBTxCntSnsr = 0, ui16CANBTxCntFlts = 0,
        ui16CANBTxCntVSC = 0;

uint32_t u32CANBmsgID1 = 0;
uint16_t uiCANBDataLength1 = 0;

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void CANB_fnTX_SiteEvent(void)
{

    canb_fnParamsUpdate();

    // Power Supply messages

    ui16CANBTxCntPSU++;

    if (ui16CANBTxCntPSU > 4)
    {
        ui16CANBTxCntPSU = 1;
    }

    switch (ui16CANBTxCntPSU)
    {

    case 1:

        CANB_tzSiteRegs.txHBCnt++;
        if (CANB_tzSiteRegs.txHBCnt >= 255)
        {
            CANB_tzSiteRegs.txHBCnt = 0;
        }

        CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_1,
                               (0x11843020 | (CANA_tzIORegs.uiUnitID << 8)),
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN8);

        ui16txMsgDataSite1[0] = CANB_tzSiteRegs.txHBCnt;
        ui16txMsgDataSite1[1] = 0x01;
        ui16txMsgDataSite1[2] = (CANA_tzActNodeRegs_PSU.all & 0xFF00) >> 8;
        ui16txMsgDataSite1[3] = (CANA_tzActNodeRegs_PSU.all & 0x00FF);

        ui16txMsgDataSite1[4] = (CANA_tzActNode1Regs_PSU.all & 0xFF00) >> 8;
        ui16txMsgDataSite1[5] = (CANA_tzActNode1Regs_PSU.all & 0x00FF);
        ui16txMsgDataSite1[6] = ui32NodePresent;

        ui16txMsgDataSite1[7] = 0;

        CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_1, CAN_mLEN8,
                        ui16txMsgDataSite1);

        break;

    case 2:

        ui16CANBTxNodeCntPSU++;

        if (ui16CANBTxNodeCntPSU > 26)
        {
            ui16CANBTxNodeCntPSU = 1;
        }

        CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_1,
                               (0x11843020 | (CANA_tzIORegs.uiUnitID << 8)),
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN8);

        ui16txMsgDataSite1[0] = ui16CANBTxNodeCntPSU;
        ui16txMsgDataSite1[1] = 0x02;
        ui16txMsgDataSite1[2] =
                CANB_tzSiteRegs.ui16MeasVolt[ui16CANBTxNodeCntPSU] >> 8;
        ui16txMsgDataSite1[3] =
                (CANB_tzSiteRegs.ui16MeasVolt[ui16CANBTxNodeCntPSU] & 0x00FF);

        ui16txMsgDataSite1[4] =
                (CANB_tzSiteRegs.ui16MeasCurr[ui16CANBTxNodeCntPSU] >> 8);
        ui16txMsgDataSite1[5] =
                (CANB_tzSiteRegs.ui16MeasCurr[ui16CANBTxNodeCntPSU] & 0x00FF);

        ui16txMsgDataSite1[6] =
                (CANB_tzSiteRegs.ui16PFCHFlts[ui16CANBTxNodeCntPSU]);
        ui16txMsgDataSite1[7] =
                (CANB_tzSiteRegs.ui16PFCLFlts[ui16CANBTxNodeCntPSU]);

        CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_1, CAN_mLEN8,
                        ui16txMsgDataSite1);

        break;

    case 3:

        CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_1,
                               (0x11843020 | (CANA_tzIORegs.uiUnitID << 8)),
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN8);

        ui16txMsgDataSite1[0] = ui16CANBTxNodeCntPSU;
        ui16txMsgDataSite1[1] = 0x03;
        ui16txMsgDataSite1[2] = 0;
        ui16txMsgDataSite1[3] = 0; // Rsvd for Mark-II PFC Faults

        ui16txMsgDataSite1[4] =
                (CANB_tzSiteRegs.ui16DCDCFlts[ui16CANBTxNodeCntPSU] >> 8);
        ui16txMsgDataSite1[5] =
                (CANB_tzSiteRegs.ui16DCDCFlts[ui16CANBTxNodeCntPSU] & 0x00FF);

        ui16txMsgDataSite1[6] = (0);
        ui16txMsgDataSite1[7] = (0); // Rsvd for Mark-II DCDC Faults

        CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_1, CAN_mLEN8,
                        ui16txMsgDataSite1);

        break;

    case 4:

        CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_1,
                               (0x11843020 | (CANA_tzIORegs.uiUnitID << 8)),
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN8);

        ui16txMsgDataSite1[0] = ui16CANBTxNodeCntPSU;
        ui16txMsgDataSite1[1] = 0x04;
        ui16txMsgDataSite1[2] = CANB_tzSiteRegs.ui16ACVolt[ui16CANBTxNodeCntPSU]
                >> 8;
        ui16txMsgDataSite1[3] =
                (CANB_tzSiteRegs.ui16ACVolt[ui16CANBTxNodeCntPSU] & 0x00FF);

        ui16txMsgDataSite1[4] =
                (CANB_tzSiteRegs.ui16ACCurr[ui16CANBTxNodeCntPSU] >> 8);
        ui16txMsgDataSite1[5] =
                (CANB_tzSiteRegs.ui16ACCurr[ui16CANBTxNodeCntPSU] & 0x00FF);

        ui16txMsgDataSite1[6] =
                (CANB_tzSiteRegs.ui16ACFreq[ui16CANBTxNodeCntPSU] >> 8);
        ui16txMsgDataSite1[7] =
                (CANB_tzSiteRegs.ui16ACFreq[ui16CANBTxNodeCntPSU] & 0x00FF);

        CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_1, CAN_mLEN8,
                        ui16txMsgDataSite1);

        break;

    default:
        break;
    }

    // Sensor Data from Master Controller

    ui16CANBTxCntSnsr++;

    if (ui16CANBTxCntSnsr > 10)
    {
        ui16CANBTxCntSnsr = 1;
    }

    switch (ui16CANBTxCntSnsr)
    {

    int32_t ui32temp;

case 1:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x1;

    ui32temp = (MATHConvtzRegs.AISensorHYS101 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[3] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorHYS501 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorHYS401 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);

    break;

case 2:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x2;

    ui32temp = (MATHConvtzRegs.AISensorHYS102 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[3] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorLVL101 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorOXS101 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);

    break;


case 3:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x3;

    ui32temp = (MATHConvtzRegs.AISensorPRT101 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[3] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorPRT102 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorPRT401 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);
    break;

case 4:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x4;

    ui32temp = (MATHConvtzRegs.AISensorPRT402 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[3] = (ui32temp & 0x00FF);

    ui32temp = (CANA_tzAISensorData.DPT_401 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorTE401 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);
    break;

case 5:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x5;
    ui16txMsgDataSite2[2] = (CANB_tzSiteRegs.WaterDemand);
    ui16txMsgDataSite2[3] = (CANB_tzSiteRegs.TurnONLCC);

    ui16txMsgDataSite2[4] = (0);
    ui16txMsgDataSite2[5] = (0);

    ui16txMsgDataSite2[6] = (0);
    ui16txMsgDataSite2[7] = (0);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);
    break;

case 6:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x6;

    ui32temp = (MATHConvtzRegs.TempSensorTTC101 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[3] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.TempSensorTTC102 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.TempSensorTTC301 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);
    break;

case 7:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11943020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x7;

    ui32temp = (MATHConvtzRegs.TempSensorKTC401 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[3] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorEBV801 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[5] = (ui32temp & 0x00FF);

    ui32temp = (MATHConvtzRegs.AISensorPMP101 * 100);

    if (ui32temp < 0)
    {
        ui32temp = 0;
    }

    ui16txMsgDataSite2[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite2[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);
    break;

default:
    break;

    }

    // LHC and LPC faults

    ui16CANBTxCntFlts++;

    if (ui16CANBTxCntFlts > 4)
    {
        ui16CANBTxCntFlts = 1;
    }

    switch (ui16CANBTxCntFlts)
    {
    case 1:

        CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_3,
                               (0x11A43020 | (CANA_tzIORegs.uiUnitID << 8)),
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN8);

        ui16txMsgDataSite3[0] = 0x0;
        ui16txMsgDataSite3[1] = 0x1;
        ui16txMsgDataSite3[2] = CANA_tzLPCIO1_DIFaultRegs.all;
        ui16txMsgDataSite3[3] = CANA_tzLPCIO1_AIFaultRegs.all;
        //ui16txMsgDataSite3[4] = CANA_tzLPCIO1_AOFaultRegs.all ;
        //ui16txMsgDataSite3[5] = CANA_tzLPCIO1_DOFaultRegs.all ;
        ui16txMsgDataSite3[6] = CANA_tzActNodeRegs_IO.all;

        CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_3, CAN_mLEN8,
                        ui16txMsgDataSite3);

        break;

    case 2:

        CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_3,
                               (0x11A43020 | (CANA_tzIORegs.uiUnitID << 8)),
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN8);

        ui16txMsgDataSite3[0] = 0x0;
        ui16txMsgDataSite3[1] = 0x2;
        ui16txMsgDataSite3[2] = CANA_tzLPCIO2_DIFaultRegs.all;
        ui16txMsgDataSite3[3] = CANA_tzLPCIO2_AIFaultRegs.all;
        //ui16txMsgDataSite3[4] = CANA_tzLPCIO2_AOFaultRegs.all ;
        //ui16txMsgDataSite3[5] = CANA_tzLPCIO2_DOFaultRegs.all ;
        ui16txMsgDataSite3[6] = CONTROLtzFaultRegs.all;

        CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_3, CAN_mLEN8,
                        ui16txMsgDataSite3);

        break;

    case 3:

        CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_3,
                               (0x11A43020 | (CANA_tzIORegs.uiUnitID << 8)),
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN8);

        ui16txMsgDataSite3[0] = 0x0;
        ui16txMsgDataSite3[1] = 0x3;
        ui16txMsgDataSite3[2] = CANA_tzLHCIO1_DIFaultRegs.all;
        ui16txMsgDataSite3[3] = CANA_tzLHCIO1_AIFaultRegs.all;
        //ui16txMsgDataSite3[4] = CANA_tzLHCIO1_AOFaultRegs.all ;
        //ui16txMsgDataSite3[5] = CANA_tzLHCIO1_DOFaultRegs.all ;
        ui16txMsgDataSite3[6] = CANA_tzThermalFaultRegs.all;

        CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_3, CAN_mLEN8,
                        ui16txMsgDataSite3);

        break;

    case 4:

        CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_3,
                               (0x11A43020 | (CANA_tzIORegs.uiUnitID << 8)),
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CAN_mLEN8);

        ui16txMsgDataSite3[0] = 0x0;
        ui16txMsgDataSite3[1] = 0x4;
        ui16txMsgDataSite3[2] = CANA_tzLHCIO2_DIFaultRegs.all;
        ui16txMsgDataSite3[3] = CANA_tzLHCIO2_AIFaultRegs.all;
        //ui16txMsgDataSite3[4] = CANA_tzLHCIO2_AOFaultRegs.all ;
        //ui16txMsgDataSite3[5] = CANA_tzLHCIO2_DOFaultRegs.all ;
        ui16txMsgDataSite3[6] = CANA_tzActMS_FaultRegs.all;

        CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_3, CAN_mLEN8,
                        ui16txMsgDataSite3);

        break;

    default:
        break;
    }

    // VSC data

    ui16CANBTxCntVSC++;

    if (ui16CANBTxCntVSC > 8)
    {
        ui16CANBTxCntVSC = 1;
    }

    switch (ui16CANBTxCntVSC)
    {
    int32_t ui32temp;

case 1:

    ui16nodeIDVS++;
    if (ui16nodeIDVS > 16)
    {
        ui16nodeIDVS = 1;
    }

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_4,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite4[0] = ui16nodeIDVS;
    ui16txMsgDataSite4[1] = 0x1;

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[1] * 1000);

    ui16txMsgDataSite4[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[3] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[2] * 1000);

    ui16txMsgDataSite4[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[5] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[3] * 1000);

    ui16txMsgDataSite4[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_4, CAN_mLEN8, ui16txMsgDataSite4);

    break;

case 2:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_4,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite4[0] = ui16nodeIDVS;
    ui16txMsgDataSite4[1] = 0x2;

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[4] * 1000);

    ui16txMsgDataSite4[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[3] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[5] * 1000);

    ui16txMsgDataSite4[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[5] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[6] * 1000);

    ui16txMsgDataSite4[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_4, CAN_mLEN8, ui16txMsgDataSite4);

    break;

case 3:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_4,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite4[0] = ui16nodeIDVS;
    ui16txMsgDataSite4[1] = 0x3;

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[7] * 1000);

    ui16txMsgDataSite4[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[3] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[8] * 1000);

    ui16txMsgDataSite4[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[5] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[9] * 1000);

    ui16txMsgDataSite4[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_4, CAN_mLEN8, ui16txMsgDataSite4);

    break;

case 4:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_4,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite4[0] = ui16nodeIDVS;
    ui16txMsgDataSite4[1] = 0x4;

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[10] * 1000);

    ui16txMsgDataSite4[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[3] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[11] * 1000);

    ui16txMsgDataSite4[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[5] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[12] * 1000);

    ui16txMsgDataSite4[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_4, CAN_mLEN8, ui16txMsgDataSite4);

    break;

case 5:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_4,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite4[0] = ui16nodeIDVS;
    ui16txMsgDataSite4[1] = 0x5;

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[14] * 1000);

    ui16txMsgDataSite4[2] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[3] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[15] * 1000);

    ui16txMsgDataSite4[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[5] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32Cellvolt[16] * 1000);

    ui16txMsgDataSite4[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_4, CAN_mLEN8, ui16txMsgDataSite4);

    break;

case 6:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_4,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite4[0] = ui16nodeIDVS;
    ui16txMsgDataSite4[1] = 0x6;

    ui16txMsgDataSite4[2] = (canA_tzVSC_info[ui16nodeIDVS].uiMaxcellNum[0]
            * 0xFF);

    ui16txMsgDataSite4[3] = (canA_tzVSC_info[ui16nodeIDVS].uiMaxcellNum[1]
            & 0xFF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32MaxCellVolt[0] * 1000);

    ui16txMsgDataSite4[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[5] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32MaxCellVolt[1] * 1000);

    ui16txMsgDataSite4[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[5] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_4, CAN_mLEN8, ui16txMsgDataSite4);

    break;

case 7:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_4,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite4[0] = ui16nodeIDVS;
    ui16txMsgDataSite4[1] = 0x7;

    ui16txMsgDataSite4[2] = (canA_tzVSC_info[ui16nodeIDVS].uiMincellNum[0]
            * 0xFF);

    ui16txMsgDataSite4[3] = (canA_tzVSC_info[ui16nodeIDVS].uiMincellNum[1]
            & 0xFF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32MinCellVolt[0] * 1000);

    ui16txMsgDataSite4[4] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[5] = (ui32temp & 0x00FF);

    ui32temp = (canA_tzVSC_info[ui16nodeIDVS].f32MinCellVolt[1] * 1000);

    ui16txMsgDataSite4[6] = (ui32temp & 0xFF00) >> 8;
    ui16txMsgDataSite4[7] = (ui32temp & 0x00FF);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_4, CAN_mLEN8, ui16txMsgDataSite4);

    break;

default:
    break;
    }

}

/*=============================================================================
 @Function name : void can_fnEnquedata(void)
 @brief  function to add data to queue

 @param void
 @return void
 ============================================================================ */

bool canb_fnEnquedata(canb_tzcirc_buff *ptr, uint16_t *data, uint32_t msgID,
                      uint16_t DLC)
{

    int16_t i_count;

    i_count = ptr->i_head + 1;

    if (i_count >= ptr->i_maxlen)
    {
        i_count = 0;
    }

    if (i_count == ptr->i_tail)
        return false;  // buffer is full

    memcpy(ptr->canB_tzRxC_buffer[ptr->i_head].uiDataBuffer, data, 8);
    ptr->canB_tzRxC_buffer[ptr->i_head].u32_msgID = msgID;
    ptr->canB_tzRxC_buffer[ptr->i_head].i_DataLength = (uint16_t) DLC;
    ptr->i_head = i_count;

    return true;
}

/*=============================================================================
 @Function name : void can_fndequedata(void)
 @brief  function to Delete data from queue

 @param void
 @return void
 ============================================================================ */
bool canb_fndequedata(canb_tzcirc_buff *ptr, uint16_t *data, uint32_t *msgID,
                      uint16_t *DLC)
{
    int16_t i_count;

    if (ptr->i_head == ptr->i_tail)
    {
        return false; // buffer is empty
    }

    i_count = ptr->i_tail + 1;

    if (i_count >= ptr->i_maxlen)
    {
        i_count = 0;
    }

    memcpy(data, ptr->canB_tzRxC_buffer[ptr->i_tail].uiDataBuffer, 8);
    *msgID = ptr->canB_tzRxC_buffer[ptr->i_tail].u32_msgID;
    *DLC = ptr->canB_tzRxC_buffer[ptr->i_tail].i_DataLength;
    ptr->i_tail = i_count;

    return true;

}

/*=============================================================================
 @Function name : void CAN_fnRXevent(void)
 @brief  function to receive messages over CAN

 @param void
 @return void
 ============================================================================ */
void CANB_fnRXevent(void)
{

    //any commands for AO/DO control
    if (CAN_IsMessageReceived(CANB_BASE, CAN_mMAILBOX_11))
    {
        CAN_readMessage(CANB_BASE, CAN_mMAILBOX_11, ui16rxMsgSP);

        canb_fnEnquedata(&uiRxbufferSP, ui16rxMsgSP, CanbRegs.CAN_IF2ARB.bit.ID,
                         CanbRegs.CAN_IF2MCTL.bit.DLC);

        CANB_tzSiteRegs.btMSComStart = 1;
        CANB_tzSiteRegs.SPComFailCnt = 0;

    }

}

/*=============================================================================
 @Function name : void CAN_fnRXevent(void)
 @brief  function to receive messages over CAN

 @param void
 @return void
 ============================================================================ */
void CANB_fnTask(void)
{
    uint32_t ui32temp;

    while (canb_fndequedata(&uiRxbufferSP, uirxPrcsMsgSP, &u32CANBmsgID1,
                            &uiCANBDataLength1))
    {

        ui32temp = (u32CANBmsgID1 & 0x00F00000);
        CANB_tzSiteRegs.uiMsgtype = (uint16_t) (ui32temp >> 20);

        canb_fnmsgPrcsSP(CANB_tzSiteRegs.uiMsgtype, uirxPrcsMsgSP);
    }

}

/*=============================================================================
 @Function name : void can_fnmsgProcess(void)
 @brief  function to Process Master Safety messages over CAN

 @param void
 @return void
 ============================================================================ */
static void canb_fnmsgPrcsSP(uint16_t uiMsgtype, uint16_t *msgDataSP)
{

    switch (uiMsgtype)
    {

    case 1:

//        if (CANB_tzSiteRegs.RxCntSP != msgDataSP[0])
    {
        CANB_tzSiteRegs.RxCntSP = msgDataSP[0];
//            CANB_tzSiteRegs.StartCmd = msgDataSP[1];
//            CANB_tzSiteRegs.PresentStMS = msgDataSP[2];
//            CANB_tzSiteRegs.AOCmd = msgDataSP[3];
//            CANB_tzSiteRegs.all = msgDataSP[4];
//            CANB_tzSiteRegs.AOVFan101 = msgDataSP[5];
//            CANB_tzSiteRegs.AOVFan501 = msgDataSP[6];
//            CANB_tzSiteRegs.AOVFan401 = msgDataSP[7];
        CANB_tzSiteRegs.btMSComStart = true;
        CANB_tzSiteRegs.SPComFailCnt = 0;

    }

        if ((msgDataSP[1] == 0x01) && (msgDataSP[2] == 0xAA)
                && (msgDataSP[3] == 0xAA))
        {
            CANB_tzSiteRegs.StartCmd = 0x01;
        }
        else if ((msgDataSP[1] == 0x01) && (msgDataSP[2] == 0x0)
                && (msgDataSP[3] == 0x0))
        {
            CANB_tzSiteRegs.StartCmd = 0x0;
        }

        if ((msgDataSP[1] == 0x02) && (msgDataSP[2] == 0xBB)
                && (msgDataSP[3] == 0xBB))
        {
            CANB_tzSiteRxRegs.Start_H2CmdStkpwr = 0x01;
        }
        else if ((msgDataSP[1] == 0x02) && (msgDataSP[2] == 0x0)
                && (msgDataSP[3] == 0x0))
        {
            CANB_tzSiteRxRegs.Start_H2CmdStkpwr = 0x0;
        }

        CANB_tzSiteRegs.SPComFailCnt++;
        if (CANB_tzSiteRegs.SPComFailCnt >= 3000)
        {
            CANB_tzSiteRegs.btMSComStart = false;
            CANB_tzSiteRegs.SPComFailCnt = 3001;
        }

        break;

    case 2:

        if((msgDataSP[0] == 0x0) && (msgDataSP[1] == 0x10))

        CANB_tzSiteRegs.H2Percent = ((msgDataSP[2] << 8) | (msgDataSP[3]))
                * 0.01;

        CANB_tzSiteRegs.f32VoltSet = 400.0;

        CANB_tzSiteRegs.f32CurrSet =
                (CANB_tzSiteRegs.H2Percent * 0.01 * 1000.0);

        CANB_tzSiteRegs.numofCells = ((msgDataSP[4] << 8) | (msgDataSP[5]));

        CANB_tzSiteRegs.numofStack = ((msgDataSP[6] << 8) | (msgDataSP[7]));

        break;

    default:
        break;
    }
}

void canb_fnParamsUpdate()
{
    CANB_tzSiteRegs.ui16MeasVolt[CANA_PSURegs.Node] =
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[CANA_PSURegs.Node] * 100;
    CANB_tzSiteRegs.ui16MeasCurr[CANA_PSURegs.Node] =
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[CANA_PSURegs.Node] * 100;
    CANB_tzSiteRegs.ui16DCDCFlts[CANA_PSURegs.Node] =
            CANA_tzDCDCFaultRegs[CANA_PSURegs.Node].all;
    CANB_tzSiteRegs.ui16PFCHFlts[CANA_PSURegs.Node] =
            CANA_tzFECHFaultRegs[CANA_PSURegs.Node].all;
    CANB_tzSiteRegs.ui16PFCLFlts[CANA_PSURegs.Node] =
            CANA_tzFECLFaultRegs[CANA_PSURegs.Node].all;

    CANB_tzSiteRegs.ui16ACFreq[CANA_PSURegs.Node] =
            (CANA_tzRxdRegs.tzPSUData.ACparamFreq[CANA_PSURegs.Node] * 10);
    CANB_tzSiteRegs.ui16ACVolt[CANA_PSURegs.Node] =
            (CANA_tzRxdRegs.tzPSUData.AClineVolt[CANA_PSURegs.Node]);

    CANB_tzSiteRegs.ui16ACCurr[CANA_PSURegs.Node] =
            (CANA_tzRxdRegs.tzPSUData.AClineCurr[CANA_PSURegs.Node] * 10);
}

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*==============================================================================
 End of File
 ==============================================================================*/

