/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  isr.c
 @author OHMLAP0042   
 @date 14-Jan-2022

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
#include "isr.h"
#include "scheduler.h"
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

/*==============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

__interrupt void canaISR(void);     // Receive interrupt for CAN-A.
__interrupt void epwm1_isr(void);
__interrupt void cpu_timer0_isr(void);

/*==============================================================================
 Local Variables
 ==============================================================================*/

volatile uint32_t errorFlag = 0;
volatile uint32_t rxMsgCount = 0;
uint16_t LEDCount = 0;
uint16_t ui16fsmCounter = 0;
/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */

interrupt void epwm1_isr(void)
{

    ui16fsmCounter++;

    if (ui16fsmCounter > 20)  // FSM occurs once evry 2ms (100us * 20 = 2ms)
    {
        STAT_fnFSMCheck();
        ui16fsmCounter = 0;
    }

    CANA_fnRXevent();

    CANA_fnTask();

    //
    // Clear INT flag for this timer
    //
    EPwm1Regs.ETCLR.bit.INT = 1;

    //
    // Acknowledge this interrupt to receive more interrupts from group 3
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;

}

//
// cpu_timer0_isr - CPU Timer0 ISR with interrupt counter
//
__interrupt void cpu_timer0_isr(void)
{

    LEDCount++;

    if(LEDCount > 1000)
    {
        LEDCount = 0;
    }

    if (LEDCount <= 500)
    {
        GpioDataRegs.GPCSET.bit.GPIO92 = 1;
    }
    else if ((LEDCount >= 501) && (LEDCount <= 1000))
    {
        GpioDataRegs.GPCCLEAR.bit.GPIO92 = 1;
    }

   CpuTimer0.InterruptCount++;

   ui16PresentEvent++;
   //ui16PresentEventCmp --;

   if(ui16PresentEvent >= SCH_mNO_OF_EVENTS)
   {
       ui16PresentEvent = 0;
       ui16PresentEventCmp = 0;
   }

   //sch_fnUpdateEvent();

   if(ui16PresentEvent < SCH_mNO_OF_EVENTS)
   {
       sch_fnEvent[ui16PresentEvent]();
   }


   //
   // Acknowledge this interrupt to receive more interrupts from group 1
   //
   PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

interrupt void canaISR(void)
{
    uint32_t status;

    //
    // Read the CAN interrupt status to find the cause of the interrupt
    //
    status = CAN_getInterruptCause(CANA_BASE);

    //
    // If the cause is a controller status interrupt, then get the status
    //
    if (status == CAN_INT_INT0ID_STATUS)
    {
        //
        // Read the controller status.  This will return a field of status
        // error bits that can indicate various errors.  Error processing
        // is not done in this example for simplicity.  Refer to the
        // API documentation for details about the error status bits.
        // The act of reading this status will clear the interrupt.
        //
        status = CAN_getStatus(CANA_BASE);

        //
        // Check to see if an error occurred.
        //
        if (((status & ~(CAN_STATUS_TXOK | CAN_STATUS_RXOK)) != 7)
                && ((status & ~(CAN_STATUS_TXOK | CAN_STATUS_RXOK)) != 0))
        {
            //
            // Set a flag to indicate some errors may have occurred.
            //
            errorFlag = 1;
        }
    }

    //
    // Check if the cause is the receive message object 2
    //
    else if (status == CAN_mMAILBOX_13)
    {
        //
        // Get the received message
        //

        //
        // Getting to this point means that the RX interrupt occurred on
        // message object 2, and the message RX is complete.  Clear the
        // message object interrupt.
        //
        CAN_clearInterruptStatus(CANA_BASE, CAN_mMAILBOX_13);

        //
        // Increment a counter to keep track of how many messages have been
        // received. In a real application this could be used to set flags to
        // indicate when a message is received.
        //
        rxMsgCount++;

        //
        // Since the message was received, clear any error flags.
        //
        errorFlag = 0;
    }

    //
    // Clear the global interrupt flag for the CAN interrupt line
    //
    CAN_clearGlobalInterruptStatus(CANA_BASE, CAN_GLOBAL_INT_CANINT0);

    //
    // Acknowledge this interrupt located in group 9
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

/*==============================================================================
 End of File
 ==============================================================================*/
