/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  init_system.c
 @author DEEPTI.K
 @date 14-Jan-2022

 @brief Description System Initialization done here
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include "init_peripherals.h"
#include "isr.h"
#include "scheduler.h"
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
#include "math.h"
#include "isr.h"
#include "scheduler.h"
#include "F021_F2837xS_C28x.h"
#include <stdint.h>
#include "F2837xS_device.h"
#include "F2837xS_Examples.h"

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

void INIT_fnCANAMailBox(void);
void INIT_fnCANBMailBox(void);
void INIT_fnSystem(void);
static void init_fnFlashSectors(void);
#define PUMPREQUEST *(unsigned long*)(0x00050024)
/*==============================================================================
 Local Variables
 ==============================================================================*/

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */
void INIT_fnSystem(void)
{

    //
    // Step 1. Initialize System Control:
    // PLL, WatchDog, enable Peripheral Clocks
    // This example function is found in the F2837xS_SysCtrl.c file.
    //
    InitSysCtrl();

    //
    // Step 2. Initialize GPIO:
    // This example function is found in the F2837xS_Gpio.c file and
    // illustrates how to set the GPIO to it's default state.
    // InitGpio();

    // Step 3. Clear all interrupts and initialize PIE vector table:
    // Disable CPU interrupts
    //
    DINT;

    //
    // Initialize the PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags
    // are cleared.
    // This function is found in the F2837xS_PieCtrl.c file.
    //
    InitPieCtrl();

    //
    // Disable CPU interrupts and clear all CPU interrupt flags:
    //
    IER = 0x0000;
    IFR = 0x0000;

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.
    // The shell ISR routines are found in F2837xS_DefaultIsr.c.
    // This function is found in F2837xS_PieVect.c.
    //
    InitPieVectTable();

    // Step 6: Initialize Peripherals

    INIT_fnPeripherals();

    // Step 7: Initialize global variables

    //
    // Map ISR functions
    //
    EALLOW;
    PieVectTable.EPWM1_INT = &epwm1_isr; //function of ePWM interrupt
    PieVectTable.TIMER0_INT = &cpu_timer0_isr;
    EDIS;

    Interrupt_register(INT_CANA0, &canaISR);

    InitCpuTimers();   // For this example, only initialize the Cpu Timers

    //
    // Configure CPU-Timer 0, 1, and 2 to interrupt every second:
    // 200MHz CPU Freq, 1 second Period (in uSeconds)
    //
        ConfigCpuTimer(&CpuTimer0, 200, 1000);

    //
    // To ensure precise timing, use write-only instructions to write to the
    // entire register. Therefore, if any of the configuration bits are changed in
    // ConfigCpuTimer and InitCpuTimers (in F2837xS_cputimervars.h), the below
    // settings must also be updated.
    //
            CpuTimer0Regs.TCR.all = 0x4000;

    //
    // Enable global Interrupts and higher priority real-time debug events:
    //
    IER |= M_INT3; //Enable group 3 interrupts
    IER |= M_INT1; //Enable group 3 interrupts


    //
    // Enable global Interrupts and higher priority real-time debug events:
    //
    EINT;

    // Enable Global interrupt INTM

    ERTM;

    // Enable Global real-time interrupt DBGM

    //
    // enable PIE interrupt
    //

    PieCtrlRegs.PIEIER3.bit.INTx1 = 1;     // ePWM Interrupt
    PieCtrlRegs.PIEIER1.bit.INTx7 = 1;     // CPU0 Interrupt

    CAN_enableGlobalInterrupt(CANA_BASE, CAN_GLOBAL_INT_CANINT0);


    // Scheduler variables initialization

    SCHR_fnVarInit(SCH_mNO_OF_EVENTS);

    // CAN MailBox Initialization

    INIT_fnCANAMailBox();

    INIT_fnCANBMailBox();

    init_fnFlashSectors();


}

/*==============================================================================
 End of File
 ==============================================================================*/


/*=============================================================================
 @brief initFlashSectors - Initializes the flash API

 @param void
 @return void
 ============================================================================ */
static void init_fnFlashSectors(void)
{
    EALLOW;

    PUMPREQUEST = 0x5A5A0001;

    Fapi_StatusType oReturnCheck;
    oReturnCheck = Fapi_initializeAPI(F021_CPU0_W1_BASE_ADDRESS, 200);
    if (oReturnCheck != Fapi_Status_Success)
    {
        //
        // Error code will be in the status parameter
        //
        __asm("    ESTOP0");
    }
    oReturnCheck = Fapi_setActiveFlashBank(Fapi_FlashBank0);
    if (oReturnCheck != Fapi_Status_Success)
    {
        //
        // Error code will be in the status parameter
        //
        __asm("    ESTOP0");
    }

    EDIS;
}

/*=============================================================================
 @brief Hardware reset

 @param void
 @return void
 ============================================================================ */
void Init_fnHWreset(void)
{
    volatile Uint16 temp;
    // Enable watchdog

        __eallow();
        temp = WdRegs.WDCR.all & 0x0007;
        WdRegs.WDCR.all = 0x0028 | temp;
        __eallow();

//        // Trigger watchdog timeout
        while(1)
            ;

}
