/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  oi_psucan.c
 @author DEEPTI.K
 @date 03-Sep-2021

 @brief Description of CAN communication with PSU's and Master Controller
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/
#include <state_machine.h>
#include "cana_defs.h"
#include "F28x_Project.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "hal/driverlib/can.h"
#include "hal/driverlib/debug.h"
#include "hal/driverlib/interrupt.h"
#include "math.h"
#include "isr.h"
#include "canb_defs.h"
#include "cana_PSUCom.h"

/*==============================================================================
 Defines
 ==============================================================================*/

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

CANA_tzTXDRegs CANA_tzTxdRegs;
CANA_tzRXDRegs CANA_tzRxdRegs;
CANA_tzPSURegs CANA_PSURegs;

CANA_tzTIMERRegs CANA_tzTimerRegs;
CANA_tzQUERYTYPE CANA_tzQueryType;
union CANA_tzACTNODE_REGS CANA_tzActNodeRegs_PSU;
union CANA_tzACTNODE1_REGS CANA_tzActNode1Regs_PSU;

union CANA_tzFAULT_DCDCREGS CANA_tzDCDCFaultRegs[29];
union CANA_tzFAULT_FECHREGS CANA_tzFECHFaultRegs[29];
union CANA_tzFAULT_FECLREGS CANA_tzFECLFaultRegs[29];
/*==============================================================================
 *
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void CANA_fnPSUTX_Event(void);
void CANA_fnPSURX_Event(void);
void cana_fnTurnON_PSU(int32 i32nodeID, int32 i32ON_command, int32 i32global);
void cana_fnQryProgParam_PSU(int32 i32nodeID);
void cana_fnQryOutputParam_PSU(int32 i32nodeID);
void cana_fnQryFaultsParam_PSU(int32 i32nodeID);
void cana_fnQryACParam_PSU(int32 i32nodeID);
void cana_fnSendImpCmds(int32 i32nodeID);

void cana_fnSetVoltage_PSU(int32 i32nodeID, float32 f32voltageValue,
                           Uint16 ui16global);

void cana_fnSetCurrent_PSU(int32 i32nodeID, float32 f32CurrentValue,
                           Uint16 ui16global);
void cana_fnReadMBox_PSU(void);
void cana_fnNodeCheckLogic_PSU(void);
void cana_fnTimeoutFaultsLogic_PSU(void);

/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t ui16txMsgDataPSU[8] = { 0 };
uint16_t ui16RxMsgDataPSU[8] = { 0 };
uint16_t ui16Rx1MsgDataPSU[8] = { 0 };
uint16_t ui16Rx2MsgDataPSU[8] = { 0 };
uint16_t ui16Rx3MsgDataPSU[8] = { 0 };

uint16_t ui16CANAPSUFailTrig1 = 0;
uint32_t ui32CANAPSUFailCnt1 = 0;

uint16_t ui16CANAIOFailTrig1 = 0;
uint32_t ui32CANAIOFailCnt1 = 0;
uint32_t ui32tempImp[3] = { 0 };
uint16_t ui16ModeChange = 0;
uint16_t ui16manualTesting = 0, PSUCommand = 0;
/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void CANA_fnPSUTX_Event(void)
{

    switch (STAT_tzStateMac.Present_st)
    {
    case STAND_BY:

        if (ui16manualTesting == 0)
        {

            if ((CANA_tzQueryType.PSU == SET_VOLT)
                    || (CANA_tzQueryType.PSU == SET_CURR)
                    || (CANA_tzQueryType.PSU == QUERY_PROGPARAM)
                    || (CANA_tzQueryType.PSU == QUERY_OP_PARAM)
                    || (CANA_tzQueryType.PSU == TURN_ONFEC)
                    || (CANA_tzQueryType.PSU == TURN_ON)
                    || (CANA_tzQueryType.PSU == QUERY_FLTS)
                    || ((CANA_tzQueryType.PSU == QUERY_ACPARAMS))
                            && (CANB_tzSiteRxRegs.StateChngStandBy == 1))

            {

                CANA_tzQueryType.PSU = TURN_OFF;

                CANB_tzSiteRxRegs.StateChngStandBy = 0;

                CANA_tzTimerRegs.tzPSU.TxCount = 0;
            }

            else if (CANA_tzQueryType.PSU == TURN_OFF)
            {
                CANA_tzTimerRegs.tzPSU.TxCount++;

                cana_fnTurnON_PSU(0, CANA_mTURNOFF_DCDC, 1);

                if (CANA_tzTimerRegs.tzPSU.TxCount > 10)
                {
                    CANA_tzTimerRegs.tzPSU.TxCount = 0;

                    CANA_tzQueryType.PSU = IDLE_PSU;
                }

            }

            else if (CANA_tzQueryType.PSU == IDLE_PSU)
            {
                CANA_tzQueryType.PSU = SET_VOLT;
            }
        }

        break;

    case READY:

        if ((CANA_tzQueryType.PSU == QUERY_PROGPARAM)
                || (CANA_tzQueryType.PSU == TURN_OFF)
                || (CANA_tzQueryType.PSU == IDLE_PSU))

        {
            CANA_tzTimerRegs.tzPSU.TxCount++;

            cana_fnQryProgParam_PSU(CANA_tzTimerRegs.tzPSU.TxCount);

            if (CANA_tzTimerRegs.tzPSU.TxCount > CANA_mTOTAL_PSUNODE)
            {
                CANA_tzTimerRegs.tzPSU.TxCount = 0;

                CANA_tzQueryType.PSU = SET_VOLT;
            }
        }
        else if (CANA_tzQueryType.PSU == SET_VOLT)
        {
            CANA_tzTxdRegs.tzPSUData.VoltageSet = 0;

            CANA_tzTimerRegs.tzPSU.TxCount++;

            cana_fnSetVoltage_PSU(0, CANA_tzTxdRegs.tzPSUData.VoltageSet, 1);

            if (CANA_tzTimerRegs.tzPSU.TxCount > 4)
            {
                CANA_tzTimerRegs.tzPSU.TxCount = 0;

                CANA_tzQueryType.PSU = SET_CURR;

            }

        }
        else if (CANA_tzQueryType.PSU == SET_CURR)
        {
            CANA_tzTxdRegs.tzPSUData.CurrentSet = 0;

            CANA_tzTimerRegs.tzPSU.TxCount++;

            cana_fnSetCurrent_PSU(0, CANA_tzTxdRegs.tzPSUData.CurrentSet, 1);

            if (CANA_tzTimerRegs.tzPSU.TxCount > 4)
            {
                CANA_tzTimerRegs.tzPSU.TxCount = 0;

                CANA_tzQueryType.PSU = QUERY_PROGPARAM;

            }
        }

        break;

    case STACK_CHECK:

        if (CANA_tzQueryType.PSU == QUERY_PROGPARAM)
//                || (CANA_tzQueryType.PSU == SET_CURR)
//                || (CANA_tzQueryType.PSU == SET_VOLT))
        {
            CANA_tzQueryType.PSU = QUERY_ACPARAMS;
        }

        else if (CANA_tzQueryType.PSU == QUERY_ACPARAMS)
        {
            CANA_tzTimerRegs.tzPSU.TxCount++;

            cana_fnQryACParam_PSU(CANA_tzTimerRegs.tzPSU.TxCount);

            if (CANA_tzTimerRegs.tzPSU.TxCount > 20)
            {
                CANA_tzTimerRegs.tzPSU.TxCount = 0;

                CANA_tzQueryType.PSU = TURN_ONFEC;
            }
        }

        else if (CANA_tzQueryType.PSU == TURN_ONFEC)
        {
            CANA_tzTimerRegs.tzPSU.TxCount++;

            cana_fnTurnON_PSU(0, CANA_mTURNON_FEC, 1);

            if (CANA_tzTimerRegs.tzPSU.TxCount > 4)
            {
                CANA_tzQueryType.PSU = QUERY_FLTS;

                CANA_tzTimerRegs.tzPSU.TxCount = 0;
            }
        }

        else if (CANA_tzQueryType.PSU == QUERY_FLTS)
        {
            CANA_tzTimerRegs.tzPSU.TxCount++;

            cana_fnQryFaultsParam_PSU(CANA_tzTimerRegs.tzPSU.TxCount);

            if (CANA_tzTimerRegs.tzPSU.TxCount > CANA_mTOTAL_PSUNODE)
            {
                CANA_tzQueryType.PSU = SET_VOLT;

                CANA_tzTimerRegs.tzPSU.TxCount = 0;
            }
        }

        else if (CANA_tzQueryType.PSU == SET_VOLT)
        {
            CANA_tzTimerRegs.tzPSU.TxCount++;

            CANA_tzTxdRegs.tzPSUData.VoltageSet = 400.0;

            cana_fnSetVoltage_PSU(0, CANA_tzTxdRegs.tzPSUData.VoltageSet, 1);

            if (CANA_tzTimerRegs.tzPSU.TxCount > 4)
            {
                CANA_tzQueryType.PSU = SET_CURR;

                CANA_tzTimerRegs.tzPSU.TxCount = 0;
            }
        }

        else if (CANA_tzQueryType.PSU == SET_CURR)
        {

            cana_fnSetCurrent_PSU(0, CANA_tzTxdRegs.tzPSUData.CurrentSet, 1);

            CANA_tzTimerRegs.tzPSU.TxCount++;

            if (CANA_tzTimerRegs.tzPSU.TxCount > CANA_mTOTAL_PSUNODE)
            {
                CANA_tzQueryType.PSU = TURN_ON;

                CANA_tzTimerRegs.tzPSU.TxCount = 0;
            }
        }

        else if (CANA_tzQueryType.PSU == TURN_ON)
        {
            cana_fnTurnON_PSU(0, CANA_mTURNON_DCDC, 1);

            CANA_tzTimerRegs.tzPSU.TxCount++;

            if (CANA_tzTimerRegs.tzPSU.TxCount > 4)
            {
                CANA_tzQueryType.PSU = QUERY_OP_PARAM;

                CANA_tzTimerRegs.tzPSU.TxCount = 0;
            }
        }

        else if (CANA_tzQueryType.PSU == QUERY_OP_PARAM)
        {
            CANA_tzTimerRegs.tzPSU.TxCount++;

            cana_fnQryOutputParam_PSU(CANA_tzTimerRegs.tzPSU.TxCount);

            if (CANA_tzTimerRegs.tzPSU.TxCount > CANA_mTOTAL_PSUNODE)
            {
                CANA_tzTimerRegs.tzPSU.TxCount = 0;

                CANA_tzQueryType.PSU = QUERY_ACPARAMS;
            }
        }

        break;

    case STACK_POWER:

        if (CANA_tzQueryType.PSU == QUERY_OP_PARAM)
        {
            CANA_tzTimerRegs.tzPSU.TxCount++;

            cana_fnQryOutputParam_PSU(CANA_tzTimerRegs.tzPSU.TxCount);

            if (CANA_tzTimerRegs.tzPSU.TxCount > CANA_mTOTAL_PSUNODE)
            {
                CANA_tzTimerRegs.tzPSU.TxCount = 0;

                CANA_tzQueryType.PSU = SET_VOLT;
            }
        }
        else if (CANA_tzQueryType.PSU == SET_VOLT)
        {
            CANA_tzTimerRegs.tzPSU.TxCount++;

            cana_fnSetVoltage_PSU(0, CANB_tzSiteRegs.f32VoltSet, 1);

            if (CANA_tzTimerRegs.tzPSU.TxCount > 10)
            {
                CANA_tzQueryType.PSU = SET_CURR;

                CANA_tzTimerRegs.tzPSU.TxCount = 0;
            }
        }
        else if (CANA_tzQueryType.PSU == SET_CURR)
        {
            CANA_tzTimerRegs.tzPSU.TxCount++;

            cana_fnSetCurrent_PSU(0, CANA_tzTxdRegs.tzPSUData.CurrentSet, 1);

            if (CANA_tzTimerRegs.tzPSU.TxCount > 10)
            {
                CANA_tzQueryType.PSU = QUERY_ACPARAMS;

                CANA_tzTimerRegs.tzPSU.TxCount = 0;
            }
        }

        if (CANA_tzQueryType.PSU == QUERY_ACPARAMS)
        {
            CANA_tzTimerRegs.tzPSU.TxCount++;

            cana_fnQryACParam_PSU(CANA_tzTimerRegs.tzPSU.TxCount);

            if (CANA_tzTimerRegs.tzPSU.TxCount > 20)
            {
                CANA_tzTimerRegs.tzPSU.TxCount = 0;

                CANA_tzQueryType.PSU = TURN_ONFEC;
            }
        }
        else if (CANA_tzQueryType.PSU == TURN_ONFEC)
        {
            CANA_tzTimerRegs.tzPSU.TxCount++;

            cana_fnTurnON_PSU(0, CANA_mTURNON_FEC, 1);

            if (CANA_tzTimerRegs.tzPSU.TxCount > 10)
            {
                CANA_tzQueryType.PSU = TURN_ON;

                CANA_tzTimerRegs.tzPSU.TxCount = 0;
            }
        }
        else if (CANA_tzQueryType.PSU == TURN_ON)
        {
            cana_fnTurnON_PSU(0, CANA_mTURNON_DCDC, 1);

            CANA_tzTimerRegs.tzPSU.TxCount++;

            if (CANA_tzTimerRegs.tzPSU.TxCount > 10)
            {
                CANA_tzQueryType.PSU = QUERY_FLTS;

                CANA_tzTimerRegs.tzPSU.TxCount = 0;
            }
        }
        else if (CANA_tzQueryType.PSU == QUERY_FLTS)
        {
            CANA_tzTimerRegs.tzPSU.TxCount++;

            cana_fnQryFaultsParam_PSU(CANA_tzTimerRegs.tzPSU.TxCount);

            if (CANA_tzTimerRegs.tzPSU.TxCount > CANA_mTOTAL_PSUNODE)
            {
                CANA_tzTimerRegs.tzPSU.TxCount = 0;

                if (ui16ModeChange == 1)
                {
                    CANA_tzQueryType.PSU = IMPEDANCE_MODE;
                }
                else if (ui16ModeChange == 0)
                {
                    CANA_tzQueryType.PSU = QUERY_OP_PARAM;
                }
            }

        }

        else if (CANA_tzQueryType.PSU == IMPEDANCE_MODE)
        {
            CANA_tzTimerRegs.tzPSU.TxCount++;

            if (CANA_tzTimerRegs.tzPSU.TxCount < 3)
            {
                cana_fnSendImpCmds(5);
            }

            else if (CANA_tzTimerRegs.tzPSU.TxCount >= 3)
            {
                CANA_tzTimerRegs.tzPSU.TxCount = 0;
                ui16ModeChange = 0;
                CANA_tzQueryType.PSU = QUERY_OP_PARAM;
            }
        }

        break;

    case FAULT:

        if ((CANA_tzQueryType.PSU == SET_VOLT)
                || (CANA_tzQueryType.PSU == SET_CURR)
                || (CANA_tzQueryType.PSU == QUERY_PROGPARAM)
                || (CANA_tzQueryType.PSU == QUERY_OP_PARAM)
                || (CANA_tzQueryType.PSU == TURN_ONFEC)
                || (CANA_tzQueryType.PSU == TURN_ON)
                || (CANA_tzQueryType.PSU == QUERY_FLTS)
                || (CANA_tzQueryType.PSU == QUERY_ACPARAMS)
                        && (CANB_tzSiteRxRegs.StateChngFault == 1))

        {

            CANA_tzQueryType.PSU = TURN_OFF;

            CANB_tzSiteRxRegs.StateChngFault = 0;

            CANA_tzTimerRegs.tzPSU.TxCount = 0;
        }

        else if (CANA_tzQueryType.PSU == TURN_OFF)
        {
            CANA_tzTimerRegs.tzPSU.TxCount++;

            cana_fnTurnON_PSU(0, CANA_mTURNOFF_DCDC, 1);

            if (CANA_tzTimerRegs.tzPSU.TxCount > 10)
            {
                CANA_tzTimerRegs.tzPSU.TxCount = 0;

                CANA_tzQueryType.PSU = IDLE_PSU;
            }

        }

        else if (CANA_tzQueryType.PSU == IDLE_PSU)
        {
            CANA_tzQueryType.PSU = SET_VOLT;
        }

        break;

    default:
        break;

    }

}

void CANA_fnPSURX_Event(void)
{

    uint16_t ui16temp;
    if (ui16CANAPSUFailTrig1 == 1)
    {
        CANA_tzActNodeRegs_PSU.all = 0;
        CANA_tzActNode1Regs_PSU.all = 0;
        CANA_PSURegs.MsgID1 = 0x00;
        CANA_PSURegs.MsgID2 = 0x00;
        CANA_PSURegs.MsgID3 = 0x00;
        CANA_PSURegs.MsgID4 = 0x00;
        CANA_PSURegs.Node = 0;
        CANA_PSURegs.HiNode = 0;
    }

    cana_fnReadMBox_PSU();

    if (CANA_PSURegs.MBox5 == 1)  // Measured Output Parameters
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_5, ui16RxMsgDataPSU);

        ui32CANAPSUFailCnt1 = 0;

        CANA_PSURegs.MsgID1 = CanaRegs.CAN_IF2ARB.bit.ID;
        CANA_PSURegs.MsgID1 = CANA_PSURegs.MsgID1 & CANA_mNODEID_MASKPSU;
        CANA_PSURegs.Node = (Uint16) (CANA_PSURegs.MsgID1 >> 8);

        CANA_tzRxdRegs.tzPSUData.uiVoltInt[CANA_PSURegs.Node] =
                ((ui16RxMsgDataPSU[0] >> 4) * 1000)
                        + ((ui16RxMsgDataPSU[0] & 0X0F) * 100)
                        + ((ui16RxMsgDataPSU[1] >> 4) * 10)
                        + ((ui16RxMsgDataPSU[1] & 0x0F) * 1);

        CANA_tzRxdRegs.tzPSUData.uiVoltFrac[CANA_PSURegs.Node] =
                ((ui16RxMsgDataPSU[2] >> 4) * 1000)
                        + ((ui16RxMsgDataPSU[2] & 0X0F) * 100)
                        + ((ui16RxMsgDataPSU[3] >> 4) * 10)
                        + ((ui16RxMsgDataPSU[3] & 0x0F) * 1);

        CANA_tzRxdRegs.tzPSUData.f32OutVolt[CANA_PSURegs.Node] =
                CANA_tzRxdRegs.tzPSUData.uiVoltInt[CANA_PSURegs.Node]
                        + (CANA_tzRxdRegs.tzPSUData.uiVoltFrac[CANA_PSURegs.Node]
                                * 0.0001);

        CANA_tzRxdRegs.tzPSUData.uiCurrInt[CANA_PSURegs.Node] =
                ((ui16RxMsgDataPSU[4] >> 4) * 1000)
                        + ((ui16RxMsgDataPSU[4] & 0X0F) * 100)
                        + ((ui16RxMsgDataPSU[5] >> 4) * 10)
                        + ((ui16RxMsgDataPSU[5] & 0x0F) * 1);

        CANA_tzRxdRegs.tzPSUData.uiCurrFrac[CANA_PSURegs.Node] =
                ((ui16RxMsgDataPSU[6] >> 4) * 1000)
                        + ((ui16RxMsgDataPSU[6] & 0X0F) * 100)
                        + ((ui16RxMsgDataPSU[7] >> 4) * 10)
                        + ((ui16RxMsgDataPSU[7] & 0x0F) * 1);

        CANA_tzRxdRegs.tzPSUData.f32OutCurr[CANA_PSURegs.Node] =
                CANA_tzRxdRegs.tzPSUData.uiCurrInt[CANA_PSURegs.Node]
                        + (CANA_tzRxdRegs.tzPSUData.uiCurrFrac[CANA_PSURegs.Node]
                                * 0.0001);

        CANA_tzActNodeRegs_PSU.all = CANA_tzActNodeRegs_PSU.all
                | (1 << CANA_PSURegs.Node);

        if (CANA_PSURegs.Node < 16)
        {
            CANA_tzActNodeRegs_PSU.all = CANA_tzActNodeRegs_PSU.all
                    | (1 << CANA_PSURegs.Node);
        }
        else
        {
            CANA_PSURegs.HiNode = (CANA_PSURegs.Node) - 15;

            CANA_tzActNode1Regs_PSU.all = CANA_tzActNode1Regs_PSU.all
                    | (1 << CANA_PSURegs.HiNode);
        }

        for (ui16temp = 0; ui16temp < 8; ui16temp++)
        {
            ui16RxMsgDataPSU[ui16temp] = 0;
        }

        cana_fnNodeCheckLogic_PSU();

        CANA_PSURegs.MBox5 = 0;
    }

    else if (CANA_PSURegs.MBox6 == 1)  // Faults
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_6, ui16Rx1MsgDataPSU);

        ui32CANAPSUFailCnt1 = 0;

        CANA_PSURegs.MsgID2 = CanaRegs.CAN_IF2ARB.bit.ID;
        CANA_PSURegs.MsgID2 = CANA_PSURegs.MsgID2 & CANA_mNODEID_MASKPSU;
        CANA_PSURegs.Node = (Uint16) (CANA_PSURegs.MsgID2 >> 8);

        CANA_tzDCDCFaultRegs[CANA_PSURegs.Node].all = ((ui16Rx1MsgDataPSU[0]
                << 8) | (ui16Rx1MsgDataPSU[1]));

        CANA_tzFECHFaultRegs[CANA_PSURegs.Node].all = (ui16Rx1MsgDataPSU[4]);
        CANA_tzFECLFaultRegs[CANA_PSURegs.Node].all = (ui16Rx1MsgDataPSU[5]);

        CANA_tzActNodeRegs_PSU.all = CANA_tzActNodeRegs_PSU.all
                | (1 << CANA_PSURegs.Node);

        if (CANA_PSURegs.Node < 16)
        {
            CANA_tzActNodeRegs_PSU.all = CANA_tzActNodeRegs_PSU.all
                    | (1 << CANA_PSURegs.Node);
        }
        else
        {
            CANA_PSURegs.HiNode = (CANA_PSURegs.Node) - 15;

            CANA_tzActNode1Regs_PSU.all = CANA_tzActNode1Regs_PSU.all
                    | (1 << CANA_PSURegs.HiNode);
        }

        for (ui16temp = 0; ui16temp < 8; ui16temp++)
        {
            ui16RxMsgDataPSU[ui16temp] = 0;
        }

        cana_fnNodeCheckLogic_PSU();

        CANA_PSURegs.MBox6 = 0;

    }

    else if (CANA_PSURegs.MBox7 == 1)  // Faults
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_7, ui16Rx2MsgDataPSU);

        ui32CANAPSUFailCnt1 = 0;

        CANA_PSURegs.MsgID3 = CanaRegs.CAN_IF2ARB.bit.ID;
        CANA_PSURegs.MsgID3 = CANA_PSURegs.MsgID3 & CANA_mNODEID_MASKPSU;
        CANA_PSURegs.Node = (Uint16) (CANA_PSURegs.MsgID3 >> 8);

        CANA_tzRxdRegs.tzPSUData.uiProgVoltInt[CANA_PSURegs.Node] =
                ((ui16Rx2MsgDataPSU[0] >> 4) * 1000)
                        + ((ui16Rx2MsgDataPSU[0] & 0X0F) * 100)
                        + ((ui16Rx2MsgDataPSU[1] >> 4) * 10)
                        + ((ui16Rx2MsgDataPSU[1] & 0x0F) * 1);

        CANA_tzRxdRegs.tzPSUData.uiProgVoltFrac[CANA_PSURegs.Node] =
                ((ui16Rx2MsgDataPSU[2] >> 4) * 1000)
                        + ((ui16Rx2MsgDataPSU[2] & 0X0F) * 100)
                        + ((ui16Rx2MsgDataPSU[3] >> 4) * 10)
                        + ((ui16Rx2MsgDataPSU[3] & 0x0F) * 1);

        CANA_tzRxdRegs.tzPSUData.f32ProgVolt[CANA_PSURegs.Node] =
                CANA_tzRxdRegs.tzPSUData.uiProgVoltInt[CANA_PSURegs.Node]
                        + (CANA_tzRxdRegs.tzPSUData.uiProgVoltFrac[CANA_PSURegs.Node]
                                * 0.0001);

        CANA_tzRxdRegs.tzPSUData.uiProgCurrInt[CANA_PSURegs.Node] =
                ((ui16Rx2MsgDataPSU[4] >> 4) * 1000)
                        + ((ui16Rx2MsgDataPSU[4] & 0X0F) * 100)
                        + ((ui16Rx2MsgDataPSU[5] >> 4) * 10)
                        + ((ui16Rx2MsgDataPSU[5] & 0x0F) * 1);

        CANA_tzRxdRegs.tzPSUData.uiProgCurrFrac[CANA_PSURegs.Node] =
                ((ui16Rx2MsgDataPSU[6] >> 4) * 1000)
                        + ((ui16Rx2MsgDataPSU[6] & 0X0F) * 100)
                        + ((ui16Rx2MsgDataPSU[7] >> 4) * 10)
                        + ((ui16Rx2MsgDataPSU[7] & 0x0F) * 1);

        CANA_tzRxdRegs.tzPSUData.f32ProgCurr[CANA_PSURegs.Node] =
                CANA_tzRxdRegs.tzPSUData.uiProgCurrInt[CANA_PSURegs.Node]
                        + (CANA_tzRxdRegs.tzPSUData.uiProgCurrFrac[CANA_PSURegs.Node]
                                * 0.0001);

        CANA_tzActNodeRegs_PSU.all = CANA_tzActNodeRegs_PSU.all
                | (1 << CANA_PSURegs.Node);

        if (CANA_PSURegs.Node < 16)
        {
            CANA_tzActNodeRegs_PSU.all = CANA_tzActNodeRegs_PSU.all
                    | (1 << CANA_PSURegs.Node);
        }
        else
        {
            CANA_PSURegs.HiNode = (CANA_PSURegs.Node) - 15;

            CANA_tzActNode1Regs_PSU.all = CANA_tzActNode1Regs_PSU.all
                    | (1 << CANA_PSURegs.HiNode);
        }

        for (ui16temp = 0; ui16temp < 8; ui16temp++)
        {
            ui16RxMsgDataPSU[ui16temp] = 0;
        }

        cana_fnNodeCheckLogic_PSU();

        CANA_PSURegs.MBox7 = 0;

    }

    else if (CANA_PSURegs.MBox8 == 1)  // AC parameters
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_8, ui16Rx3MsgDataPSU);

        ui32CANAPSUFailCnt1 = 0;

        CANA_PSURegs.MsgID4 = CanaRegs.CAN_IF2ARB.bit.ID;
        CANA_PSURegs.MsgID4 = CANA_PSURegs.MsgID4 & CANA_mNODEID_MASKPSU;
        CANA_PSURegs.Node = (Uint16) (CANA_PSURegs.MsgID4 >> 8);

        CANA_tzRxdRegs.tzPSUData.ACparamFreq[CANA_PSURegs.Node] =
                (((ui16Rx3MsgDataPSU[0] << 8) + (ui16Rx3MsgDataPSU[1])) * 0.1);

        CANA_tzRxdRegs.tzPSUData.AClineVolt[CANA_PSURegs.Node] =
                ((ui16Rx3MsgDataPSU[2] << 8) + (ui16Rx3MsgDataPSU[3]));

        CANA_tzRxdRegs.tzPSUData.AClineCurr[CANA_PSURegs.Node] =
                ((ui16Rx3MsgDataPSU[4] << 8) + (ui16Rx3MsgDataPSU[5])) * 0.1;

        CANA_tzActNodeRegs_PSU.all = CANA_tzActNodeRegs_PSU.all
                | (1 << CANA_PSURegs.Node);

        if (CANA_PSURegs.Node < 16)
        {
            CANA_tzActNodeRegs_PSU.all = CANA_tzActNodeRegs_PSU.all
                    | (1 << CANA_PSURegs.Node);
        }
        else
        {
            CANA_PSURegs.HiNode = (CANA_PSURegs.Node) - 15;

            CANA_tzActNode1Regs_PSU.all = CANA_tzActNode1Regs_PSU.all
                    | (1 << CANA_PSURegs.HiNode);
        }

        for (ui16temp = 0; ui16temp < 8; ui16temp++)
        {
            ui16RxMsgDataPSU[ui16temp] = 0;
        }

        cana_fnNodeCheckLogic_PSU();

        CANA_PSURegs.MBox8 = 0;

    }

    cana_fnTimeoutFaultsLogic_PSU();

    canb_fnParamsUpdate();

}

void cana_fnNodeCheckLogic_PSU()
{
    if ((CANA_PSURegs.Node == 1) && (CANA_tzDCDCFaultRegs[1].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[1] = 0;
    }

    else if ((CANA_PSURegs.Node == 2) && (CANA_tzDCDCFaultRegs[2].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[2] = 0;
    }

    else if ((CANA_PSURegs.Node == 3) && (CANA_tzDCDCFaultRegs[3].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[3] = 0;
    }

    else if ((CANA_PSURegs.Node == 4) && (CANA_tzDCDCFaultRegs[4].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[4] = 0;
    }

    else if ((CANA_PSURegs.Node == 5) && (CANA_tzDCDCFaultRegs[5].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[5] = 0;
    }

    else if ((CANA_PSURegs.Node == 6) && (CANA_tzDCDCFaultRegs[6].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[6] = 0;
    }

    else if ((CANA_PSURegs.Node == 7) && (CANA_tzDCDCFaultRegs[7].all == 0x0))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[7] = 0;
    }

    else if ((CANA_PSURegs.Node == 8) && (CANA_tzDCDCFaultRegs[8].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[8] = 0;
    }

    else if ((CANA_PSURegs.Node == 9) && (CANA_tzDCDCFaultRegs[9].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[9] = 0;
    }

    else if ((CANA_PSURegs.Node == 10) && (CANA_tzDCDCFaultRegs[10].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[10] = 0;
    }

    else if ((CANA_PSURegs.Node == 11) && (CANA_tzDCDCFaultRegs[11].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[11] = 0;
    }

    else if ((CANA_PSURegs.Node == 12) && (CANA_tzDCDCFaultRegs[12].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[12] = 0;
    }

    else if ((CANA_PSURegs.Node == 13) && (CANA_tzDCDCFaultRegs[13].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[13] = 0;
    }

    else if ((CANA_PSURegs.Node == 14) && (CANA_tzDCDCFaultRegs[14].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[14] = 0;
    }

    else if ((CANA_PSURegs.Node == 15) && (CANA_tzDCDCFaultRegs[15].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[15] = 0;
    }

    else if ((CANA_PSURegs.Node == 16) && (CANA_tzDCDCFaultRegs[16].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[16] = 0;
    }

    else if ((CANA_PSURegs.Node == 17) && (CANA_tzDCDCFaultRegs[17].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[17] = 0;
    }

    else if ((CANA_PSURegs.Node == 18) && (CANA_tzDCDCFaultRegs[18].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[18] = 0;
    }

    else if ((CANA_PSURegs.Node == 19) && (CANA_tzDCDCFaultRegs[19].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[19] = 0;
    }

    else if ((CANA_PSURegs.Node == 20) && (CANA_tzDCDCFaultRegs[20].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[20] = 0;
    }

    else if ((CANA_PSURegs.Node == 21) && (CANA_tzDCDCFaultRegs[21].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[21] = 0;
    }

    else if ((CANA_PSURegs.Node == 22) && (CANA_tzDCDCFaultRegs[22].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[22] = 0;
    }

    else if ((CANA_PSURegs.Node == 23) && (CANA_tzDCDCFaultRegs[23].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[23] = 0;
    }

    else if ((CANA_PSURegs.Node == 24) && (CANA_tzDCDCFaultRegs[24].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[24] = 0;
    }

    else if ((CANA_PSURegs.Node == 25) && (CANA_tzDCDCFaultRegs[25].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[25] = 0;
    }

    else if ((CANA_PSURegs.Node == 26) && (CANA_tzDCDCFaultRegs[26].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[26] = 0;
    }

    else if ((CANA_PSURegs.Node == 27) && (CANA_tzDCDCFaultRegs[27].all == 0x0))

    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[27] = 0;
    }

}

void cana_fnTimeoutFaultsLogic_PSU(void)
{
    if ((CANA_PSURegs.Node != 1) || ((CANA_tzDCDCFaultRegs[1].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[1]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[1] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[1] = CANA_mPSUTIMEOUT;
            CANA_tzActNodeRegs_PSU.bit.bt_node1 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[1] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[1] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[1] = 0;
//            CANA_tzRxdRegs.tzPSUData.uiCurrInt[1] = 0;
//            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[1] = 0;
//            CANA_tzRxdRegs.tzPSUData.f32OutCurr[1] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 2) || ((CANA_tzDCDCFaultRegs[2].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[2]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[2] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[2] = CANA_mPSUTIMEOUT;
            CANA_tzActNodeRegs_PSU.bit.bt_node2 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[2] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[2] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[2] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[2] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[2] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[2] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 3) || ((CANA_tzDCDCFaultRegs[3].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[3]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[3] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[3] = CANA_mPSUTIMEOUT;
            CANA_tzActNodeRegs_PSU.bit.bt_node3 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[3] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[3] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[3] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[3] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[3] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[3] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 4) || ((CANA_tzDCDCFaultRegs[4].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[4]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[4] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[4] = CANA_mPSUTIMEOUT;
            CANA_tzActNodeRegs_PSU.bit.bt_node4 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[4] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[4] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[4] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[4] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[4] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[4] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 5) || ((CANA_tzDCDCFaultRegs[5].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[5]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[5] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[5] = CANA_mPSUTIMEOUT;
            CANA_tzActNodeRegs_PSU.bit.bt_node5 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[5] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[5] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[5] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[5] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[5] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[5] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 6) || ((CANA_tzDCDCFaultRegs[6].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[6]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[6] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[6] = CANA_mPSUTIMEOUT;
            CANA_tzActNodeRegs_PSU.bit.bt_node6 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[6] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[6] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[6] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[6] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[6] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[6] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 7) || ((CANA_tzDCDCFaultRegs[7].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[7]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[7] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[7] = CANA_mPSUTIMEOUT;
            CANA_tzActNodeRegs_PSU.bit.bt_node7 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[7] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[7] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[7] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[7] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[7] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[7] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 8) || ((CANA_tzDCDCFaultRegs[8].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[8]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[8] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[8] = CANA_mPSUTIMEOUT;
            CANA_tzActNodeRegs_PSU.bit.bt_node8 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[8] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[8] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[8] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[8] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[8] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[8] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 9) || ((CANA_tzDCDCFaultRegs[9].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[9]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[9] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[9] = CANA_mPSUTIMEOUT;
            CANA_tzActNodeRegs_PSU.bit.bt_node9 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[9] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[9] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[9] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[9] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[9] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[9] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 10) || ((CANA_tzDCDCFaultRegs[10].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[10]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[10] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[10] = CANA_mPSUTIMEOUT;
            CANA_tzActNodeRegs_PSU.bit.bt_node10 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[10] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[10] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[10] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[10] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[10] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[10] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 11) || ((CANA_tzDCDCFaultRegs[11].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[11]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[11] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[11] = CANA_mPSUTIMEOUT;
            CANA_tzActNodeRegs_PSU.bit.bt_node11 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[11] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[11] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[11] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[11] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[11] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[11] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 12) || ((CANA_tzDCDCFaultRegs[12].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[12]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[12] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[12] = CANA_mPSUTIMEOUT;
            CANA_tzActNodeRegs_PSU.bit.bt_node12 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[12] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[12] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[12] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[12] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[12] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[12] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 13) || ((CANA_tzDCDCFaultRegs[13].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[13]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[13] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[13] = CANA_mPSUTIMEOUT;
            CANA_tzActNodeRegs_PSU.bit.bt_node13 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[13] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[13] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[13] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[13] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[13] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[13] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 14) || ((CANA_tzDCDCFaultRegs[14].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[14]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[14] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[14] = CANA_mPSUTIMEOUT;
            CANA_tzActNodeRegs_PSU.bit.bt_node14 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[14] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[14] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[14] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[14] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[14] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[14] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 15) || ((CANA_tzDCDCFaultRegs[15].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[15]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[15] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[15] = CANA_mPSUTIMEOUT;
            CANA_tzActNodeRegs_PSU.bit.bt_node15 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[15] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[15] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[15] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[15] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[15] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[15] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 16) || ((CANA_tzDCDCFaultRegs[16].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[16]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[16] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[16] = CANA_mPSUTIMEOUT;
            CANA_tzActNode1Regs_PSU.bit.bt_node16 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[16] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[16] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[16] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[16] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[16] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[16] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 17) || ((CANA_tzDCDCFaultRegs[17].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[17]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[17] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[17] = CANA_mPSUTIMEOUT;
            CANA_tzActNode1Regs_PSU.bit.bt_node17 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[17] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[17] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[17] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[17] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[17] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[17] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 18) || ((CANA_tzDCDCFaultRegs[18].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[18]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[18] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[18] = CANA_mPSUTIMEOUT;
            CANA_tzActNode1Regs_PSU.bit.bt_node18 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[18] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[18] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[18] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[18] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[18] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[18] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 19) || ((CANA_tzDCDCFaultRegs[19].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[19]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[19] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[19] = CANA_mPSUTIMEOUT;
            CANA_tzActNode1Regs_PSU.bit.bt_node19 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[19] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[19] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[19] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[19] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[19] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[19] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 20) || ((CANA_tzDCDCFaultRegs[20].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[20]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[20] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[20] = CANA_mPSUTIMEOUT;
            CANA_tzActNode1Regs_PSU.bit.bt_node20 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[20] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[20] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[20] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[20] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[20] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[20] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 21) || ((CANA_tzDCDCFaultRegs[21].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[21]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[21] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[21] = CANA_mPSUTIMEOUT;
            CANA_tzActNode1Regs_PSU.bit.bt_node21 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[21] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[21] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[21] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[21] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[21] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[21] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 22) || ((CANA_tzDCDCFaultRegs[22].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[22]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[22] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[22] = CANA_mPSUTIMEOUT;
            CANA_tzActNode1Regs_PSU.bit.bt_node22 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[22] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[22] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[22] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[22] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[22] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[22] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 23) || ((CANA_tzDCDCFaultRegs[23].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[23]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[23] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[23] = CANA_mPSUTIMEOUT;
            CANA_tzActNode1Regs_PSU.bit.bt_node23 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[23] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[23] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[23] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[23] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[23] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[23] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 24) || ((CANA_tzDCDCFaultRegs[24].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[24]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[24] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[24] = CANA_mPSUTIMEOUT;
            CANA_tzActNode1Regs_PSU.bit.bt_node24 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[24] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[24] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[24] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[24] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[24] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[24] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 25) || ((CANA_tzDCDCFaultRegs[25].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[25]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[25] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[25] = CANA_mPSUTIMEOUT;
            CANA_tzActNode1Regs_PSU.bit.bt_node25 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[25] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[25] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[25] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[25] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[25] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[25] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 26) || ((CANA_tzDCDCFaultRegs[26].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[26]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[26] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[26] = CANA_mPSUTIMEOUT;
            CANA_tzActNode1Regs_PSU.bit.bt_node26 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[26] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[26] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[26] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[26] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[26] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[26] = 0;

        }
    }

    if ((CANA_PSURegs.Node != 27) || ((CANA_tzDCDCFaultRegs[27].all >= 0x0001)))
    {
        CANA_tzTimerRegs.tzPSU.CANfailCnt[27]++;
        if (CANA_tzTimerRegs.tzPSU.CANfailCnt[27] >= CANA_mPSUTIMEOUT)
        {
            CANA_tzTimerRegs.tzPSU.CANfailCnt[27] = CANA_mPSUTIMEOUT;
            CANA_tzActNode1Regs_PSU.bit.bt_node27 = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltInt[27] = 0;
            CANA_tzRxdRegs.tzPSUData.uiVoltFrac[27] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutVolt[27] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrInt[27] = 0;
            CANA_tzRxdRegs.tzPSUData.uiCurrFrac[27] = 0;
            CANA_tzRxdRegs.tzPSUData.f32OutCurr[27] = 0;
        }
    }

}

void cana_fnTurnON_PSU(int32 i32nodeID, int32 i32ON_command, int32 i32global)
{
    if (i32global == 1)
    {

        CAN_setupMessageObject(CANA_BASE, CAN_mMAILBOX_2,
        CANA_mMSGID_GLOBAL_COMMAND_TO_TURNON_PSU,
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF, CAN_MSG_OBJ_NO_FLAGS,
                               CANA_mTWO_BYTE);

        if (i32ON_command == CANA_mTURNON_DCDC)
        {
            ui16txMsgDataPSU[0] = 0xAA;
            ui16txMsgDataPSU[1] = 0xAA;
        }
        else if (i32ON_command == CANA_mTURNOFF_DCDC)
        {
            ui16txMsgDataPSU[0] = 0x00;
            ui16txMsgDataPSU[1] = 0x00;
        }
        else if (i32ON_command == CANA_mTURNON_FEC)
        {
            ui16txMsgDataPSU[0] = 0xBB;
            ui16txMsgDataPSU[1] = 0xBB;
        }

        CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_2, CANA_mTWO_BYTE,
                        ui16txMsgDataPSU);
    }

}

void cana_fnQryProgParam_PSU(int32 i32nodeID)
{
    CAN_setupMessageObject(
            CANA_BASE, CAN_mMAILBOX_2,
            (i32nodeID << 8) | CANA_mMSGID_QRY_MODELSPC_PROGPARAM_PSU,
            CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
            CAN_MSG_OBJ_NO_FLAGS,
            CANA_mTWO_BYTE);

    ui16txMsgDataPSU[0] = 0x61;
    ui16txMsgDataPSU[1] = 0x61;

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_2, CANA_mTWO_BYTE,
                    ui16txMsgDataPSU);

}

void cana_fnQryOutputParam_PSU(int32 i32nodeID)
{
    CAN_setupMessageObject(
            CANA_BASE, CAN_mMAILBOX_2,
            (i32nodeID << 8) | CANA_mMSGID_QRY_MODELSPC_MEASPARAM_PSU,
            CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
            CAN_MSG_OBJ_NO_FLAGS,
            CANA_mTWO_BYTE);

    ui16txMsgDataPSU[0] = 0x62;
    ui16txMsgDataPSU[1] = 0x62;

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_2, CANA_mTWO_BYTE,
                    ui16txMsgDataPSU);

}

void cana_fnQryACParam_PSU(int32 i32nodeID)
{
    CAN_setupMessageObject(
            CANA_BASE, CAN_mMAILBOX_2,
            (i32nodeID << 8) | CANA_mMSGID_QRY_MODELSPC_ACPARAM_PSU,
            CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
            CAN_MSG_OBJ_NO_FLAGS,
            CANA_mTWO_BYTE);

    ui16txMsgDataPSU[0] = 0x72;
    ui16txMsgDataPSU[1] = 0x72;

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_2, CANA_mTWO_BYTE,
                    ui16txMsgDataPSU);

}

void cana_fnQryFaultsParam_PSU(int32 i32nodeID)
{
    CAN_setupMessageObject(
            CANA_BASE, CAN_mMAILBOX_2,
            (i32nodeID << 8) | CANA_mMSGID_QRY_MODELSPC_FAULTS_PSU,
            CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
            CAN_MSG_OBJ_NO_FLAGS,
            CANA_mTWO_BYTE);

    ui16txMsgDataPSU[0] = 0x6C;
    ui16txMsgDataPSU[1] = 0x6C;

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_2, CANA_mTWO_BYTE,
                    ui16txMsgDataPSU);

}

void cana_fnSetVoltage_PSU(int32 i32nodeID, float32 f32voltageValue,
                           Uint16 ui16global)
{
    int32_t i32tempInt[8], i32tempFrac[8];

    if (ui16global == 1)
    {
        CAN_setupMessageObject(CANA_BASE, CAN_mMAILBOX_2,
        CANA_mMSGID_GLOBAL_COMMAND_TO_VOLTSET_PSU,
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CANA_mFOUR_BYTE);
    }

    i32tempInt[0] = (int32) (f32voltageValue);
    i32tempFrac[0] = (int32) ((f32voltageValue - i32tempInt[0]) * 10000);

    ui16txMsgDataPSU[0] = (((i32tempInt[0] / 1000) & 0xF) << 4)
            | (((i32tempInt[0] % 1000) / 100) & 0xF);
    ui16txMsgDataPSU[1] = ((((i32tempInt[0] % 100) / 10) & 0xF) << 4)
            | (((i32tempInt[0] % 10) / 1) & 0xF);

    ui16txMsgDataPSU[2] = (((i32tempFrac[0] / 1000) & 0xF) << 4)
            | (((i32tempFrac[0] % 1000) / 100) & 0xF);
    ui16txMsgDataPSU[3] = ((((i32tempFrac[0] % 100) / 10) & 0xF) << 4)
            | (((i32tempFrac[0] % 10) / 1) & 0xF);

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_2, CANA_mFOUR_BYTE,
                    ui16txMsgDataPSU);
}

void cana_fnSetCurrent_PSU(int32 i32nodeID, float32 f32currentValue,
                           Uint16 ui16global)
{
    int32_t i32tempInt[8], i32tempFrac[8];

    if (ui16global == 1)
    {
        CAN_setupMessageObject(CANA_BASE, CAN_mMAILBOX_2,
        CANA_mMSGID_GLOBAL_COMMAND_TO_CURRSET_PSU,
                               CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX,
                               0x1FFFFFFF,
                               CAN_MSG_OBJ_NO_FLAGS,
                               CANA_mFOUR_BYTE);
    }

    i32tempInt[0] = (int32) (f32currentValue);
    i32tempFrac[0] = (int32) ((f32currentValue - i32tempInt[0]) * 10000);

    ui16txMsgDataPSU[0] = (((i32tempInt[0] / 1000) & 0xF) << 4)
            | (((i32tempInt[0] % 1000) / 100) & 0xF);
    ui16txMsgDataPSU[1] = ((((i32tempInt[0] % 100) / 10) & 0xF) << 4)
            | (((i32tempInt[0] % 10) / 1) & 0xF);

    ui16txMsgDataPSU[2] = (((i32tempFrac[0] / 1000) & 0xF) << 4)
            | (((i32tempFrac[0] % 1000) / 100) & 0xF);
    ui16txMsgDataPSU[3] = ((((i32tempFrac[0] % 100) / 10) & 0xF) << 4)
            | (((i32tempFrac[0] % 10) / 1) & 0xF);

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_2, CANA_mFOUR_BYTE,
                    ui16txMsgDataPSU);
}

void cana_fnSendImpCmds(int32 i32nodeID)
{
    CAN_setupMessageObject(
            CANA_BASE, CAN_mMAILBOX_2,
            (i32nodeID << 8) | CANA_mMSGID_QRY_MODELSPC_IMPSET_PSU,
            CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
            CAN_MSG_OBJ_NO_FLAGS,
            CANA_mFOUR_BYTE);

    ui32tempImp[0] = (int32) (CANB_tzSiteRxRegs.ImpAmpVar);

    ui16txMsgDataPSU[0] = (((ui32tempImp[0] / 1000) & 0xF) << 4)
            | (((ui32tempImp[0] % 1000) / 100) & 0xF);
    ui16txMsgDataPSU[1] = ((((ui32tempImp[0] % 100) / 10) & 0xF) << 4)
            | (((ui32tempImp[0] % 10) / 1) & 0xF);

    ui32tempImp[1] = (int32) (CANB_tzSiteRxRegs.ImpFreqVar);

    ui16txMsgDataPSU[2] = (((ui32tempImp[1] / 1000) & 0xF) << 4)
            | (((ui32tempImp[1] % 1000) / 100) & 0xF);
    ui16txMsgDataPSU[3] = ((((ui32tempImp[1] % 100) / 10) & 0xF) << 4)
            | (((ui32tempImp[1] % 10) / 1) & 0xF);

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_2, CANA_mFOUR_BYTE,
                    ui16txMsgDataPSU);

}

void cana_fnReadMBox_PSU(void)
{
    CANA_PSURegs.MBox5 = CAN_readMessage(CANA_BASE, CAN_mMAILBOX_5,
                                         ui16RxMsgDataPSU); // Measured Parameters
    CANA_PSURegs.MBox6 = CAN_readMessage(CANA_BASE, CAN_mMAILBOX_6,
                                         ui16Rx1MsgDataPSU); // Faults
    CANA_PSURegs.MBox7 = CAN_readMessage(CANA_BASE, CAN_mMAILBOX_7,
                                         ui16Rx2MsgDataPSU); // Programmed Parameters
    CANA_PSURegs.MBox8 = CAN_readMessage(CANA_BASE, CAN_mMAILBOX_8,
                                         ui16Rx3MsgDataPSU); // Programmed Parameters

}

/*==============================================================================
 End of File
 ==============================================================================*/
