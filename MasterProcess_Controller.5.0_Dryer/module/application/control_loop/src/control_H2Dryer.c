/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  control_H2Dryer.c
 @author OHMLAP0042   
 @date 01-Feb-2022

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include "F28x_Project.h"              // Standard libraries headers
#include "F2837xS_Device.h"
#include "math.h"
#include <stdint.h>
#include "control_defs.h"
#include "cana_defs.h"
#include "state_machine.h"
#include "eep.h"

/*==============================================================================
 Defines
 ==============================================================================*/

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

union H2_tzVALVES H2_tzValves;

/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void H2_fnSVcontrol(void);
void CONTROL_DryerOutletValveLogic();
void CONTROL_DryerValveBleedLogic();

/*==============================================================================
 Local Variables
 ==============================================================================*/

uint32_t val1_2Cnt, ui16H2count, ui16H2count_eprom, ui16eepcount = 0;
uint16_t ui16valSetCnt_SV401402, ui16valResetCnt_SV401402;
uint16_t ui16CycleCount = 0;
uint16_t ui16GPIOStatus = 0;

uint16_t ui16AllValvesOffCnt = 0;

uint16_t valvestatus = 0;
uint16_t ui16Bleedh2 = 0, ui16BleedCnt = 0, dryerVal = 0, dryerVal1 = 1;
uint16_t uieepData1[8] = { 0 };
uint16_t uireadData1[8] = { 0 };

uint16_t eepwritecount = 0, epromStAdd = 0, uiepromAdd = 0;
uint16_t eepPagecnt = 0, eepPagecnt_rd = 0, readCnt = 0, writedone = 0;
uint16_t epromStAdd_wt = 0, epromStAdd_rd = 0, epromreset = 0,
        epromResetCnt = 0;
/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void H2_fnSVcontrol(void)
{

    if (epromreset == 1)
    {
        eepwritecount = 0;

        eepPagecnt = 0;
        epromStAdd_wt = 0;
        epromStAdd_rd = 0;
        eepPagecnt_rd = 0;

        epromResetCnt++;

        if (epromResetCnt >= 101)
        {
            epromResetCnt = 0;
        }

        if (epromResetCnt == 50)
        {

            uiTxMsgBuffer1[0] = (uint16_t) (epromStAdd_wt & 0xFF);
            uiTxMsgBuffer1[1] = (uint16_t) ((epromStAdd_wt >> 8) & 0xFF);
            uiTxMsgBuffer1[2] = (uint16_t) (eepPagecnt & 0xFF);
            uiTxMsgBuffer1[3] = (uint16_t) ((eepPagecnt >> 8) & 0xFF);

            EEP_fnWrite(&transaction_I2CMsg, 0x0, &uiTxMsgBuffer1[0], 4);

        }
        else if (epromResetCnt == 100)
        {
            uiTxMsgBuffer[0] = (uint16_t) (0);
            uiTxMsgBuffer[1] = (uint16_t) (0);
            uiTxMsgBuffer[2] = (uint16_t) (0);

            uiTxMsgBuffer[3] = (uint16_t) (0);
            uiTxMsgBuffer[4] = (uint16_t) (0);

            uiTxMsgBuffer[0] = (uint16_t) (0);

            EEP_fnWrite(&transaction_I2CMsg, 0x0, &uiTxMsgBuffer[0], 5);
        }

    }
    else
    {
        if (STAT_tzStateMac.Present_st == STACK_POWER)
        {

            eepwritecount++;
            if (eepwritecount >= 1251)
            {
                eepwritecount = 0;
            }
            //
            if (eepwritecount == 1200)
            {

                if (eepPagecnt <= 100)
                {

                    epromStAdd_wt = epromStAdd_rd + 64;
                    eepPagecnt = eepPagecnt_rd + 1;

                    uiTxMsgBuffer1[0] = (uint16_t) (epromStAdd_wt & 0xFF);
                    uiTxMsgBuffer1[1] =
                            (uint16_t) ((epromStAdd_wt >> 8) & 0xFF);
                    uiTxMsgBuffer1[2] = (uint16_t) (eepPagecnt & 0xFF);
                    uiTxMsgBuffer1[3] = (uint16_t) ((eepPagecnt >> 8) & 0xFF);

                    //            uiTxMsgBuffer1[0] = (uint16_t) (0 & 0xFF);
                    //            uiTxMsgBuffer1[1] = (uint16_t) ((0 >> 8) & 0xFF);
                    //            uiTxMsgBuffer1[2] = (uint16_t) (0 & 0xFF);
                    //            uiTxMsgBuffer1[3] = (uint16_t) ((0 >> 8) & 0xFF);

                    EEP_fnWrite(&transaction_I2CMsg, 0x0, &uiTxMsgBuffer1[0],
                                4);

                }

            }

            if (eepwritecount == 1250)
            {
                if (eepPagecnt <= 100)
                {
                    uiTxMsgBuffer[0] = (uint16_t) (ui16H2count & 0xFF);
                    uiTxMsgBuffer[1] = (uint16_t) ((ui16H2count >> 8) & 0xFF);
                    uiTxMsgBuffer[2] = (uint16_t) ((ui16H2count >> 16) & 0xFF);

                    uiTxMsgBuffer[3] = (uint16_t) (val1_2Cnt & 0xFF);
                    uiTxMsgBuffer[4] = (uint16_t) ((val1_2Cnt >> 8) & 0xFF);

                    uiTxMsgBuffer[0] = (uint16_t) (ui16CycleCount);
                    //            uiTxMsgBuffer[1] = (uint16_t) (0);
                    //            uiTxMsgBuffer[2] = (uint16_t) (0);
                    //
                    //            uiTxMsgBuffer[3] = (uint16_t) (0 & 0xFF);
                    //            uiTxMsgBuffer[4] = (uint16_t) ((0 >> 8) & 0xFF);

                    EEP_fnWrite(&transaction_I2CMsg, epromStAdd_wt,
                                &uiTxMsgBuffer[0], 6);

                    epromStAdd_rd = epromStAdd_wt;
                    eepPagecnt_rd = eepPagecnt;
                }

            }

            if (eepPagecnt >= 101)
            {
                eepPagecnt = 0;
                epromStAdd = 0;
            }
        }
    }
    // if (dryerVal == 1)

    ui16eepcount++;

    if (ui16eepcount >= 1201)
    {
        ui16eepcount = 1201;
    }

    if ((dryerVal1 == 1) && (ui16eepcount > 0) && (ui16eepcount <= 150))
    {
        EEP_fnRead(&transaction_I2CMsg, 0, &uiRxMsgBuffer1[0], 5);

    }

    else if ((dryerVal1 == 1) && (ui16eepcount >= 151) && (ui16eepcount <= 300))

    {

        epromStAdd_rd = ((int32) (((uiRxMsgBuffer1[1] << 8)
                | ((uiRxMsgBuffer1[0]) & 0x00FF)) & 0x0000FFFF));

        eepPagecnt_rd = ((int32) (((uiRxMsgBuffer1[3] << 8)
                | ((uiRxMsgBuffer1[2]) & 0x00FF)) & 0x0000FFFF));

    }

    else if ((dryerVal1 == 1) && (ui16eepcount >= 500)
            && (ui16eepcount <= 1000))
    {

        EEP_fnRead(&transaction_I2CMsg, epromStAdd_rd, &uiRxMsgBuffer[0], 5);

    }

    else if ((dryerVal1 == 1) && (ui16eepcount >= 1200)
            && (ui16eepcount <= 1300))
    {

        Uint32 temp = 0x0000ffff;

        temp = (temp & (int32) uiRxMsgBuffer[2]) << 16;

        ui16H2count = (int32) (temp
                | (int32) (((uiRxMsgBuffer[1] << 8)
                        | ((uiRxMsgBuffer[0]) & 0x00FF)) & 0x0000FFFF));

        val1_2Cnt = ((int32) (((uiRxMsgBuffer[4] << 8)
                | ((uiRxMsgBuffer[3]) & 0x00FF)) & 0x0000FFFF));

        ui16CycleCount = ((((uiRxMsgBuffer[3]) & 0x00FF)) & 0x0000FFFF);

        dryerVal1 = 0;

    }

    if (ui16Bleedh2 == 0)

    {
        if (STAT_tzStateMac.Present_st == STACK_POWER)
        {
            ui16AllValvesOffCnt = 0;
            ui16H2count++;

            // Separate count for operating valve 1 and 2

            val1_2Cnt++;

            // Resetting the count after 20 minutes

            if (ui16H2count > 24001)  //20001
            {
                ui16H2count = 0;
                ui16CycleCount = !ui16CycleCount;
            }

            if (val1_2Cnt > 600)
            {
                val1_2Cnt = 0;
            }

            if ((val1_2Cnt == 1) || (val1_2Cnt == 3))
            {
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO0 = 0x1;
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO1 = 0x0;

                CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC11_IO,
                                &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]); // Turn ON SV1ON & TURN OFF SV2

                H2_tzValves.bit.bt_Svalve1 = 1;
                H2_tzValves.bit.bt_Svalve2 = 0;

            }

            else if ((val1_2Cnt == 540) || (val1_2Cnt == 541))
            {
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO0 = 0x0;
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO1 = 0x1;

                CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC11_IO,
                                &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]); // Turn ON SV1OFF & TURN OFF SV1

                H2_tzValves.bit.bt_Svalve1 = 0;
                H2_tzValves.bit.bt_Svalve2 = 1;
            }

            if (ui16CycleCount == 0)
            {
                H2_tzValves.bit.bt_Svalve5 = 0;  // SV5 OFF in entire Cycle

                if ((ui16H2count == 2) || (ui16H2count == 4))
                {

                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO3 = 0x1; // Turn ON SV4

                    H2_tzValves.bit.bt_Svalve4 = 1;

                    CANA_fnMSTxCmds(
                            CANA_mLHC_CABID, CANA_mLHC11_IO,
                            &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);

                }
                else if ((ui16H2count >= 601) && (ui16H2count <= 602)) // turn ON SV6
                {
                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO5 = 0x1;

                    CANA_fnMSTxCmds(
                            CANA_mLHC_CABID, CANA_mLHC11_IO,
                            &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);

                    H2_tzValves.bit.bt_Svalve6 = 1;

                }

                else if ((ui16H2count == 17401) || (ui16H2count == 17403)) // turn OFF SV6
                {
                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO5 = 0x0;

                    CANA_fnMSTxCmds(
                            CANA_mLHC_CABID, CANA_mLHC11_IO,
                            &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);

                    H2_tzValves.bit.bt_Svalve6 = 0;

                }
                else if ((ui16H2count == 23401) || (ui16H2count == 23402)) // turn ON SV7
                {

                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO6 = 0x1;

                    CANA_fnMSTxCmds(
                            CANA_mLHC_CABID, CANA_mLHC11_IO,
                            &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);

                    H2_tzValves.bit.bt_Svalve7 = 1;

                }

                else if ((ui16H2count == 23999) || (ui16H2count == 24000)) // clear All GPIOs
                {
                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].all = 0x0;
                    CANA_fnMSTxCmds(
                            CANA_mLHC_CABID, CANA_mLHC11_IO,
                            &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]); // Turn ON SV1ON
                    H2_tzValves.all = 0;

                }

            }
            else
            {
                H2_tzValves.bit.bt_Svalve6 = 0;  // SV6 OFF in entire Cycle

                if ((ui16H2count == 2) || (ui16H2count == 4)) // Turn ON SV3

                {
                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO2 = 0x1;
                    CANA_fnMSTxCmds(
                            CANA_mLHC_CABID, CANA_mLHC11_IO,
                            &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);
                    H2_tzValves.bit.bt_Svalve4 = 1;

                }
                else if ((ui16H2count >= 601) && (ui16H2count <= 602)) // turn ON SV6
                {
                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO4 = 0x1;
                    CANA_fnMSTxCmds(
                            CANA_mLHC_CABID, CANA_mLHC11_IO,
                            &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);
                }

                else if ((ui16H2count == 17401) || (ui16H2count == 17403)) // turn OFF SV6
                {
                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO4 = 0x0;
                    CANA_fnMSTxCmds(
                            CANA_mLHC_CABID, CANA_mLHC11_IO,
                            &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);
                }
                else if ((ui16H2count == 23401) || (ui16H2count == 23402)) // turn ON SV7
                {
                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO6 = 0x1;
                    CANA_fnMSTxCmds(
                            CANA_mLHC_CABID, CANA_mLHC11_IO,
                            &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);

                }

                else if ((ui16H2count == 23999) || (ui16H2count == 24000)) // clear All GPIOs
                {
                    CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].all = 0x0;
                    CANA_fnMSTxCmds(
                            CANA_mLHC_CABID, CANA_mLHC11_IO,
                            &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);
                    H2_tzValves.all = 0;

                }
            }

        }
    }

    // Keeping all the valves OFF in Other states Except Stack Power

//        else if ((ui16Bleedh2 == 0)
//                && (STAT_tzStateMac.Present_st != STACK_POWER))
//        {
//            //  ui16H2count = 0;
//            ui16CycleCount = 0;
//
//            //    val1_2Cnt = 0;
//
//            ui16AllValvesOffCnt++;
//            if (ui16AllValvesOffCnt > 3)
//            {
//                ui16AllValvesOffCnt = 3;
//            }
//
//            if ((ui16AllValvesOffCnt >= 1) && (ui16AllValvesOffCnt <= 2))
//            {
//                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].all = 0x0;
//                CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC11_IO,
//                                &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]); // Reset all DOs
//
//                H2_tzValves.all = 0;
//
//            }
//        }
//}
}

// Dryer Outlet SV401 and SV402 Logic

//void CONTROL_DryerOutletValveLogic()
//{
//
////    if ((ui16SafeShutDownFlg == 1) || (ui16InstShutDownFlg == 1)
////            || (STAT_tzStateMac.Present_st != STACK_POWER))
////    {
////        ui16valSetCnt_SV401402 = 0;
////        ui16valResetCnt_SV401402++;
////        if (ui16valResetCnt_SV401402 < 2)
////        {
////            CANA_tzDO[1][0].bit.DO1 = 0x0;
////            CANA_tzDO[1][0].bit.DO2 = 0x0;
////
////            CANA_fnMSTxCmds(1, 0, &CANA_tzDO[1][0]); //Turn OFF SV401 & SV402 when H2 stops
////        }
////        else if (ui16valResetCnt_SV401402 >= 2)
////        {
////            ui16valResetCnt_SV401402 = 2;
////        }
////
////    }
////    else if ((ui16SafeShutDownFlg == 0) && (ui16InstShutDownFlg == 0)
////            && (STAT_tzStateMac.Present_st == STACK_POWER))
////    {
////        ui16valResetCnt_SV401402 = 0;
////        ui16valSetCnt_SV401402++;
////        if (ui16valSetCnt_SV401402 < 2)
////        {
////            CANA_tzDO[1][0].bit.DO1 = 0x1;
////            CANA_tzDO[1][0].bit.DO2 = 0x1;
////
////            CANA_fnMSTxCmds(1, 0, &CANA_tzDO[1][0]); //Turn ON SV401 & SV402 when H2 Production Starts
////        }
////        else if (ui16valSetCnt_SV401402 >= 2)
////        {
////            ui16valSetCnt_SV401402 = 2;
////        }
////
////    }
//}
//
//void CONTROL_DryerValveBleedLogic()
//{
//    if ((ui16Bleedh2 == 1) && (STAT_tzStateMac.Present_st == STAND_BY))
//    {
//        ui16BleedCnt++;
//        if ((ui16BleedCnt == 1) || (ui16BleedCnt == 2))
//        {
//            CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO6 = 0x1;
//            CANA_fnMSTxCmds(1, 1, &CANA_tzDO[1][1]); //Turn ON SV401 & SV402 when H2 Production Starts
//
//        }
//        else if ((ui16BleedCnt == 100) || (ui16BleedCnt == 101))
//        {
//            CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO2 = 0x1; // SV3 to SV6 on after 5 sec
//            CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO3 = 0x1;
//            CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO4 = 0x1;
//            CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO5 = 0x1;
//            CANA_fnMSTxCmds(1, 1, &CANA_tzDO[1][1]);
//        }
//        else if (ui16BleedCnt > 101)
//        {
//            ui16BleedCnt = 0;
//        }
//
//    }
//    else if (((STAT_tzStateMac.Present_st == READY) && (ui16Bleedh2 == 0))
//            || (STAT_tzStateMac.Present_st == STACK_CHECK)
//            || (STAT_tzStateMac.Present_st == STACK_POWER) && (ui16Bleedh2 == 0)
//            || ((ui16Bleedh2 == 0) && (STAT_tzStateMac.Present_st == STAND_BY)))
//    {
//        // ui16Bleedh2 = 0;
//        ui16BleedCnt = 0;
//    }
//}

/*==============================================================================
 End of File
 ==============================================================================*/
