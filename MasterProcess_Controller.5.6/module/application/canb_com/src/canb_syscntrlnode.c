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
#include "canb_defs.h"

#include "scheduler.h"

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

/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t ui16RxMsgDataSite7[8] = { 0 };
uint16_t ui16RxMsgDataSite8[8] = { 0 };
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

        CANB_tzSiteRxRegs.H2Percent = ((ui16RxMsgDataSite8[0] << 8)
                | (ui16RxMsgDataSite8[1])) * 0.01;

        CANB_tzSiteRxRegs.f32VoltSet = 400.0; //CV limit changed after connecting 15 power Supplies 15 cell voltages
        CANB_tzSiteRxRegs.f32CurrSet = (CANB_tzSiteRxRegs.H2Percent * 0.01
                * 1000.0);

        if (CANB_tzSiteRxRegs.f32CurrSet >= 10.0f)
        {
            CANB_tzSiteRxRegs.Start_H2Cmd = 1;
            CANB_tzSiteRxRegs.Start_cmd = 2;
        }
        else if (CANB_tzSiteRxRegs.f32CurrSet < 1.0f)
        {
            CANB_tzSiteRxRegs.Start_H2Cmd = 0;
        }

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

/*==============================================================================
 End of File
 ==============================================================================*/

