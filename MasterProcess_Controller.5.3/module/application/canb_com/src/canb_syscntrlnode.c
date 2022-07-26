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

/*==============================================================================
 Defines
 ==============================================================================*/

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

void CANB_fnSite_Event(void);
void canb_fnReadMBox_Site(void);
void canb_fnParamsUpdate(void);

/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t ui16RxMsgDataSite7[8] = { 0 };
uint16_t ui16RxMsgDataSite8[8] = { 0 };

uint16_t ui16txMsgDataSite1[8] = { 0 };
uint16_t ui16txMsgDataSite2[8] = { 0 };
uint16_t ui16txMsgDataSite3[8] = { 0 };
uint16_t ui16txMsgDataSite4[8] = { 0 };

uint16_t ui16CANBTxCntPSU = 0, ui16CANBTxNodeCntPSU = 0;

uint16_t ui16CANBTxCntSnsr = 0;
/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void CANB_fnSite_Event(void)
{
    canb_fnReadMBox_Site();

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
                           (0x11843020 | (CANA_tzIORegs.uiUnitID << 8)),
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
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
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
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
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
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x4;

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

case 5:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x5;

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

case 6:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x6;
    ui16txMsgDataSite2[2] = (CANB_tzSiteRegs.WaterDemand);
    ui16txMsgDataSite2[3] = (CANB_tzSiteRegs.TurnONLCC);

    ui16txMsgDataSite2[4] = (0);
    ui16txMsgDataSite2[5] = (0);

    ui16txMsgDataSite2[6] = (0);
    ui16txMsgDataSite2[7] = (0);

    CAN_sendMessage(CANB_BASE, CAN_mMAILBOX_2, CAN_mLEN8, ui16txMsgDataSite2);
    break;

case 7:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x7;

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


case 8:

    CAN_setupMessageObject(CANB_BASE, CAN_mMAILBOX_2,
                           (0x11B43020 | (CANA_tzIORegs.uiUnitID << 8)),
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    ui16txMsgDataSite2[0] = 0x0;
    ui16txMsgDataSite2[1] = 0x8;

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

    /**********************START & STOP command , RUT, H2 Percent***********************************************/

    if (CANB_tzSiteRegs.MBox7 == 1)
    {
        CANB_tzSiteRegs.MBox7 = CAN_readMessage(CANB_BASE, CAN_mMAILBOX_7,
                                                ui16RxMsgDataSite7);

        CANB_tzSiteRegs.MsgID7 = CanbRegs.CAN_IF2ARB.bit.ID;

//        if ((ui16RxMsgDataSite7[0] == 0xAA) && (ui16RxMsgDataSite7[1] == 0xAA)) //Start Purge Command
//        {
//            CANB_tzSiteRxRegs.Start_cmd = 0x1;  // Ready State
//        }
//        if ((ui16RxMsgDataSite7[0] == 0xBB) && (ui16RxMsgDataSite7[1] == 0xBB))
//        {
//            CANB_tzSiteRxRegs.Start_cmd = 0x2;  // Status Check State
//        }
//        if ((ui16RxMsgDataSite7[0] == 0xCC) && (ui16RxMsgDataSite7[1] == 0xCC))
//        {
//            CANB_tzSiteRxRegs.Start_cmd = 0x3;  // Stack Power
//        }
////        if ((ui16RxMsgDataSite7[0] == 0xDD) && (ui16RxMsgDataSite7[1] == 0xDD))
////        {
////            CANB_tzSiteRxRegs.Start_cmd = 0x4;  // Fault State
////        }
//        if ((ui16RxMsgDataSite7[0] == 0xEE) && (ui16RxMsgDataSite7[1] == 0xEE))
//        {
//            CANB_tzSiteRxRegs.Start_cmd = 0x5;  // Commissioning State
//        }
//        if ((ui16RxMsgDataSite7[0] == 0x00) && (ui16RxMsgDataSite7[1] == 0x00))
//        {
//            CANB_tzSiteRxRegs.Start_cmd = 0x0;  // StandBy State
//
//        }

        CANB_tzSiteRegs.MBox7 = 0;
    }

    /*****************************************************************************************************/

    if (CANB_tzSiteRegs.MBox8 == 1) //set current and set voltage from site controller
    {
        CAN_readMessage(CANB_BASE, CAN_mMAILBOX_8, ui16RxMsgDataSite8);

        CANB_tzSiteRegs.MsgID8 = CanbRegs.CAN_IF2ARB.bit.ID;

        CANB_tzSiteRegs.H2Percent = ((ui16RxMsgDataSite8[0] << 8)
                | (ui16RxMsgDataSite8[1])) * 0.01;

        CANB_tzSiteRegs.f32VoltSet = 400.0; //CV limit changed after connecting 15 power Supplies 15 cell voltages
        CANB_tzSiteRegs.f32CurrSet =
                (CANB_tzSiteRegs.H2Percent * 0.01 * 1000.0);

        CANB_tzSiteRegs.MBox8 = 0;

    }

}

void canb_fnReadMBox_Site(void)
{

// RX ID is 0x14160103 - MAILBOX7
// RECEIVE QUERY for RUT Command (R U There? for handshaking) / H2 command / Start / Reset Command
// If data is 1, response in 0x14160103

    CANB_tzSiteRegs.MBox7 = CAN_readMessage(CANB_BASE, CAN_mMAILBOX_7,
                                            ui16RxMsgDataSite7); //

// RX ID is 0x141601A1 - MAILBOX8
// RECEIVE QUERY for H2 Percentage Set and Voltage Set

    CANB_tzSiteRegs.MBox8 = CAN_readMessage(CANB_BASE, CAN_mMAILBOX_8,
                                            ui16RxMsgDataSite8); //

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
 End of File
 ==============================================================================*/

