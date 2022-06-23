/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  init_gpio.c
 @author DEEPTI.K
 @date 06-Sep-2021

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include <module/application/init_gpio/inc/init_gpio.h>
#include <stdint.h>
#include "F28x_Project.h"
#include <stdint.h>
#include <stdbool.h>
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

void GPIO_fnInit(void);
void gpio_fnInitSYS(void);
void gpio_fnInitPWM1(void);

/*==============================================================================
 Local Variables
 ==============================================================================*/

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @brief Initialization of GPIO's done here

 @param void
 @return void
 ============================================================================ */

void INIT_fnGPIO()
{
    gpio_fnInitSYS();                     // System GPIO Initialize
    gpio_fnInitPWM1();                      // ADC Start of Conversion

}

/*=============================================================================
 @Function name : void gpio_fnInitPWM1(void)
 @brief configures required GPIO's for System functionality
 @param void
 @return void
 ============================================================================ */

void gpio_fnInitSYS(void)
{
    EALLOW;

//    // ISR Execution check LED
    GpioCtrlRegs.GPCGMUX2.bit.GPIO85 = GPIO_mGPIOFUNCTION;  // Configure as GPIO
    GpioCtrlRegs.GPCDIR.bit.GPIO85 = 1;

    GpioCtrlRegs.GPCGMUX2.bit.GPIO86 = GPIO_mGPIOFUNCTION;  // Configure as GPIO
    GpioCtrlRegs.GPCDIR.bit.GPIO86 = 1;

    GpioCtrlRegs.GPCGMUX2.bit.GPIO92 = GPIO_mGPIOFUNCTION;  // Configure as GPIO
    GpioCtrlRegs.GPCDIR.bit.GPIO92 = 1;

    EDIS;
}

/*=============================================================================
 @Function name : void gpio_fnInitPWM1(void)
 @brief configures required GPIO's for PWM1
 @param void
 @return void
 ============================================================================ */

void gpio_fnInitPWM1(void)
{
    EALLOW;

    //
    GpioCtrlRegs.GPAPUD.bit.GPIO0 = GPIO_mDISABLE_PULLUP; // Disable pull-up on GPIO0 (EPWM1A)
    GpioCtrlRegs.GPAPUD.bit.GPIO1 = GPIO_mDISABLE_PULLUP; // Disable pull-up on GPIO1 (EPWM1B)

    //
    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = GPIO_mGPIOFUNCTION; // Configure GPIO0 as EPWM1A
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = GPIO_mGPIOFUNCTION; // Configure GPIO1 as EPWM1B

    EDIS;
}

//void gpio_fnInitSYS(void)
//{
////    EALLOW;
////    // ISR Execution check LED
////    GpioCtrlRegs.GPCGMUX2.bit.GPIO85 = GPIO_mFUNCTION;      // Configure as GPIO
////    GpioCtrlRegs.GPCDIR.bit.GPIO85 = 1;
//
////    // ISR Execution check LED
////    GpioCtrlRegs.GPAGMUX1.bit.GPIO0 = GPIO_mFUNCTION;       // Configure as GPIO
////    GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;
////
////    GpioCtrlRegs.GPAGMUX1.bit.GPIO1 = GPIO_mFUNCTION;       // Configure as GPIO
////    GpioCtrlRegs.GPADIR.bit.GPIO1 = 1;
////
////    GpioCtrlRegs.GPAGMUX1.bit.GPIO2 = GPIO_mFUNCTION;       // Configure as GPIO
////    GpioCtrlRegs.GPADIR.bit.GPIO2 = 1;
////
////    GpioCtrlRegs.GPAGMUX1.bit.GPIO3 = GPIO_mFUNCTION;       // Configure as GPIO
////    GpioCtrlRegs.GPADIR.bit.GPIO3 = 1;
////
////    GpioCtrlRegs.GPAGMUX1.bit.GPIO4 = GPIO_mFUNCTION;       // Configure as GPIO
////    GpioCtrlRegs.GPADIR.bit.GPIO4 = 1;
////
////    GpioCtrlRegs.GPCGMUX2.bit.GPIO86 = GPIO_mFUNCTION;      // Configure as GPIO
////    GpioCtrlRegs.GPCDIR.bit.GPIO86 = 1;
//
//  //  EDIS;
//}
///*=============================================================================
// @Function name : void gpio_fnInitPWM1(void)
// @brief configures GPIO's based on PWM1 functionality
//
// @param void
// @return void
// ============================================================================ */
//void gpio_fnInitPWM1(void)
//{
//
//    //
//    GpioCtrlRegs.GPAPUD.bit.GPIO0 = GPIO_mDISABLE_PULLUP; // Disable pull-up on GPIO0 (EPWM1A)
//    GpioCtrlRegs.GPAPUD.bit.GPIO1 = GPIO_mDISABLE_PULLUP; // Disable pull-up on GPIO1 (EPWM1B)
//
//    //
//    GpioCtrlRegs.GPAMUX1.bit.GPIO0 = GPIO_mGPIOFUNCTION; // Configure GPIO0 as EPWM1A
//    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = GPIO_mGPIOFUNCTION; // Configure GPIO1 as EPWM1B
//    //
//
//}
///*=============================================================================
// @Function name : void gpio_fnInitCANA(void)
// @brief configures GPIO's based on CANA functionality
//
// @param void
// @return void
// ============================================================================ */
//void gpio_fnInitCANA(void)
//{
//    //
//    // Enable CAN-A on GPIO62 - GPIO63
//    //
//    //
//    // Setup GPIO pin mux for CAN-A TX/RX and CAN-B TX/RX
//    //
//    GPIO_SetupPinMux(62, GPIO_MUX_CPU1, 6); //GPIO62 -  CANRXA
//    GPIO_SetupPinOptions(62, GPIO_INPUT, GPIO_ASYNC);
//    GPIO_SetupPinMux(63, GPIO_MUX_CPU1, 6); //GPIO63 - CANTXA
//    GPIO_SetupPinOptions(63, GPIO_OUTPUT, GPIO_PUSHPULL);
//
//}
//
///*=============================================================================
// @Function name : void gpio_fnInitCANB(void)
// @brief configures GPIO's based on CANB functionality
//
// @param void
// @return void
// ============================================================================ */
//void gpio_fnInitCANB(void)
//{
//    //
//    // Enable CAN-B on GPIO73 - GPIO20
//    //
//    //
//    // Setup GPIO pin mux for CAN-A TX/RX and CAN-B TX/RX
//    //
//    GPIO_SetupPinMux(73, GPIO_MUX_CPU1, 5); //GPIO73 -  CANRXB
//    GPIO_SetupPinOptions(73, GPIO_INPUT, GPIO_ASYNC);
//    GPIO_SetupPinMux(20, GPIO_MUX_CPU1, 3); //GPIO20 - CANTXB
//    GPIO_SetupPinOptions(20, GPIO_OUTPUT, GPIO_PUSHPULL);
//
//}

/*==============================================================================
 End of File
 ==============================================================================*/
