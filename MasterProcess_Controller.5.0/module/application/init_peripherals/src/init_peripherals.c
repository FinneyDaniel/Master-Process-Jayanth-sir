/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  init_peripherals.c
 @author OHMLAP0042   
 @date 14-Jan-2022

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"

#include "hal/driverlib/can.h"
#include "init_gpio.h"
#include "init_peripherals.h"
#include "cana_defs.h"
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

void init_fnPWM(void);
void init_fnPWM1(void);
void INIT_fnGPIO(void);
void INIT_fnPeripherals(void);
static void per_fnIniti2cB(void);

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
void INIT_fnPeripherals(void)
{

    EALLOW;
    ClkCfgRegs.PERCLKDIVSEL.bit.EPWMCLKDIV = PER_mCLKDIV_BY_TWO; // Divide by 2 of PLLSYSCLK
    EDIS;
    INIT_fnGPIO();                            // GPIO initialization to the PWMs
    init_fnPWM();                             // Initialization of EPWMs

    //
    // Setup GPIO pin mux for CAN-A TX/RX and CAN-B TX/RX
    //
    GPIO_SetupPinMux(62, GPIO_MUX_CPU1, 6); //GPIO30 -  CANRXA
    GPIO_SetupPinOptions(62, GPIO_INPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(63, GPIO_MUX_CPU1, 6); //GPIO31 - CANTXA
    GPIO_SetupPinOptions(63, GPIO_OUTPUT, GPIO_PUSHPULL);

    GPIO_SetupPinMux(73, GPIO_MUX_CPU1, 5); //GPIO73 -  CANRXB
    GPIO_SetupPinOptions(73, GPIO_INPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(20, GPIO_MUX_CPU1, 3); //GPIO20 - CANTXB
    GPIO_SetupPinOptions(20, GPIO_OUTPUT, GPIO_PUSHPULL);
    //
    // Initialize the CAN controller
    //

    CAN_initModule(CANA_BASE);
//    CAN_initModule(CANB_BASE);

    //
    // Set up the CAN bus bit rate to 500 kbps
    // Refer to the Driver Library User Guide for information on how to set
    // tighter timing control. Additionally, consult the device data sheet
    // for more information about the CAN module clocking.
    //
    CAN_setBitRate(CANA_BASE, 200000000, 500000, 20);

    CAN_setBitRate(CANB_BASE, 200000000, 500000, 20);

    CAN_disableTestMode(CANA_BASE);  // Disable Test Mode in CANA Module.

    CAN_disableTestMode(CANB_BASE);  // Disable Test Mode in CANA Module.

    CAN_startModule(CANA_BASE);

    CAN_startModule(CANB_BASE);


    GPIO_SetupPinMux(2, GPIO_MUX_CPU1, 6); //GPIO30 -  CANRXA
    GPIO_SetupPinMux(3, GPIO_MUX_CPU1, 6); //GPIO30 -  CANRXA

    per_fnIniti2cB();


}

void init_fnPWM()
{
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0; // This bit reset to 0 stops all PWM clocks.
    init_fnPWM1();                                   // Initialization of EPWMs
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1; // This bit set to 1 turns ON all pwm clocks at the same time
    EDIS;
}

//
// InitEPwm2Example - Initialize EPWM2 configuration - ADC Start of Conversion
//
void init_fnPWM1()
{
    //
    // Setup TBCLK
    //
    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up/down and down
    EPwm1Regs.TBPRD = PER_mPWM1_TBPRD_VALUE;           // Set timer period
    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;        // Disable phase loading
    EPwm1Regs.TBPHS.bit.TBPHS = 0x0000;            // Phase is 0
    EPwm1Regs.TBCTR = 0x0000;                      // Clear counter
    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV1;       // Clock ratio to SYSCLKOUT
    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV1;
    EPwm1Regs.TBCTL.bit.PRDLD = TB_SHADOW;
    //
    // Setup shadow register load on ZERO
    //
    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO_PRD;
    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO_PRD;

    //
    // Set Actions
    //
    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;         // Set PWM2A on period
    EPwm1Regs.AQCTLA.bit.CAD = AQ_SET;       // Clear PWM2A on event B, down
                                             // count

    EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;       // Clear PWM2A on period
    EPwm1Regs.AQCTLB.bit.CBD = AQ_SET;         // Set PWM2A on event A, up

    EPwm1Regs.CMPA.bit.CMPA = 0;

    // Interrupt where we will change the Compare Values
    //
    EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;    // Select INT on Zero event
    EPwm1Regs.ETSEL.bit.INTEN = 1;               // Enable INT
    EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;          // Generate INT on 3rd event

}

void INIT_fnCANAMailBox(void)

{
    // CAN-A RX  message IDs


    // TO RECEIVE CAN FLASHING
    CAN_setupMessageObject(CANA_BASE, CAN_mMAILBOX_1, 0x141101F2,
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_RX, 0,
                           CAN_MSG_OBJ_NO_FLAGS, CANA_mEIGHT_BYTE);

// LPCIO
    CAN_setupMessageObject(
            CANA_BASE, CAN_mMAILBOX_2, CANA_mRX_LPCMSGID1, CAN_MSG_FRAME_EXT,
            CAN_MSG_OBJ_TYPE_RX, 0X1F0FFFF0,
            CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER, //QUERY FOR MASTER OPERATING STATE
            CANA_mEIGHT_BYTE);

    //LHCIO
    CAN_setupMessageObject(
            CANA_BASE, CAN_mMAILBOX_3, CANA_mRX_LHCMSGID1, CAN_MSG_FRAME_EXT,
            CAN_MSG_OBJ_TYPE_RX, 0X1F0FFFF0,
            CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER, //QUERY FOR MASTER OPERATING STATE
            CANA_mEIGHT_BYTE);

    //LHCIO
    CAN_setupMessageObject(
            CANA_BASE, CAN_mMAILBOX_7, CANA_mRX_MSMSGID2, CAN_MSG_FRAME_EXT,
            CAN_MSG_OBJ_TYPE_RX, 0X1F0FFFF0,
            CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER, //QUERY FOR MASTER OPERATING STATE
            CANA_mEIGHT_BYTE);


}

void INIT_fnCANBMailBox(void)

{
    // CAN-B RX  message IDs
    //CANB   CAN RX  message ID's
    CAN_setupMessageObject(
            CANB_BASE, CAN_mMAILBOX_7, 0x14160103, CAN_MSG_FRAME_EXT,
            CAN_MSG_OBJ_TYPE_RX, 0X1FFF00FF,
            CAN_MSG_OBJ_USE_ID_FILTER | CAN_MSG_OBJ_USE_EXT_FILTER, // RUT
            CANA_mTWO_BYTE);
}

/*=============================================================================
 @Function Name : void per_fnIniti2cA(void)
 @brief initialize I2CA module with a bit rate @400khz

 @param void
 @return void
 ============================================================================ */
static void per_fnIniti2cB(void)
{
    // Initialize I2C, I2C clock = 10MHz, I2C Bus Clock = 400KHz
//    I2caRegs.I2CSAR.all = 0x0000;     // Slave address - 0x00

    I2cbRegs.I2CPSC.all = 9;         // Prescaler - need 10 Mhz on module clk
    I2cbRegs.I2CCLKL = 10;            // NOTE: must be non zero
    I2cbRegs.I2CCLKH = 5;             // NOTE: must be non zero
    I2cbRegs.I2CIER.all = 0x00;       // Enable SCD & ARDY __interrupts

    I2cbRegs.I2CMDR.all = 0x0020;     // Take I2C out of reset
                                      // Stop I2C when suspended

    I2cbRegs.I2CFFTX.all = 0x6000;    // Enable FIFO mode and TXFIFO
    I2cbRegs.I2CFFRX.all = 0x2040;    // Enable RXFIFO, clear RXFFINT,

}


/*==============================================================================
 End of File
 ==============================================================================*/
