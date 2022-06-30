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

    CANA_tzIOtimers.TxCntWaterloop++;

    if(CANA_tzIOtimers.TxCntWaterloop > 3)
    {
        CANA_tzIOtimers.TxCntWaterloop = 1;
    }

    // Operating Water Solenoid Valve based on LVL101
    if (CANA_tzIOtimers.TxCntWaterloop == 1)
    {
        if (MATHConvtzRegs.AISensorLVL101 >= 85.0) // greater than 85 % turn OFF
        {
            CANA_fnCmdsForDigIOs(CANA_mLHC_CABID, CANA_mLHC110_IO,
                                 (CANA_mDIG0_CLEAR)); // Turn OFF WSV101
        }
        else if (MATHConvtzRegs.AISensorLVL101 <= 70.0) // lesser than 70 % turn ON
        {

            CANA_fnCmdsForDigIOs(CANA_mLHC_CABID, CANA_mLHC110_IO,
                                 (CANA_mDIG0_SET)); // Turn ON WSV101
        }
    }
    else if (CANA_tzIOtimers.TxCntWaterloop == 2)
    {
        if (MATHConvtzRegs.AISensorLVL101 >= 60.0) // greater than 85 % turn OFF
        {
            CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV1 = 35000;

            CANA_fnCmdsForAnaOPVs(3, 0, &CANA_tzAnaOPParams);
        }
        else if (MATHConvtzRegs.AISensorLVL101 <= 58.0)
        {
            CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV1 = 0;

            CANA_fnCmdsForAnaOPVs(3, 0, &CANA_tzAnaOPParams);
        }
    }
//    else if (CANA_tzIOtimers.TxCntWaterloop == 3)
//    {
//        if (MATHConvtzRegs.AISensorLVL101 >= 60.0) // greater than 85 % turn OFF
//        {
//            CANA_tzAnaOPParams.CANA_tzAOI[0][0].AOI1 = 35000;
//
//            CANA_fnCmdsForAnaOPIs(3, 0, &CANA_tzAnaOPParams);
//        }
//        else if (MATHConvtzRegs.AISensorLVL101 <= 58.0)
//        {
//            CANA_tzAnaOPParams.CANA_tzAOI[0][0].AOI1 = 0;
//
//            CANA_fnCmdsForAnaOPIs(3, 0, &CANA_tzAnaOPParams);
//        }
//
//    }



}

/*==============================================================================
 End of File
 ==============================================================================*/
