/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================

 @file  scheduler.c
 @author JOTHI RAMESH
 @date 26-Jan-2022

 @brief Description

 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include "scheduler.h"
#include "F28x_Project.h"
#include <stdint.h>
#include "cana_defs.h"
#include <stdint.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "init_peripherals.h"
#include "hal/driverlib/can.h"
#include "hal/driverlib/debug.h"
#include "hal/driverlib/interrupt.h"
#include "cana_defs.h"
#include "i2c.h"
#include "eep.h"
#include "safety_lib.h"
#include "safety_lib_prv.h"
#include "app_constants.h"
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
static void SCH_fnslot_0(void);
static void SCH_fnslot_1(void);
static void SCH_fnslot_2(void);
static void SCH_fnslot_3(void);
static void SCH_fnslot_4(void);
static void SCH_fnslot_5(void);

void SCH_fnslot_all(void);

const fp_sch_slot_t psch_slots[NUM_mTIME_SLOTS] =
    {SCH_fnslot_0,
    SCH_fnslot_1,
    SCH_fnslot_2,
    SCH_fnslot_3,
    SCH_fnslot_4,
    SCH_fnslot_5,
     };

/*==============================================================================
 Local Variables
 ==============================================================================*/
uint16_t ui16txMsgDataPSU[8] = { 0 };
uint16_t i2cChk = 0;
uint16_t i2cChkRead = 0;
static const fp_sch_slot_t pevery_sch_slot;
static const fp_sch_slot_t pevery_sch_slot_cmp;
static int16_t icurrent_slot;
extern const fp_sch_slot_t psch_slots[NUM_mTIME_SLOTS];
uint16_t xx=0,yy=0;
/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */

//-----------------------------------Event Drivers------------------------------------------------//
void scheduler_init(uint16_t slots, const fp_sch_slot_t *psch_slots,
                    const fp_sch_slot_t SCH_fnslot_all)
{
    icurrent_slot = 0;
    fp_sch_slot_t *p_every_sch_slot;
    p_every_sch_slot = (fp_sch_slot_t*) &pevery_sch_slot;
    *p_every_sch_slot = SCH_fnslot_all;
    p_every_sch_slot = (fp_sch_slot_t*) &pevery_sch_slot_cmp;
    *p_every_sch_slot = SCH_fnslot_all;
}

void scheduler_task(void)
{
    icurrent_slot++;
    if (icurrent_slot >= NUM_mTIME_SLOTS)
    {
        icurrent_slot = 0;
  //      itotal_slots_cmp = 0;
    }

    if (icurrent_slot < NUM_mTIME_SLOTS)
    {
        psch_slots[icurrent_slot]();
    }
    if (pevery_sch_slot != NULL)
        pevery_sch_slot();
}
/*=============================================================================*/

void SCH_fnslot_0(void)
{
    xx++;
    safety_fnlog_monitoring_chk();

    safety_fnLog_monitoring_slot_exe(0);
 }

void SCH_fnslot_1(void)
{
    CANA_FlashEvent();
    //safety_fnLog_monitoring_slot_exe(1);
 }

void SCH_fnslot_2(void)
{
    MATH_fnCalc();
    //safety_fnLog_monitoring_slot_exe(2);
 }

void SCH_fnslot_3(void)
{
    //safety_fnLog_monitoring_slot_exe(3);
 }

void SCH_fnslot_4(void)
{
    //safety_fnLog_monitoring_slot_exe(4);
}

void SCH_fnslot_5(void)
{

}



void SCH_fnslot_all(void)
{
    yy++;
}

/*==============================================================================
 End of File
 ==============================================================================*/
