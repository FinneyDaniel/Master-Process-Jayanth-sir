/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================

 @file  oi_scheduler.c
 @author DEEPTI.K
 @date 06-Sep-2021
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
#include "canb_defs.h"
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

void SCHR_fnVarInit(uint16_t ui16totalEvents);

p_fn_EventDriver sch_fnEvent[SCH_mNO_OF_EVENTS];
static void sch_fnEvent1(void);
static void sch_fnEvent2(void);
static void sch_fnEvent3(void);
static void sch_fnEvent4(void);
static void sch_fnEvent5(void);
static void sch_fnEvent6(void);

uint16_t ui16txMsgDataPSU[8] = { 0 };
uint16_t i2cChk = 0;
uint16_t i2cChkRead = 0;
uint16_t x = 0;
/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t ui16PresentEvent;
uint16_t ui16PresentEventCmp;
uint16_t ui16totalNumEvents;
uint16_t ui16totalNumEventsCmp;

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void SCHR_fnVarInit(uint16_t ui16totalEvents)
{
// Initialize pointer to initial location in memory
    ui16PresentEvent = 0;
    ui16PresentEventCmp = 0;
    ui16totalNumEvents = ui16totalEvents;
    ui16totalNumEventsCmp = -ui16totalEvents;
    sch_fnEvent[0] = sch_fnEvent1;
    sch_fnEvent[1] = sch_fnEvent2;
    sch_fnEvent[2] = sch_fnEvent3;
    sch_fnEvent[3] = sch_fnEvent4;
    sch_fnEvent[4] = sch_fnEvent5;
    sch_fnEvent[5] = sch_fnEvent6;

}

static void sch_fnEvent1(void)
{

    CANA_FlashEvent();
}

static void sch_fnEvent2(void)
{
    x++;
    if (x > 255)
    {
        x = 0;
    }
}

static void sch_fnEvent3(void)
{
    CANB_fnSite_Event();
}

static void sch_fnEvent4(void)
{

}

static void sch_fnEvent5(void)
{

}

static void sch_fnEvent6(void)
{

}

/*==============================================================================
 End of File
 ==============================================================================*/
