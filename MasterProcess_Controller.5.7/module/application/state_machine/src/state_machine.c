/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  state_machine.c
 @author DEEPTI.K
 @date 06-Sep-2021

 @brief Description
 ==============================================================================*/

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
#include "cana_defs.h"
#include "canb_defs.h"

#include "state_machine.h"
#include "scheduler.h"
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

/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void STAT_fnFSMCheck(void);

static void stat_fnFSMNextState(void);
static void stat_IOReset(void);
/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t ui32StateTimer = 0;
STAT_tzSTATEMAC STAT_tzStateMac;
STAT_tzSTATE_MSMAC STAT_tzStateMacMS;
/*==============================================================================
 Local Constants
 ==============================================================================*/

void stat_fnInitState(void)
{
    // Global Flags

    // State Variables Initialization
    STAT_tzStateMac.Next_st = STAND_BY;
    STAT_tzStateMac.Previous_st = STAND_BY;
    STAT_tzStateMac.Present_st = STAND_BY;
    CANA_tzMSRegs.StartCmd = 0;
    ui16Bleedh2 = 0;
}
/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */

void STAT_fnFSMCheck(void)
{
    {

        //  Determine the Next State depending on the flags set/reset in the rest of the code

        stat_fnFSMNextState();

        switch (STAT_tzStateMac.Present_st)
        {

        case STAND_BY:
        {

            break;
        }

        case READY:
        {

            break;
        }

        case STACK_CHECK:
        {

            break;
        }

        case STACK_POWER:
        {

            break;
        }

        case FAULT:
        {

            break;
        }

        case COMMISSION:
        {

            break;
        }

        default:
            break;
        }
    }

}

//#################################################################################################################
static void stat_fnFSMNextState()
{
    switch (STAT_tzStateMac.Next_st)
    {

    case STAND_BY:
    {

        if (CANA_tzMSRegs.StartCmd == 0)
        {
            STAT_tzStateMac.Next_st = STAND_BY;

            CANB_tzSiteRxRegs.StateChngStandBy = 1;
            stat_IOReset();
        }

        else if ((CANA_tzMSRegs.StartCmd == 1)
                && (CANA_tzIOflags.LPC30Comfail == 0)
                && (CANA_tzIOflags.LPC31Comfail == 0)
                && (CANA_tzIOflags.LHC10Comfail == 0)
                && (CANA_tzIOflags.LHC11Comfail == 0)
                && (CANB_tzSiteRxRegs.Start_cmd == 1))  // Manually change it for now
        {

            STAT_tzStateMac.Next_st = READY;
        }

        break;
    }
    case READY:
    {
        if (CANA_tzMSRegs.StartCmd == 0)
        {
            STAT_tzStateMac.Next_st = STAND_BY;

            CANB_tzSiteRxRegs.StateChngStandBy = 1;
            stat_IOReset();
        }

        else if ((CANA_tzMSRegs.StartCmd == 1)
                && (CANB_tzSiteRxRegs.Start_cmd == 2))
        {
            STAT_tzStateMac.Next_st = STACK_CHECK;
        }

        else if (CANA_tzMSRegs.StartCmd == 5)
        {
            STAT_tzStateMac.Next_st = COMMISSION;
        }

        if (CANB_tzSiteRxRegs.f32CurrSet >= 10.0f)
        {
            CANB_tzSiteRxRegs.Start_H2Cmd = 1;
            CANB_tzSiteRxRegs.Start_cmd = 2;
        }
        else if (CANB_tzSiteRxRegs.f32CurrSet < 1.0f)
        {
            CANB_tzSiteRxRegs.Start_H2Cmd = 0;
        }


        break;
    }
    case STACK_CHECK:
    {

        if (CANA_tzMSRegs.StartCmd == 0)
        {
            STAT_tzStateMac.Next_st = STAND_BY;

            CANB_tzSiteRxRegs.StateChngStandBy = 1;
            stat_IOReset();
        }

        else if ((CANA_tzMSRegs.StartCmd == 1)
                && (CANB_tzSiteRxRegs.Start_cmd == 3))
        {
            STAT_tzStateMac.Next_st = STACK_POWER;
        }

        break;
    }
    case STACK_POWER:
    {

        if (CANA_tzMSRegs.StartCmd == 0)
        {
            STAT_tzStateMac.Next_st = STAND_BY;

            CANB_tzSiteRxRegs.StateChngStandBy = 1;
            stat_IOReset();
        }

        if(CANB_tzSiteRxRegs.Start_H2Cmd == 0)
        {
            ui16StateTnstCnt = 0;
            ui16StateRstCnt = 0;
            STAT_tzStateMac.Next_st = READY;
            CANB_tzSiteRxRegs.Start_cmd = 1;

        }

        if(ui16InstShutDownFlg == 1)
        {
            CANB_tzSiteRxRegs.f32CurrSet = 0;
            CANB_tzSiteRxRegs.StateChngFault = 1;
            STAT_tzStateMac.Next_st = FAULT;
        }

        break;
    }
    case FAULT:
    {

        if (CANB_tzSiteRxRegs.Start_cmd == 0)
        {
            STAT_tzStateMac.Next_st = STAND_BY;

            CANB_tzSiteRxRegs.StateChngStandBy = 1;
            stat_IOReset();
        }

        else if(ui16InstShutDownFlg == 0)
        {
            ui16StateTnstCnt = 0;
             ui16StateRstCnt = 0;
             CANB_tzSiteRxRegs.StateChngFault = 0;
             CANB_tzSiteRxRegs.Start_cmd = 1;
              stat_IOReset();

            STAT_tzStateMac.Next_st = READY;

        }

        else if (CANA_tzMSRegs.StartCmd == 0)
        {
            STAT_tzStateMac.Next_st = STAND_BY;

            CANB_tzSiteRxRegs.StateChngStandBy = 1;
            stat_IOReset();
        }
        break;
    }

    case COMMISSION:
    {
        if (CANB_tzSiteRxRegs.Start_cmd == 0)
        {
            STAT_tzStateMac.Next_st = STAND_BY;

            CANB_tzSiteRxRegs.StateChngStandBy = 1;
            stat_IOReset();
        }

        break;
    }

    default:
        break;
    }

// If the State.Next remains the same as State.Current no action needs to be taken
// However if two above mentioned states are different, the System is disabled
// and the state change flags, which track the time after a state change are reset
// to their initial states.
    if (STAT_tzStateMac.Next_st == STAT_tzStateMac.Present_st)
    {
        // asm ("NOP");
    }
    else
    {

    }
    STAT_tzStateMac.Previous_st = STAT_tzStateMac.Present_st;
    STAT_tzStateMac.Present_st = STAT_tzStateMac.Next_st;
}

/*==============================================================================
 End of File
 ==============================================================================*/
void stat_IOReset(void)
{
//    uint16_t ui16temp;

//    CANA_tzAISensorData.COS_101 = 4.0;
//    CANA_tzAISensorData.HYS_101 = 4.0;
//    CANA_tzAISensorData.HYS_501 = 4.0;
//    CANA_tzAISensorData.HYS_401 = 4.0;
//    CANA_tzAISensorData.HYS_102 = 4.0;
//    CANA_tzAISensorData.OXS_101 = 4.0;
//    CANA_tzAISensorData.TE_401 = 4.0;
//    CANA_tzAISensorData.DPT_401 = 4.0;
//    CANA_tzAISensorData.LVL_101 = 4.0;
//    CANA_tzAISensorData.PRT_101 = 4.0;
//    CANA_tzAISensorData.PRT_102 = 4.0;
//    CANA_tzAISensorData.PRT_401 = 4.0;
//
//    CANA_tzAISensorData.PRT_402 = 4.0;
//
//    CANA_tzThermoCoupleData.KTC_401 = 4.0;
//    CANA_tzThermoCoupleData.KTC_402 = 4.0;
//    CANA_tzThermoCoupleData.TTC_102 = 4.0;
//    CANA_tzThermoCoupleData.TTC_101 = 4.0;
//    CANA_tzThermoCoupleData.KTC_801 = 4.0;
//    CANA_tzThermoCoupleData.TTC_301 = 4.0;
//
//    //CANA_tzMSRegs.StartCmd = 0;
//
//    for (ui16temp = 0; ui16temp <= 3; ui16temp++)
//    {
//
//        // CANA_tzDI_IORegs[ui16temp].all = 0;
//
//    }


}
