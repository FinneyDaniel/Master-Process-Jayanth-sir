/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  oi_psurefcurset.c
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
#include "cana_PSUCom.h"
#include "scheduler.h"
#include "canb_defs.h"
#include "mathcalc.h"
#include "control_defs.h"
/*==============================================================================
 Defines
 ==============================================================================*/

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

union CONTROL_FLTS_REG CONTROLtzFaultRegs;

/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void CONTROL_fnPSU_IRef(void);
void control_fncellmaxval(void);

/*==============================================================================
 Local Variables
 ==============================================================================*/

uint32_t ui32NodePresent = 0;
uint16_t ui16SafeShutDownFlg = 0;
uint16_t ui16InstShutDownFlg = 0;
uint16_t testflag = 1;
uint32_t ui32WaterOkCnt = 0, ui32WaterNotOkCnt = 0;
uint32_t ui32CellVoltTripCnt = 0, ui32stackCellNotokcnt = 0,
        ui32stackCellokcnt = 0;

int16_t n1 = 12, i, j, k1;
int16_t n2 = 12;
int16_t n3 = 25; //Array Size Declaration
float32_t c[26]; //Array Declaration
float32_t f32maxCellVolt;
/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void CONTROL_fnPSU_IRef(void)
{

//    control_fncellmaxval();
//
//    if (f32maxCellVolt > CANB_tzSiteRxRegs.CellMaxLimit)
//    {
//        CANA_tzIOFlags.CellFault = 1;
//    }
//    else if (f32maxCellVolt < CANB_tzSiteRxRegs.CellRecvryLimit)
//    {
//        CANA_tzIOFlags.CellFault = 0;
//    }

    // Receiving Count value in seconds => 1 count = 60msec, ....cnts = 1sec => 1/60ms = 16.667

//    ui32CellVoltTripCnt = (ceil)(CANB_tzSiteRxRegs.CellNotokTripCnt * 16.667);
//
//    if (CANA_tzIOFlags.CellFault == 1)
//    {
//        ui32stackCellokcnt = 0;
//        ui32stackCellNotokcnt++;
//        if (ui32stackCellNotokcnt >= ui32CellVoltTripCnt)
//        {
//            ui32stackCellNotokcnt = ui32CellVoltTripCnt;
//            CONTROLtzFaultRegs.bit.StackcellFault = 0;
//        }
//    }
//    else if (CANA_tzIOFlags.CellFault == 0)
//    {
//        ui32stackCellNotokcnt = 0;
//        ui32stackCellokcnt++;
//        if (ui32stackCellokcnt >= ui32CellVoltTripCnt)
//        {
//            ui32stackCellokcnt = ui32CellVoltTripCnt;
//            CONTROLtzFaultRegs.bit.StackcellFault = 0;
//        }
//    }



    if (MATHConvtzRegs.AISensorPRT101 < 0.0f)
    {
        MATHConvtzRegs.AISensorPRT101 = 0;
    }

    if (MATHConvtzRegs.AISensorPRT401 < 0.0f)
    {
        MATHConvtzRegs.AISensorPRT401 = 0;
    }

    if (MATHConvtzRegs.AISensorPRT402 < 0.0f)
    {
        MATHConvtzRegs.AISensorPRT402 = 0;
    }


//    if (CANA_tzIOFlags.WaterLevelOk == 0)
//    {
//        ui32WaterOkCnt = 0;
//        ui32WaterNotOkCnt++;
//        if (ui32WaterNotOkCnt > 100)
//        {
//            ui32WaterNotOkCnt = 100;
//            CONTROLtzFaultRegs.bit.WaterlvlsFail = 1;
//        }
//
//    }
//    else
//    {
//        ui32WaterNotOkCnt = 0;
//        ui32WaterOkCnt++;
//        if (ui32WaterOkCnt > 100)
//        {
//            ui32WaterOkCnt = 100;
//            CONTROLtzFaultRegs.bit.WaterlvlsFail = 0;
//        }
//    }

    ui32NodePresent = CANA_tzActNodeRegs_PSU.bit.bt_node1
            + CANA_tzActNodeRegs_PSU.bit.bt_node2
            + CANA_tzActNodeRegs_PSU.bit.bt_node3
            + CANA_tzActNodeRegs_PSU.bit.bt_node4
            + CANA_tzActNodeRegs_PSU.bit.bt_node5
            + CANA_tzActNodeRegs_PSU.bit.bt_node6
            + CANA_tzActNodeRegs_PSU.bit.bt_node7
            + CANA_tzActNodeRegs_PSU.bit.bt_node8
            + CANA_tzActNodeRegs_PSU.bit.bt_node9
            + CANA_tzActNodeRegs_PSU.bit.bt_node10
            + CANA_tzActNodeRegs_PSU.bit.bt_node11
            + CANA_tzActNodeRegs_PSU.bit.bt_node12
            + CANA_tzActNodeRegs_PSU.bit.bt_node13
            + CANA_tzActNodeRegs_PSU.bit.bt_node14
            + CANA_tzActNodeRegs_PSU.bit.bt_node15
            + CANA_tzActNode1Regs_PSU.bit.bt_node16
            + CANA_tzActNode1Regs_PSU.bit.bt_node17
            + CANA_tzActNode1Regs_PSU.bit.bt_node18
            + CANA_tzActNode1Regs_PSU.bit.bt_node19
            + CANA_tzActNode1Regs_PSU.bit.bt_node20
            + CANA_tzActNode1Regs_PSU.bit.bt_node21
            + CANA_tzActNode1Regs_PSU.bit.bt_node22
            + CANA_tzActNode1Regs_PSU.bit.bt_node23
            + CANA_tzActNode1Regs_PSU.bit.bt_node24
            + CANA_tzActNode1Regs_PSU.bit.bt_node25
            + CANA_tzActNode1Regs_PSU.bit.bt_node26
            + CANA_tzActNode1Regs_PSU.bit.bt_node27;

    if (ui32NodePresent == 0)
    {
        CANA_tzTxdRegs.tzPSUData.CurrentSet = 0;
    }
    else
    {
        if ((STAT_tzStateMac.Present_st == STAND_BY) || (STAT_tzStateMac.Present_st == READY) || (STAT_tzStateMac.Present_st == STACK_CHECK))
        {
            CANA_tzTxdRegs.tzPSUData.TotalCurrentSet = 0;
        }

        else if (STAT_tzStateMac.Present_st == STACK_POWER)
        {

            if ((CANB_tzSiteRxRegs.Start_H2Cmd == 1)
//                    && (CANA_tzLHC2AIFaultRegs.bit.COS_101 == 0)
//                    && (CONTROLtzFaultRegs.bit.WaterlvlsFail == 0)
//                    && (CANA_tzLHC1AIFaultRegs.bit.PRT_101 == 0)
//
//                    && (CANA_tzLHC1AIFaultRegs.bit.PRT_401 == 0)
//                    && (CANA_tzLHC2AIFaultRegs.bit.PRT_102 == 0)
//                    && (CANA_tzThermalFaultRegs.bit.TTC_101 == 0)
//                    && (CANA_tzThermalFaultRegs.bit.TTC_102 == 0)
//                    && (CANA_tzThermalFaultRegs.bit.KTC_401 == 0)
//                    && (CONTROLtzFaultRegs.bit.StackcellFault == 0)
//                    && (CANA_tzIOFlags.hydrogenValuesOK == 1)
//                    && (CANA_tzLPCAIFaultRegs.bit.HYS_102_ShtDwn == 0)
//                    && (CONTROLtzFaultRegs.bit.VFDPumpFail == 0)
//                    && (CANA_tzIOFlags.WLSNoleak == 1)
//                    && (CANA_tzLHC2AIFaultRegs.bit.RTD_401 == 0)
//                    && (CANA_tzThermalFaultRegs.bit.TTC_301 == 0)
//                    && (CANA_tzLPCAIFaultRegs.bit.OXS_101_ShtDwn == 0)
//                    && (CANA_tzLPCAIFaultRegs.bit.OXS_101_RmpDwn == 0)
                    )

            {
                CANA_tzTimerRegs.tzPSU.CurRampDowncnt = 0;
                CANA_tzTimerRegs.tzPSU.InstShutDowncnt = 0;

                ui16SafeShutDownFlg = 0;
                ui16InstShutDownFlg = 0;

                CANA_tzTimerRegs.tzPSU.CurRampUpcnt++;

                CANA_tzTxdRegs.tzPSUData.TotalCurrentSet =
                        CANB_tzSiteRxRegs.f32CurrSet;

                CANA_tzTxdRegs.tzPSUData.TotalISetTemp =
                        (CANA_tzTxdRegs.tzPSUData.TotalCurrentSet
                                / ui32NodePresent);

                if (CANA_tzTimerRegs.tzPSU.CurRampUpcnt >= 90)
                {
                    CANA_tzTxdRegs.tzPSUData.CurrentSet += 2;

                    CANA_tzTimerRegs.tzPSU.CurRampUpcnt = 0;
                }

                if (CANA_tzTxdRegs.tzPSUData.CurrentSet
                        >= CANA_tzTxdRegs.tzPSUData.TotalISetTemp)
                {
                    CANA_tzTxdRegs.tzPSUData.CurrentSet =
                            CANA_tzTxdRegs.tzPSUData.TotalISetTemp;
                }

                CONTROLtzFaultRegs.bit.LPCCurHealthy = 0;

            }

            else
            {
                if ((CANB_tzSiteRxRegs.Start_cmd == 0)
                        || (CANB_tzSiteRxRegs.Start_H2Cmd == 0))
//                        || (CANA_tzActNodeRegs_IO.bit.bt_node1 == 0)
//                        || (CANA_tzActNodeRegs_IO.bit.bt_node2 == 0)
//                        || (CANA_tzActNodeRegs_IO.bit.bt_node3 == 0)
//                        || (CANA_tzActNodeRegs_IO.bit.bt_node4 == 0)
//                        || (CANA_tzLHC2AIFaultRegs.bit.COS_101 == 1)
//                        || (CONTROLtzFaultRegs.bit.WaterlvlsFail == 1)
//                        || (CANA_tzLHC1AIFaultRegs.bit.PRT_401 == 1)
//                        || (CANA_tzLHC1AIFaultRegs.bit.PRT_101 == 1)
//                        || (CANA_tzLHC2AIFaultRegs.bit.PRT_102 == 1)
//                        || (CANA_tzThermalFaultRegs.bit.TTC_101 == 1)
//                        || (CANA_tzThermalFaultRegs.bit.TTC_102 == 1)
//                        || (CANA_tzThermalFaultRegs.bit.KTC_401 == 1)
//                        || (CONTROLtzFaultRegs.bit.StackcellFault == 1)
//                        || (CANA_tzIOFlags.hydrogenValuesOK == 0)
//                        || (CANA_tzIOFlags.WLSNoleak == 0)
//                        || (CANA_tzLHC2AIFaultRegs.bit.RTD_401 == 1)
//                        || (CANA_tzThermalFaultRegs.bit.TTC_301 == 1)
//                        || (CANA_tzLPCAIFaultRegs.bit.OXS_101_RmpDwn == 1)
//                        )
                {

                    CANA_tzTimerRegs.tzPSU.CurRampUpcnt = 0;

                    CANA_tzTxdRegs.tzPSUData.TotalCurrentSet =
                            CANB_tzSiteRxRegs.f32CurrSet;

                    CANA_tzTxdRegs.tzPSUData.TotalISetTemp =
                            (CANA_tzTxdRegs.tzPSUData.TotalCurrentSet
                                    / ui32NodePresent);

                    CANA_tzTimerRegs.tzPSU.CurRampDowncnt++;

                    CONTROLtzFaultRegs.bit.LPCCurHealthy = 1;

                    if (CANA_tzTimerRegs.tzPSU.CurRampDowncnt >= 90)
                    {
                        CANA_tzTxdRegs.tzPSUData.CurrentSet -= 2;

                        CANA_tzTimerRegs.tzPSU.CurRampDowncnt = 0;
                    }

                    if (CANA_tzTxdRegs.tzPSUData.CurrentSet <= 0.5)
                    {
                        CANA_tzTxdRegs.tzPSUData.CurrentSet = 0.0;
                        CANA_tzTxdRegs.tzPSUData.TotalISetTemp = 0;
                        ui16SafeShutDownFlg = 1;
                    }

                }

//                 if ((CANA_tzLPCAIFaultRegs.bit.HYS_102_ShtDwn == 1) || (CONTROLtzFaultRegs.bit.VFDPumpFail == 1) || (CANA_tzLPCAIFaultRegs.bit.OXS_101_ShtDwn == 1))
//                {
//                    CANA_tzTimerRegs.tzPSU.CurRampDowncnt = 0;
//                    CANA_tzTimerRegs.tzPSU.CurRampUpcnt = 0;
//                    CONTROLtzFaultRegs.bit.LPCCurHealthy = 1;
//                    ui16SafeShutDownFlg = 0;
//                    CANA_tzTimerRegs.tzPSU.InstShutDowncnt++;
//                    if (CANA_tzTimerRegs.tzPSU.InstShutDowncnt >= 20)   // delay of 1.2s( 60m * 20) for Instantaneous trip
//                    {
//                        CANA_tzTimerRegs.tzPSU.InstShutDowncnt = 20;
//                        ui16InstShutDownFlg = 1;
//                    }
//                }

            }
        }
    }
}

/*==============================================================================
 End of File
 ==============================================================================*/
//void control_fncellmaxval()
//{
//
//    for (i = 0; i < n1; i++)
//    {
//        c[i] = CANA_tzRxdRegs.tzVSData.MaxCellvolt1[i];
//    }
//    for (j = n1; j < n3; j++)
//    {
//        c[j] = CANA_tzRxdRegs.tzVSData.MaxCellvolt2[j - i];
//    }
//
//    f32maxCellVolt = c[0];
//
//    for (k1 = 1; k1 < n3; k1++)
//    {
//        if (c[k1] > f32maxCellVolt)
//        {
//            f32maxCellVolt = c[k1];
//        }
//    }
//
//}
