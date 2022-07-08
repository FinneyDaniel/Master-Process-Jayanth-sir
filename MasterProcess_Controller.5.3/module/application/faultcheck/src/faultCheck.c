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
#include "mathcalc.h"
#include "control_defs.h"

/*==============================================================================
 Defines
 ==============================================================================*/

uint16_t faultSettimer[2][8] = { 0 };
uint16_t faultResettimer[2][8] = { 0 };
uint16_t faultSettimer1[2][8] = { 0 };
uint16_t faultResettimer1[2][8] = { 0 };
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
#define FAULT_mHYS101              (5U)
#define FAULT_mHYS501              (6U)
#define FAULT_mHYS401              (7U)
#define FAULT_mHYS102_SHUTDOWN     (8U)
#define FAULT_mHYS102_RAMPDOWN     (9U)
#define FAULT_mOXS101_SHUTDOWN     (10U)
#define FAULT_mOXS101_RAMPDOWN     (11U)

#define FAULT_mVFD_PMPFAIL         (12U)


// LHC Faults

#define FAULT_mDOS101_103          (0U)
#define FAULT_mDOS301_303          (1U)

#define FAULT_mLVL101              (6U)
#define FAULT_mPRT101              (7U)
#define FAULT_mPRT102              (8U)
#define FAULT_mCOS101              (9U)





/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

bool faultLPC_fnSet(uint16_t IOcardType, uint16_t ui16DIFaultType,
                    uint16_t timerValueSet);
bool faultLPC_fnReset(uint16_t IOcardType, uint16_t ui16DIFaultType,
                      uint16_t timerValueReset);

bool faultLHC_fnSet(uint16_t IOcardType1, uint16_t ui16DIFaultType1,
                    uint16_t timerValueSet1);
bool faultLHC_fnReset(uint16_t IOcardType1, uint16_t ui16DIFaultType1,
                      uint16_t timerValueReset1);

/*==============================================================================
 Local Variables
 ==============================================================================*/

void faultCheck(void)
{
    // LPC Cabinet DI Faults

    if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit0 == 0x0)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC30_IO].bit.WLS_101 = faultLPC_fnSet(
                CANA_mLPC30_IO, FAULT_mWLS101, 20);

    }
    else if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit0 == 0x1)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC30_IO].bit.WLS_101 = faultLPC_fnReset(
                CANA_mLPC30_IO, FAULT_mWLS101, 20);
    }

    if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit1 == 0x0)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC30_IO].bit.WLS_102 = faultLPC_fnSet(
                CANA_mLPC30_IO, FAULT_mWLS102, 20);
    }
    else if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit1 == 0x1)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC30_IO].bit.WLS_102 = faultLPC_fnReset(
                CANA_mLPC30_IO, FAULT_mWLS102, 20);
    }

    if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit2 == 0x0)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC30_IO].bit.WLS_103 = faultLPC_fnSet(
                CANA_mLPC30_IO, FAULT_mWLS103, 20);
    }
    else if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit2 == 0x1)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC30_IO].bit.WLS_103 = faultLPC_fnReset(
                CANA_mLPC30_IO, FAULT_mWLS103, 20);
    }

    if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit3 == 0x01)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC30_IO].bit.DOS_301 = faultLPC_fnSet(
                CANA_mLPC30_IO, FAULT_mDOS301, 20);
    }
    else if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit3 == 0x0)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC30_IO].bit.DOS_301 = faultLPC_fnReset(
                CANA_mLPC30_IO, FAULT_mDOS301, 20);
    }

    if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit4 == 0x01)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC30_IO].bit.DOS_303 = faultLPC_fnSet(
                CANA_mLPC30_IO, FAULT_mDOS303, 20);
    }
    else if (CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].bit.DI_bit4 == 0x0)
    {
        CANA_tzLPCDIFaultRegs[CANA_mLPC30_IO].bit.DOS_303 = faultLPC_fnReset(
                CANA_mLPC30_IO, FAULT_mDOS303, 20);
    }

    // Analog Faults of LPC

    if (MATHConvtzRegs.AISensorHYS101 >= 25.0)
    {
        CANA_tzLPCAIFaultRegs[CANA_mLPC31_IO].bit.HYS_101 = faultLPC_fnSet(
                CANA_mLPC31_IO, FAULT_mHYS101, 20);
    }
    else if (MATHConvtzRegs.AISensorHYS101 <= 23.0)
    {
        CANA_tzLPCAIFaultRegs[CANA_mLPC31_IO].bit.HYS_101 = faultLPC_fnReset(
                CANA_mLPC31_IO, FAULT_mHYS101, 20);
    }

    if (MATHConvtzRegs.AISensorHYS501 >= 25.0)
    {
        CANA_tzLPCAIFaultRegs[CANA_mLPC31_IO].bit.HYS_501 = faultLPC_fnSet(
                CANA_mLPC31_IO, FAULT_mHYS501, 20);
    }
    else if (MATHConvtzRegs.AISensorHYS501 <= 23.0)
    {
        CANA_tzLPCAIFaultRegs[CANA_mLPC31_IO].bit.HYS_501 = faultLPC_fnReset(
                CANA_mLPC31_IO, FAULT_mHYS501, 20);
    }

    if (MATHConvtzRegs.AISensorHYS401 >= 25.0)
    {
        CANA_tzLPCAIFaultRegs[CANA_mLPC31_IO].bit.HYS_401 = faultLPC_fnSet(
                CANA_mLPC31_IO, FAULT_mHYS401, 20);
    }
    else if (MATHConvtzRegs.AISensorHYS401 <= 23.0)
    {
        CANA_tzLPCAIFaultRegs[CANA_mLPC31_IO].bit.HYS_401 = faultLPC_fnReset(
                CANA_mLPC31_IO, FAULT_mHYS401, 20);
    }

    if (MATHConvtzRegs.AISensorHYS102 >= 75.0)
    {

        CANA_tzLPCAIFaultRegs[CANA_mLPC31_IO].bit.HYS_102_ShtDwn =
                faultLPC_fnSet(
                CANA_mLPC31_IO,
                               FAULT_mHYS102_SHUTDOWN, 20);
        CANA_tzLPCAIFaultRegs[CANA_mLPC31_IO].bit.HYS_102_RmpDwn =
                faultLPC_fnReset(
                CANA_mLPC31_IO,
                                 FAULT_mHYS102_RAMPDOWN, 20);

    }

    else if ((MATHConvtzRegs.AISensorHYS102 >= 50.0)
            && (MATHConvtzRegs.AISensorHYS102 < 72.0))
    {
        CANA_tzLPCAIFaultRegs[CANA_mLPC31_IO].bit.HYS_102_ShtDwn =
                faultLPC_fnReset(
                CANA_mLPC31_IO,
                                 FAULT_mHYS102_SHUTDOWN, 20);
        CANA_tzLPCAIFaultRegs[CANA_mLPC31_IO].bit.HYS_102_RmpDwn =
                faultLPC_fnSet(
                CANA_mLPC31_IO,
                               FAULT_mHYS102_RAMPDOWN, 20);
    }
    else if (MATHConvtzRegs.AISensorHYS102 < 48.0)
    {
        CANA_tzLPCAIFaultRegs[CANA_mLPC31_IO].bit.HYS_102_RmpDwn =
                faultLPC_fnReset(
                CANA_mLPC31_IO,
                                 FAULT_mHYS102_RAMPDOWN, 20);
        CANA_tzLPCAIFaultRegs[CANA_mLPC31_IO].bit.HYS_102_ShtDwn =
                faultLPC_fnReset(
                CANA_mLPC31_IO,
                                 FAULT_mHYS102_SHUTDOWN, 20);
    }

    if (MATHConvtzRegs.AISensorOXS101 > 25.0)
    {
        CANA_tzLPCAIFaultRegs[CANA_mLPC31_IO].bit.OXS_101_RmpDwn =
                faultLPC_fnReset(
                CANA_mLPC31_IO,
                                 FAULT_mOXS101_RAMPDOWN, 20);
        CANA_tzLPCAIFaultRegs[CANA_mLPC31_IO].bit.OXS_101_ShtDwn =
                faultLPC_fnSet(
                CANA_mLPC31_IO,
                               FAULT_mOXS101_SHUTDOWN, 20);
    }
    else if ((MATHConvtzRegs.AISensorOXS101 >= 23.0)
            && (MATHConvtzRegs.AISensorOXS101 <= 24.8))
    {
        CANA_tzLPCAIFaultRegs[CANA_mLPC31_IO].bit.OXS_101_RmpDwn =
                faultLPC_fnSet(
                CANA_mLPC31_IO,
                               FAULT_mOXS101_RAMPDOWN, 20);
        CANA_tzLPCAIFaultRegs[CANA_mLPC31_IO].bit.OXS_101_ShtDwn =
                faultLPC_fnReset(
                CANA_mLPC31_IO,
                                 FAULT_mOXS101_SHUTDOWN, 20);
    }
    else if (MATHConvtzRegs.AISensorOXS101 <= 22.5)
    {
        CANA_tzLPCAIFaultRegs[CANA_mLPC31_IO].bit.OXS_101_RmpDwn =
                faultLPC_fnReset(
                CANA_mLPC31_IO,
                                 FAULT_mOXS101_RAMPDOWN, 20);
        CANA_tzLPCAIFaultRegs[CANA_mLPC31_IO].bit.OXS_101_ShtDwn =
                faultLPC_fnReset(
                CANA_mLPC31_IO,
                                 FAULT_mOXS101_SHUTDOWN, 20);
    }

    // LHC Cabinet DI Faults

    if ((CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit0 == 0x01)
            || (CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit1 == 0x01))
    {
        CANA_tzLHCDIFaultRegs[CANA_mLHC10_IO].bit.DOS101_103 = faultLHC_fnSet(
                CANA_mLHC10_IO, FAULT_mDOS101_103, 20);
    }
    else if ((CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit0 == 0x0)
            && (CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit1 == 0x0))
    {
        CANA_tzLHCDIFaultRegs[CANA_mLHC10_IO].bit.DOS101_103 = faultLHC_fnReset(
                CANA_mLHC10_IO, FAULT_mDOS101_103, 20);
    }

    if ((CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit2 == 0x01)
            || (CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit3 == 0x01))
    {
        CANA_tzLHCDIFaultRegs[CANA_mLHC10_IO].bit.LGT101_402 = faultLHC_fnSet(
                CANA_mLHC10_IO, FAULT_mDOS301_303, 20);
    }
    else if ((CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit2 == 0x0)
            && (CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].bit.DI_bit3 == 0x0))
    {
        CANA_tzLHCDIFaultRegs[CANA_mLHC10_IO].bit.LGT101_402 = faultLHC_fnReset(
                CANA_mLHC10_IO, FAULT_mDOS301_303, 20);
    }


    // LHC Cabinet AI Faults

    if (MATHConvtzRegs.AISensorLVL101 <= 60.0)
    {
        CANA_tzLHCAIFaultRegs[CANA_mLHC10_IO].bit.LVL_101 = faultLHC_fnSet(
                CANA_mLHC10_IO, FAULT_mLVL101, 20);
    }
    else if (MATHConvtzRegs.AISensorLVL101 >= 90.0)
    {
        CANA_tzLHCAIFaultRegs[CANA_mLHC10_IO].bit.LVL_101 = faultLHC_fnReset(
                CANA_mLHC10_IO, FAULT_mLVL101, 20);
    }

    if (MATHConvtzRegs.AISensorPRT101 >= 0.5)
    {
        CANA_tzLHCAIFaultRegs[CANA_mLHC10_IO].bit.PRT_101 = faultLHC_fnSet(
                CANA_mLHC10_IO, FAULT_mPRT101, 20);
    }
    else if (MATHConvtzRegs.AISensorPRT101 <= 0.2)
    {
        CANA_tzLHCAIFaultRegs[CANA_mLHC10_IO].bit.PRT_101 = faultLHC_fnReset(
                CANA_mLHC10_IO, FAULT_mPRT101, 20);
    }


    if (MATHConvtzRegs.AISensorPRT102 > 1.15)
    {
        CANA_tzLHCAIFaultRegs[CANA_mLHC10_IO].bit.PRT_102 = faultLHC_fnReset(
                CANA_mLHC10_IO, FAULT_mPRT102, 20);
        CONTROLtzFaultRegs.bit.VFDPumpFail = faultLPC_fnReset(
                CANA_mLPC30_IO, FAULT_mVFD_PMPFAIL, 20);
    }
    else if ((MATHConvtzRegs.AISensorPRT102 >= 0.51) && (MATHConvtzRegs.AISensorPRT102 <= 1.1))
    {
        CANA_tzLHCAIFaultRegs[CANA_mLHC10_IO].bit.PRT_102 = faultLHC_fnSet(
                CANA_mLHC10_IO, FAULT_mPRT102, 20);
        CONTROLtzFaultRegs.bit.VFDPumpFail = faultLPC_fnReset(
                CANA_mLPC30_IO, FAULT_mVFD_PMPFAIL, 20);
    }
    else if(MATHConvtzRegs.AISensorPRT102 < 0.5)
    {
        CONTROLtzFaultRegs.bit.VFDPumpFail = faultLPC_fnSet(
                CANA_mLPC30_IO, FAULT_mVFD_PMPFAIL, 20);
        CANA_tzLHCAIFaultRegs[CANA_mLHC10_IO].bit.PRT_102 = faultLHC_fnReset(
                 CANA_mLHC10_IO, FAULT_mPRT102, 20);

    }


    if (MATHConvtzRegs.AISensorCOS101 >= 3.0)
    {
        CANA_tzLHCAIFaultRegs[CANA_mLHC10_IO].bit.COS_101 = faultLHC_fnSet(
                CANA_mLHC10_IO, FAULT_mCOS101, 20);
    }
    else if (MATHConvtzRegs.AISensorCOS101 <= 2.0)
    {
        CANA_tzLHCAIFaultRegs[CANA_mLHC10_IO].bit.COS_101 = faultLHC_fnReset(
                CANA_mLHC10_IO, FAULT_mCOS101, 20);
    }

}

bool faultLPC_fnSet(uint16_t IOcardType, uint16_t ui16DIFaultType,
                    uint16_t timerValueSet)
{
    if (IOcardType == 0)
    {
        faultSettimer[0][ui16DIFaultType]++;
        if (faultSettimer[0][ui16DIFaultType] >= timerValueSet)
        {
            faultSettimer[0][ui16DIFaultType] = timerValueSet;
            faultResettimer[0][ui16DIFaultType] = 0;
            LPCFaultRegs[0][ui16DIFaultType] = 1;
        }

    }
    else if (IOcardType == 1)
    {
        faultSettimer[1][ui16DIFaultType]++;
        if (faultSettimer[1][ui16DIFaultType] >= timerValueSet)
        {
            faultSettimer[1][ui16DIFaultType] = timerValueSet;
            faultResettimer[1][ui16DIFaultType] = 0;
            LPCFaultRegs[1][ui16DIFaultType] = 1;
        }

    }

    return LPCFaultRegs[IOcardType][ui16DIFaultType];

}

bool faultLPC_fnReset(uint16_t IOcardType, uint16_t ui16DIFaultType,
                      uint16_t timerValueReset)
{
    if (IOcardType == 0)
    {
        faultResettimer[0][ui16DIFaultType]++;
        if (faultResettimer[0][ui16DIFaultType] >= timerValueReset)
        {
            faultResettimer[0][ui16DIFaultType] = timerValueReset;
            faultSettimer[0][ui16DIFaultType] = 0;
            LPCFaultRegs[0][ui16DIFaultType] = 0;
        }
    }
    else if (IOcardType == 1)
    {
        faultResettimer[1][ui16DIFaultType]++;
        if (faultResettimer[1][ui16DIFaultType] >= timerValueReset)
        {
            faultResettimer[1][ui16DIFaultType] = timerValueReset;
            faultSettimer[1][ui16DIFaultType] = 0;
            LPCFaultRegs[1][ui16DIFaultType] = 0;
        }
    }
    return LPCFaultRegs[IOcardType][ui16DIFaultType];
}

bool faultLHC_fnSet(uint16_t IOcardType1, uint16_t ui16DIFaultType1,
                    uint16_t timerValueSet1)
{
    if (IOcardType1 == 0)
    {
        faultSettimer1[0][ui16DIFaultType1]++;
        if (faultSettimer1[0][ui16DIFaultType1] >= timerValueSet1)
        {
            faultSettimer1[0][ui16DIFaultType1] = timerValueSet1;
            faultResettimer1[0][ui16DIFaultType1] = 0;
            LHCFaultRegs[0][ui16DIFaultType1] = 1;
        }
    }
    else if (IOcardType1 == 1)
    {
        faultSettimer1[1][ui16DIFaultType1]++;
        if (faultSettimer1[1][ui16DIFaultType1] >= timerValueSet1)
        {
            faultSettimer1[1][ui16DIFaultType1] = timerValueSet1;
            faultResettimer1[1][ui16DIFaultType1] = 0;
            LHCFaultRegs[1][ui16DIFaultType1] = 1;
        }
    }
    return LHCFaultRegs[IOcardType1][ui16DIFaultType1];
}

bool faultLHC_fnReset(uint16_t IOcardType1, uint16_t ui16DIFaultType1,
                      uint16_t timerValueReset1)
{

    if (IOcardType1 == 0)
    {

        faultResettimer1[0][ui16DIFaultType1]++;
        if (faultResettimer1[0][ui16DIFaultType1] >= timerValueReset1)
        {
            faultResettimer1[0][ui16DIFaultType1] = timerValueReset1;
            faultSettimer1[0][ui16DIFaultType1] = 0;
            LHCFaultRegs[0][ui16DIFaultType1] = 0;
        }
    }
    else if (IOcardType1 == 1)
    {
        faultResettimer1[1][ui16DIFaultType1]++;
        if (faultResettimer1[1][ui16DIFaultType1] >= timerValueReset1)
        {
            faultResettimer1[1][ui16DIFaultType1] = timerValueReset1;
            faultSettimer1[1][ui16DIFaultType1] = 0;
            LHCFaultRegs[1][ui16DIFaultType1] = 0;
        }
    }
    return LHCFaultRegs[IOcardType1][ui16DIFaultType1];
}
/*==============================================================================
 End of File
 ==============================================================================*/
