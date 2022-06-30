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

/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t ui16H2count = 0, val1_2Cnt = 0;
uint16_t ui16CycleCount = 0;
uint16_t ui16GPIOStatus = 0;

uint16_t ui16AllValvesOffCnt = 0;

uint16_t ui16HSV1_2Count1 = 0, ui16HSV1_2Count2 = 0;
uint16_t ui16HSV3_4Count1 = 0, ui16HSV3_4Count2 = 0, ui16HSV5_6Count3 = 0,
        ui16HSV5_6Count4 = 0;
uint16_t ui16HSV5_6Count1 = 0, ui16HSV5_6Count2 = 0;
uint16_t ui16HSV7Count1 = 0, ui16HSV7Count2 = 0;
uint16_t valvestatus = 0;
uint16_t valveGpioNumber = 0;
/*==============================================================================
 Local Constantsxffxxxxxx
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void H2_fnSVcontrol(void)
{

    // cana_fnH2DryerCommands1(CANA_mTXLHC101_IO, valveGpioNumber, valvestatus);  // SV6 ON

    if (STAT_tzStateMac.Present_st == OI_STACK_POWER)
    {
        ui16AllValvesOffCnt = 0;
        ui16H2count++;

        // Separate count for operating valve 1 and 2

        val1_2Cnt++;

        if ((val1_2Cnt == 1) || (val1_2Cnt == 4))
        {
            cana_fnH2DryerCommands1(CANA_mTXLHC101_IO, 0x02, GPIO_SET); // SV1 ON
            H2_tzValves.bit.bt_Svalve1 = 1;

        }

        else if ((val1_2Cnt == 2) || (val1_2Cnt == 5))
        {
            cana_fnH2DryerCommands1(CANA_mTXLHC101_IO, 0x04, GPIO_CLEAR); // SV2 OFF
            H2_tzValves.bit.bt_Svalve2 = 0;
        }

        else if ((val1_2Cnt == 451) || (val1_2Cnt == 453))
        {
            cana_fnH2DryerCommands1(CANA_mTXLHC101_IO, 0x02, GPIO_CLEAR); // SV1 OFF
            H2_tzValves.bit.bt_Svalve1 = 0;

        }

        else if ((val1_2Cnt == 452) || (val1_2Cnt == 454))
        {
            cana_fnH2DryerCommands1(CANA_mTXLHC101_IO, 0x04, GPIO_SET); // SV2 ON
            H2_tzValves.bit.bt_Svalve2 = 1;
        }

        if (ui16CycleCount == 0)
        {
            H2_tzValves.bit.bt_Svalve5 = 0;

            if ((ui16H2count == 3) || (ui16H2count == 6))
            {
                cana_fnH2DryerCommands1(CANA_mTXLHC101_IO, 0x10, GPIO_SET); // SV4 ON  in the beginning of cycle
                H2_tzValves.bit.bt_Svalve4 = 1;

            }
            else if ((ui16H2count >= 501) && (ui16H2count <= 502)) // after 3 sec  (to be changed to 30sec - Number 501, 502)
            {
                cana_fnH2DryerCommands1(CANA_mTXLHC101_IO, 0x40, GPIO_SET); // SV6 ON
                H2_tzValves.bit.bt_Svalve6 = 1;

            }

            else if ((ui16H2count == 14501) || (ui16H2count == 14503)) // after 87 sec (to be changed to 870sec - Number 14501, 14503)
            {
                cana_fnH2DryerCommands1(CANA_mTXLHC101_IO, 0x40, GPIO_CLEAR); // SV6 OFF
                H2_tzValves.bit.bt_Svalve6 = 0;
            }
            else if ((ui16H2count == 19502) || (ui16H2count == 19504)) // after 117 sec (to be changed to 1170sec - Number 19501, 19503 -on for 30sec)
            {
                cana_fnH2DryerCommands1(CANA_mTXLHC101_IO, 0x80, GPIO_SET); // SV7 ON
                H2_tzValves.bit.bt_Svalve7 = 1;

            }

            else if ((ui16H2count == 19999) || (ui16H2count == 20000)) // after 120 sec (to be changed to 1200sec - Number 19999, 20000 - off all valves at end of cycle)
            {
                cana_fnH2DryerCommands1(CANA_mTXLHC101_IO, 0xFE, GPIO_CLEAR); // CLEAR ALL
                H2_tzValves.all = 0;

            }

        }
        else
        {
            H2_tzValves.bit.bt_Svalve6 = 0;

            if ((ui16H2count == 3) || (ui16H2count == 6))
            {
                cana_fnH2DryerCommands1(CANA_mTXLHC101_IO, 0x08, GPIO_SET); // SV3 ON in the beginning of cycle
                H2_tzValves.bit.bt_Svalve3 = 1;

            }
            else if ((ui16H2count >= 501) && (ui16H2count <= 502)) // after 3 sec (to be changed to 30sec - Number 501, 502)
            {
                cana_fnH2DryerCommands1(CANA_mTXLHC101_IO, 0x20, GPIO_SET); // SV5 ON
                H2_tzValves.bit.bt_Svalve5 = 1;
            }

            else if ((ui16H2count == 14501) || (ui16H2count == 14503)) // after 87 sec (to be changed to 870sec - Number 14501, 14503)
            {
                cana_fnH2DryerCommands1(CANA_mTXLHC101_IO, 0x20, GPIO_CLEAR); // SV5 OFF
                H2_tzValves.bit.bt_Svalve5 = 0;

            }
            else if ((ui16H2count == 19502) || (ui16H2count == 19504)) // after 117 sec (to be changed to 1170sec - Number 19501, 19503 -on for 30sec)
            {
                cana_fnH2DryerCommands1(CANA_mTXLHC101_IO, 0x80, GPIO_SET); // SV7 ON
                H2_tzValves.bit.bt_Svalve7 = 1;

            }

            else if ((ui16H2count == 19999) || (ui16H2count == 20000)) // after 120 sec (to be changed to 1200sec - Number 19999, 20000 - off all valves at end of cycle)
            {
                cana_fnH2DryerCommands1(CANA_mTXLHC101_IO, 0xFE, GPIO_CLEAR); // CLEAR ALL
                H2_tzValves.all = 0;

            }

        }

        // Resetting the count after 20 minutes

        if (ui16H2count > 20000)  //20001
        {
            ui16H2count = 0;
            ui16CycleCount = !ui16CycleCount;
        }

        if (val1_2Cnt > 500)
        {
            val1_2Cnt = 0;
        }
    }
    //
    // Keeping all the valves OFF in Other states Except Stack Power

    else
    {
        ui16H2count = 0;
        ui16CycleCount = 0;

        ui16HSV1_2Count1 = 0;
        ui16HSV1_2Count2 = 0;
        ui16HSV3_4Count1 = 0;
        ui16HSV3_4Count2 = 0;
        ui16HSV5_6Count1 = 0;
        ui16HSV5_6Count2 = 0;
        ui16HSV5_6Count3 = 0;
        ui16HSV5_6Count4 = 0;
        ui16HSV7Count1 = 0;
        ui16HSV7Count2 = 0;
        val1_2Cnt = 0;

        ui16AllValvesOffCnt++;
        if (ui16AllValvesOffCnt > 3)
        {
            ui16AllValvesOffCnt = 3;
        }

        ui16GPIOStatus = 1;

        if ((ui16AllValvesOffCnt >= 1) && (ui16AllValvesOffCnt <= 2))
        {
            cana_fnH2DryerCommands1(CANA_mTXLHC101_IO, 0xFE, GPIO_CLEAR); // CLEAR ALL
            H2_tzValves.all = 0;

        }
    }

}

/*==============================================================================
 End of File
 ==============================================================================*/
