/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  control_waterloop.c
 @author DEEPTI.K
 @date 06-Sep-2021

 @brief Description: Water-flow to the stack is decided based on Water level in tank
 @and requesting for a DI water from System Controller
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include <stdint.h>
#include "F28x_Project.h"
#include <stdint.h>
#include <stdbool.h>

#include "math.h"
#include "isr.h"
#include "scheduler.h"
#include "cana_defs.h"
#include "canb_defs.h"
#include "state_machine.h"
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

/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void control_waterloop(void);

/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t ui16SiteCommandTurnONPMP3 = 0;
uint16_t ui16VoltMotorSpeedCntrl = 0;
uint16_t ui16VoltMotorSpeedCntrl1 = 0;

float32_t f32VFDVoltageCAN = 0;
float32_t f32MotorSpeed = 0;

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void control_waterloop(void)
{

    // Operating Water Solenoid Valve based on LVL101

    if (MATHConvtzRegs.AISensorLVL101 >= 85.0)  // greater than 85 % turn OFF
    {
        CANA_fnCmdsForDigIOs(CANA_mLHC_CABID, CANA_mLHC110_IO,
                             (CANA_mDIG0_CLEAR)); // Turn OFF WSV101
    }
    else if (MATHConvtzRegs.AISensorLVL101 <= 70.0) // lesser than 70 % turn ON
    {

        CANA_fnCmdsForDigIOs(CANA_mLHC_CABID, CANA_mLHC110_IO,
                             (CANA_mDIG0_SET)); // Turn ON WSV101
    }





    //cana_fnCmdsForDigIOs()

}

//void Waterloop_fnCommandIOTURNONVFD(int32_t i32nodeID,
//                                    float32_t f32VoltMotorSpeedCntrl) // Turn OFF VFD valve
//{
//    CAN_setupMessageObject(CANA_BASE, CANA_mMAILBOX_21,
//                           (i32nodeID << 8) | CANA_mMSGID_QRY_MODELSPC_IO,
//                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
//                           CAN_MSG_OBJ_NO_FLAGS,
//                           CANA_mEIGHT_BYTE);
//
////    CANA_tzSetDO_IORegs[CANA_mLPC100_IO].all = 0;
////    CANA_tzSetAIEnable_IORegs[CANA_mLPC100_IO].all = 0xFF; //0x0F
////
////    // CANA_tzSetDO_IORegs[CANA_mLPC100_IO].bit.DO_bit7 = 1; // Turn ON VFD Relay
////
////    f32VFDVoltageCAN = (f32VoltMotorSpeedCntrl * 100);
////
////    CANA_tzSetAOData_IORegs[CANA_mLPC100_IO].AO_1 = f32VFDVoltageCAN; // 1V corresponds to 10Hz in VFD
////    //  Set_AO_Data[LPC_100_NODE].AO_2 = VFDVoltage & 0xFF;;
////
////    ui16txMsgDataIO[0] = CANA_mCOMMAND_QRY_GENERIC_IO;
////    ui16txMsgDataIO[1] = CANA_mDO_SET_IO;
////    ui16txMsgDataIO[2] = CANA_tzSetDO_IORegs[CANA_mLPC100_IO].all; // Turn On power to LHC IO Card Do_5 High
////    ui16txMsgDataIO[3] = CANA_tzSetAIEnable_IORegs[CANA_mLPC100_IO].all;
////    ui16txMsgDataIO[4] = ((CANA_tzSetAOData_IORegs[CANA_mLPC100_IO].AO_1
////            & 0xFF00) >> 8);
////    ui16txMsgDataIO[5] =
////            (CANA_tzSetAOData_IORegs[CANA_mLPC100_IO].AO_1 & 0x00FF);
////    ui16txMsgDataIO[6] = (CANA_tzSetAOData_IORegs[CANA_mLPC100_IO].AO_2 << 8);
////    ui16txMsgDataIO[7] = (CANA_tzSetAOData_IORegs[CANA_mLPC100_IO].AO_2 & 0xFF);
//
//    CAN_sendMessage(CANA_BASE, CANA_mMAILBOX_21, CANA_mEIGHT_BYTE,
//                    ui16txMsgDataIO);
//}
//
//void Waterloop_fnCommandIOTURNOFFVFD(int32_t i32nodeID,
//                                     float32_t f32VoltMotorSpeedCntrl) // Turn OFF VFD valve
//{
////    CAN_setupMessageObject(CANA_BASE, CANA_mMAILBOX_21,
////                           (i32nodeID << 8) | CANA_mMSGID_QRY_MODELSPC_IO,
////                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0x1FFFFFFF,
////                           CAN_MSG_OBJ_NO_FLAGS,
////                           CANA_mEIGHT_BYTE);
////
////    CANA_tzSetDO_IORegs[CANA_mLPC100_IO].all = 0;
////    CANA_tzSetAIEnable_IORegs[CANA_mLPC100_IO].all = 0xFF; //0x0F
////
////    f32VFDVoltageCAN = (f32VoltMotorSpeedCntrl * 100);
////
////    CANA_tzSetAOData_IORegs[CANA_mLPC100_IO].AO_1 = f32VFDVoltageCAN; // 1V corresponds to 10Hz in VFD
////    //  Set_AO_Data[LPC_100_NODE].AO_2 = VFDVoltage & 0xFF;;
////
////    ui16txMsgDataIO[0] = CANA_mCOMMAND_QRY_GENERIC_IO;
////    ui16txMsgDataIO[1] = CANA_mDO_CLEAR_IO;
////    ui16txMsgDataIO[2] = CANA_tzSetDO_IORegs[CANA_mLPC100_IO].all; // Turn On power to LHC IO Card Do_5 High
////    ui16txMsgDataIO[3] = CANA_tzSetAIEnable_IORegs[CANA_mLPC100_IO].all;
////    ui16txMsgDataIO[4] = ((CANA_tzSetAOData_IORegs[CANA_mLPC100_IO].AO_1
////            & 0xFF00) >> 8);
////    ui16txMsgDataIO[5] =
////            (CANA_tzSetAOData_IORegs[CANA_mLPC100_IO].AO_1 & 0x00FF);
////    ui16txMsgDataIO[6] = (CANA_tzSetAOData_IORegs[CANA_mLPC100_IO].AO_2 << 8);
////    ui16txMsgDataIO[7] = (CANA_tzSetAOData_IORegs[CANA_mLPC100_IO].AO_2 & 0xFF);
////
////    CAN_sendMessage(CANA_BASE, CANA_mMAILBOX_21, CANA_mEIGHT_BYTE,
////                    ui16txMsgDataIO);
//}
//
/*==============================================================================
 End of File
 ==============================================================================*/
