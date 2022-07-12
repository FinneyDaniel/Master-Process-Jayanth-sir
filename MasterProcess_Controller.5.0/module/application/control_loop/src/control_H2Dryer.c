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

uint16_t valvestatus = 0;
uint16_t valveGpioNumber = 0;
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

    if (STAT_tzStateMac.Present_st == STACK_POWER)
    {
        ui16AllValvesOffCnt = 0;
        ui16H2count++;

        // Separate count for operating valve 1 and 2

        val1_2Cnt++;

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

        if ((val1_2Cnt == 1) || (val1_2Cnt == 3))
        {
            CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO0 = 0x1;
            CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO1 = 0x0;

            CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC11_IO,
                            &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]); // Turn ON SV1ON & TURN OFF SV2

            H2_tzValves.bit.bt_Svalve1 = 1;
            H2_tzValves.bit.bt_Svalve2 = 0;

        }

        else if ((val1_2Cnt == 451) || (val1_2Cnt == 452))
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

                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO4 = 0x1; // Turn ON SV4

                H2_tzValves.bit.bt_Svalve4 = 1;

                CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC11_IO,
                                &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);

            }
            else if ((ui16H2count >= 501) && (ui16H2count <= 502)) // turn ON SV6
            {
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO6 = 0x1;

                CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC11_IO,
                                &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);

                H2_tzValves.bit.bt_Svalve6 = 1;

            }

            else if ((ui16H2count == 14501) || (ui16H2count == 14503)) // turn OFF SV6
            {
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO6 = 0x0;

                CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC11_IO,
                                &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);

                H2_tzValves.bit.bt_Svalve6 = 0;

            }
            else if ((ui16H2count == 19502) || (ui16H2count == 19504)) // turn ON SV7
            {

                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO7 = 0x1;

                CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC11_IO,
                                &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);

                H2_tzValves.bit.bt_Svalve7 = 1;

            }

            else if ((ui16H2count == 19999) || (ui16H2count == 20000)) // clear All GPIOs
            {
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].all = 0x0;
                CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC11_IO,
                                &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]); // Turn ON SV1ON
                H2_tzValves.all = 0;

            }

        }
        else
        {
            H2_tzValves.bit.bt_Svalve6 = 0;  // SV6 OFF in entire Cycle

            if ((ui16H2count == 3) || (ui16H2count == 6))  // Turn ON SV4
            {
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO3 = 0x1;
                CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC11_IO,
                                &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);
                H2_tzValves.bit.bt_Svalve4 = 1;

            }
            else if ((ui16H2count >= 501) && (ui16H2count <= 502)) //Turn ON SV5
            {
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO5 = 0x1;
                CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC11_IO,
                                &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);
            }

            else if ((ui16H2count == 14501) || (ui16H2count == 14503)) //Turn OFF SV5
            {
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO5 = 0x0;
                CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC11_IO,
                                &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);
            }
            else if ((ui16H2count == 19502) || (ui16H2count == 19504)) //Turn ON SV7
            {
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].bit.DO7 = 0x1;
                CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC11_IO,
                                &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);

            }

            else if ((ui16H2count == 19999) || (ui16H2count == 20000)) // Clear All GPIOs
            {
                CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].all = 0x0;
                CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC11_IO,
                                &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]);
                H2_tzValves.all = 0;

            }
        }

    }

    // Keeping all the valves OFF in Other states Except Stack Power

    else
    {
        ui16H2count = 0;
        ui16CycleCount = 0;

        val1_2Cnt = 0;

        ui16AllValvesOffCnt++;
        if (ui16AllValvesOffCnt > 3)
        {
            ui16AllValvesOffCnt = 3;
        }

        if ((ui16AllValvesOffCnt >= 1) && (ui16AllValvesOffCnt <= 2))
        {
            CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO].all = 0x0;
            CANA_fnMSTxCmds(CANA_mLHC_CABID, CANA_mLHC11_IO,
                            &CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC11_IO]); // Turn ON SV1ON

            H2_tzValves.all = 0;

        }
    }
}

/*==============================================================================
 End of File
 ==============================================================================*/
