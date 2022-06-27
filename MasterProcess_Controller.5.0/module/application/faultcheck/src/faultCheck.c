/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  canaEvent.c
 @author DEEPTI.K
 @date 20-Jun-2022

 @brief Description
 ==============================================================================*/

/*==============================================================================
 Includes
 ==============================================================================*/

#include <stdint.h>
#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include "F2837xS_device.h"
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "cana_defs.h"
#include "hal/driverlib/can.h"
#include <stdint.h>
/*==============================================================================
 Defines
 ==============================================================================*/

uint16_t faultSettimer[2][8] = { 0 };
uint16_t faultResettimer[2][8] = { 0 };

uint16_t LPCFaultRegs[2][16] = { 0, 0 };
uint16_t LHCFaultRegs[2][16] = { 0, 0 };

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

/*==============================================================================
 Macros
 ==============================================================================*/
// LPC Faults

#define FAULT_mWLS101              (0U)
#define FAULT_mWLS102              (1U)
#define FAULT_mWLS103              (2U)
#define FAULT_mDOS301              (3U)
#define FAULT_mDOS303              (4U)

// LHC Faults

#define FAULT_mDOS101_103              (0U)
#define FAULT_mDOS301_303              (1U)

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

bool faultLPC_fnSet(uint16_t IOcardType, uint16_t ui16DIFaultType,
                    uint16_t timerValueSet);
bool faultLPC_fnReset(uint16_t IOcardType, uint16_t ui16DIFaultType,
                      uint16_t timerValueReset);

bool faultLHC_fnSet(uint16_t IOcardType, uint16_t ui16DIFaultType,
                    uint16_t timerValueSet);
bool faultLHC_fnReset(uint16_t IOcardType, uint16_t ui16DIFaultType,
                      uint16_t timerValueReset);

/*==============================================================================
 Local Variables
 ==============================================================================*/

void faultCheck(void)
{
    // LPC Cabinet DI Faults

    if (CANA_tzLPCDI_IORegs[CANA_mLPC130_IO].bit.DI_bit0 == 0x0)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC130_IO].bit.WLS_101 = faultLPC_fnSet(
                CANA_mLPC130_IO, FAULT_mWLS101, 2000);

    }
    else if (CANA_tzLPCDI_IORegs[CANA_mLPC130_IO].bit.DI_bit0 == 0x1)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC130_IO].bit.WLS_101 = faultLPC_fnReset(
                CANA_mLPC130_IO, FAULT_mWLS101, 2000);
    }

    if (CANA_tzLPCDI_IORegs[CANA_mLPC130_IO].bit.DI_bit1 == 0x0)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC130_IO].bit.WLS_102 = faultLPC_fnSet(
                CANA_mLPC130_IO, FAULT_mWLS102, 2000);
    }
    else if (CANA_tzLPCDI_IORegs[CANA_mLPC130_IO].bit.DI_bit1 == 0x1)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC130_IO].bit.WLS_102 = faultLPC_fnReset(
                CANA_mLPC130_IO, FAULT_mWLS102, 2000);
    }

    if (CANA_tzLPCDI_IORegs[CANA_mLPC130_IO].bit.DI_bit2 == 0x0)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC130_IO].bit.WLS_103 = faultLPC_fnSet(
                CANA_mLPC130_IO, FAULT_mWLS103, 2000);
    }
    else if (CANA_tzLPCDI_IORegs[CANA_mLPC130_IO].bit.DI_bit2 == 0x1)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC130_IO].bit.WLS_103 = faultLPC_fnReset(
                CANA_mLPC130_IO, FAULT_mWLS103, 2000);
    }

    if (CANA_tzLPCDI_IORegs[CANA_mLPC130_IO].bit.DI_bit3 == 0x01)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC130_IO].bit.DOS_301 = faultLPC_fnSet(
                CANA_mLPC130_IO, FAULT_mDOS301, 2000);
    }
    else if (CANA_tzLPCDI_IORegs[CANA_mLPC130_IO].bit.DI_bit3 == 0x0)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC130_IO].bit.DOS_301 = faultLPC_fnReset(
                CANA_mLPC130_IO, FAULT_mDOS301, 2000);
    }

    if (CANA_tzLPCDI_IORegs[CANA_mLPC130_IO].bit.DI_bit4 == 0x01)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC130_IO].bit.DOS_303 = faultLPC_fnSet(
                CANA_mLPC130_IO, FAULT_mDOS303, 2000);
    }
    else if (CANA_tzLPCDI_IORegs[CANA_mLPC130_IO].bit.DI_bit4 == 0x0)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC130_IO].bit.DOS_303 = faultLPC_fnReset(
                CANA_mLPC130_IO, FAULT_mDOS303, 2000);
    }



    // LHC Cabinet DI Faults

    if ((CANA_tzLHCDI_IORegs[CANA_mLHC110_IO].bit.DI_bit0 == 0x01)
            || (CANA_tzLHCDI_IORegs[CANA_mLHC110_IO].bit.DI_bit1 == 0x01))
    {
        CANA_tzLHCDIFaultRegs[CANA_mLHC110_IO].bit.DOS101_103 = faultLHC_fnSet(
                CANA_mLHC110_IO, FAULT_mDOS101_103, 2000);
    }
    else if ((CANA_tzLHCDI_IORegs[CANA_mLHC110_IO].bit.DI_bit0 == 0x0)
            && (CANA_tzLHCDI_IORegs[CANA_mLHC110_IO].bit.DI_bit1 == 0x0))
    {
       CANA_tzLHCDIFaultRegs[CANA_mLHC110_IO].bit.DOS101_103 = faultLHC_fnReset(
           CANA_mLHC110_IO,FAULT_mDOS101_103, 2000);
    }

    if ((CANA_tzLHCDI_IORegs[CANA_mLHC110_IO].bit.DI_bit2 == 0x01)
            || (CANA_tzLHCDI_IORegs[CANA_mLHC110_IO].bit.DI_bit3 == 0x01))
    {
        CANA_tzLHCDIFaultRegs[CANA_mLHC110_IO].bit.LGT101_402 = faultLHC_fnSet(
          CANA_mLHC110_IO, FAULT_mDOS301_303, 2000);
    }
    else if ((CANA_tzLHCDI_IORegs[CANA_mLHC110_IO].bit.DI_bit2 == 0x0)
            && (CANA_tzLHCDI_IORegs[CANA_mLHC110_IO].bit.DI_bit3 == 0x0))
    {
       CANA_tzLHCDIFaultRegs[CANA_mLHC110_IO].bit.LGT101_402 = faultLHC_fnReset(
        CANA_mLHC110_IO, FAULT_mDOS301_303, 2000);
    }

}

bool faultLPC_fnSet(uint16_t IOcardType, uint16_t ui16DIFaultType,
                    uint16_t timerValueSet)
{
    faultSettimer[0][ui16DIFaultType]++;
    if (faultSettimer[0][ui16DIFaultType] >= timerValueSet)
    {
        faultSettimer[0][ui16DIFaultType] = timerValueSet;
        faultResettimer[0][ui16DIFaultType] = 0;
        LPCFaultRegs[IOcardType][ui16DIFaultType] = 1;
    }
    return LPCFaultRegs[IOcardType][ui16DIFaultType];
}

bool faultLPC_fnReset(uint16_t IOcardType, uint16_t ui16DIFaultType,
                      uint16_t timerValueReset)
{
    faultResettimer[0][ui16DIFaultType]++;
    if (faultResettimer[0][ui16DIFaultType] >= timerValueReset)
    {
        faultResettimer[0][ui16DIFaultType] = timerValueReset;
        faultSettimer[0][ui16DIFaultType] = 0;
        LPCFaultRegs[IOcardType][ui16DIFaultType] = 0;
    }
    return LPCFaultRegs[IOcardType][ui16DIFaultType];
}

bool faultLHC_fnSet(uint16_t IOcardType, uint16_t ui16DIFaultType,
                    uint16_t timerValueSet)
{
    faultSettimer[1][ui16DIFaultType]++;
    if (faultSettimer[1][ui16DIFaultType] >= timerValueSet)
    {
        faultSettimer[1][ui16DIFaultType] = timerValueSet;
        faultResettimer[1][ui16DIFaultType] = 0;
        LHCFaultRegs[IOcardType][ui16DIFaultType] = 1;
    }
    return LHCFaultRegs[IOcardType][ui16DIFaultType];
}

bool faultLHC_fnReset(uint16_t IOcardType, uint16_t ui16DIFaultType,
                      uint16_t timerValueReset)
{
    faultResettimer[1][ui16DIFaultType]++;
    if (faultResettimer[1][ui16DIFaultType] >= timerValueReset)
    {
        faultResettimer[1][ui16DIFaultType] = timerValueReset;
        faultSettimer[1][ui16DIFaultType] = 0;
        LHCFaultRegs[IOcardType][ui16DIFaultType] = 0;
    }
    return LHCFaultRegs[IOcardType][ui16DIFaultType];
}
/*==============================================================================
 End of File
 ==============================================================================*/
