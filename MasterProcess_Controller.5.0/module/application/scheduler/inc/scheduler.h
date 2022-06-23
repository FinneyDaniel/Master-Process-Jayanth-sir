/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  oi_scheduler.h
 @author DEEPTI.K
 @date 06-Sep-2021

 @brief Description
==============================================================================*/

#ifndef MODULE_APPLICATION_SCHEDULER_INC_SCHEDULER_H_
#define MODULE_APPLICATION_SCHEDULER_INC_SCHEDULER_H_

/*==============================================================================
 Includes
==============================================================================*/

#include "F28x_Project.h"
#include <stdint.h>


/*==============================================================================
 Defines
==============================================================================*/

/*==============================================================================
 Enums
==============================================================================*/

/*==============================================================================
 Structures
==============================================================================*/

//// The basic structure is defined here.

// Structure to store Event related Info



/*==============================================================================
 Macros
==============================================================================*/

#define TRUE                             (1U)
#define FALSE                            (0U)
#define SCH_mNO_OF_EVENTS                (6U)
#define SCH_mSCHEDULER_COUNT             (200U)

/*==============================================================================
 Extern/Public Function Prototypes
==============================================================================*/

extern void SCHR_fnSchedulerVariablesInit(void);

// Scheduler events

extern uint16_t schr_fnEvent1();
extern uint16_t schr_fnEvent2();
extern uint16_t schr_fnEvent3();
extern uint16_t schr_fnEvent4();
extern uint16_t schr_fnEvent5();

extern uint16_t schr_fnCntrlEvent1();
extern uint16_t schr_fnCntrlEvent2();
extern uint16_t schr_fnCntrlEvent3();
extern uint16_t schr_fnCntrlEvent4();
extern uint16_t schr_fnCntrlEvent5();

/*==============================================================================
 Extern/Public Variables
==============================================================================*/

typedef void (*p_fn_EventDriver)(void);
extern p_fn_EventDriver  sch_fnEvent[SCH_mNO_OF_EVENTS];
extern void sch_fnTask(void);

extern void SCHR_fnVarInit(uint16_t ui16totalEvents);
/*==============================================================================
 Extern/Public structures
==============================================================================*/

extern uint16_t ui16PresentEvent;
extern uint16_t ui16PresentEventCmp;
extern uint16_t ui16totalNumEvents;
extern uint16_t ui16totalNumEventsCmp;

/*==============================================================================
 Extern/Public Constants
==============================================================================*/


#endif /* MODULE_SCHEDULER_INC_OI_SCHEDULER_H_ */
