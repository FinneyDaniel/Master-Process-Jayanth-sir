/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  canaEvent.c
 @author OHMLAP0042
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
#include "state_machine.h"
#include "control_defs.h"
/*==============================================================================
 Defines
 ==============================================================================*/

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

CANA_tzIOREGS CANA_tzIORegs;
CANA_tzIOFLAGS CANA_tzIOflags;
CANA_tzTIMERIOREGS CANA_tzIOtimers;
CANA_tzMSREGS CANA_tzMSRegs;

CANA_tzAI_IOREGS CANA_tzAIData_IORegs[CANA_mTOTAL_IONODE];
CANA_tzDIFREQ_IOREGS CANA_tzAIDataFreq_IORegs[CANA_mTOTAL_IONODE];

CANA_tzTHERMAL_IOREGS CANA_tzThermal_IORegs[CANA_mTOTAL_IONODE];
CANA_tzAISENSOR_DATA CANA_tzAISensorData;
CANA_tzDISENSOR_DATA CANA_tzDISensorData;

CANA_tzTHERMOCOUPLE_DATA CANA_tzThermoCoupleData;

union CANA_tzDI_IOREGS CANA_tzLPCDI_IORegs[CANA_mTOTAL_LPCNODES],
        CANA_tzLHCDI_IORegs[CANA_mTOTAL_LHCNODES];
union CANA_tzAIFLT_IOREGS CANA_tzLPCAIFlt_IORegs[CANA_mTOTAL_IONODE],
        CANA_tzLHCAIFlt_IORegs[CANA_mTOTAL_IONODE];

union CANA_tzLPCIO1_DIFLT_IOREGS CANA_tzLPCIO1_DIFaultRegs;
union CANA_tzLPCIO2_DIFLT_IOREGS CANA_tzLPCIO2_DIFaultRegs;

union CANA_tzLHCIO1_DIFLT_IOREGS CANA_tzLHCIO1_DIFaultRegs;
union CANA_tzLHCIO2_DIFLT_IOREGS CANA_tzLHCIO2_DIFaultRegs;

union CANA_tzLPCIO1_AIFLT_IOREGS CANA_tzLPCIO1_AIFaultRegs;
union CANA_tzLPCIO2_AIFLT_IOREGS CANA_tzLPCIO2_AIFaultRegs;

union CANA_tzLHCIO1_AIFLT_IOREGS CANA_tzLHCIO1_AIFaultRegs;
union CANA_tzLHCIO2_AIFLT_IOREGS CANA_tzLHCIO2_AIFaultRegs;

union CANA_tzTHERMALFLT_IOREGS CANA_tzThermalFaultRegs;

CANA_tzDO_IOREGS CANA_tzSetDO_IORegs;

can_tzAnaOPParams CANA_tzAnaOPParams;

//can_tzDigOPParams CANA_tzDigOPParams;

CANA_tzDIG_OP CANA_tzDO[2][2];

/*==============================================================================
 Macros
 ==============================================================================*/

CIRC_BUF_DEF(uiRxbufferLPCIO, 20);
CIRC_BUF_DEF(uiRxbufferLHCIO, 20);
CIRC_BUF_DEF(uiRxbufferMS, 20);

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void CANA_fnInitMBox(void);
void CANA_fnRXevent(void);
void CANA_fnTask(void);
void CANA_fnTx(void);

static void cana_fnmsgPrcsLPCIO(uint16_t uimsgID, uint16_t *msgData,
                                uint16_t uiNodeType);

static void cana_fnmsgPrcsLHCIO(uint16_t uimsgID, uint16_t *msgData,
                                uint16_t uiNodeType);

static void cana_fnmsgPrcsMS(uint16_t *msgDataMS);

static void canaTX_fnMS();

bool can_fnEnquedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t msgID,
                     uint16_t DLC);
bool can_fndequedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t *msgID,
                     uint16_t *DLC);
//void CANA_fnCmdsForDigIOs(uint16_t ui16unitID, uint16_t ui16cab_ID,
//                          uint16_t ui16nodeID, uint16_t digitalIO);

//void CANA_fnCmdsForDigOPs(uint16_t ui16unitID, uint16_t ui16cab_ID,
//                           uint16_t ui16nodeID, can_tzDigOPParams *ptrDigOP);
void CANA_fnCmdsForDigOPs(uint16_t ui16unitID, uint16_t ui16cab_ID,
                          uint16_t ui16nodeID, CANA_tzDIG_OP *ptrDigOP);

void CANA_fnCmdsForAnaOPVs(uint16_t ui16unitID, uint16_t ui16cab_ID,
                           uint16_t ui16nodeID, can_tzAnaOPParams *ptrAO_V);

void CANA_fnCmdsForAnaOPIs(uint16_t ui16unitID, uint16_t ui16cab_ID,
                           uint16_t ui16nodeID, can_tzAnaOPParams *ptrAO_I);

/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t uirxMsgLPCIO[8] = { 0 };
uint16_t uirxMsgLHCIO[8] = { 0 };

uint16_t uirxMsgMS[8] = { 0 };
uint16_t ui16txMsgDataIO[8] = { 0 };
uint16_t uirxPrcsMsgLPCIO[8] = { 0 };
uint16_t uirxPrcsMsgLHCIO[8] = { 0 };
uint16_t uirxPrcsMsgMS[8] = { 0 };

uint32_t u32msgID = 0;
uint16_t uiDataLength = 0;
uint16_t uiMsgtype = 0, uiNodeType = 0;
uint16_t uiCANtxMsgDataMS[8] = { 0 };
uint16_t ui16CabID = 0, ui16prev_value = 0;

/*==============================================================================
 Local Constants
 ==============================================================================*/

/*=============================================================================
 @Function name : void can_fnEnquedata(void)
 @brief  function to add data to queue

 @param void
 @return void
 ============================================================================ */

bool can_fnEnquedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t msgID,
                     uint16_t DLC)
{

    int16_t i_count;

    i_count = ptr->i_head + 1;

    if (i_count >= ptr->i_maxlen)
    {
        i_count = 0;
    }

    if (i_count == ptr->i_tail)
        return false;  // buffer is full

    memcpy(ptr->canA_tzRxC_buffer[ptr->i_head].uiDataBuffer, data, 8);
    ptr->canA_tzRxC_buffer[ptr->i_head].u32_msgID = msgID;
    ptr->canA_tzRxC_buffer[ptr->i_head].i_DataLength = (uint16_t) DLC;
    ptr->i_head = i_count;

    return true;
}

/*=============================================================================
 @Function name : void can_fndequedata(void)
 @brief  function to Delete data from queue

 @param void
 @return void
 ============================================================================ */
bool can_fndequedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t *msgID,
                     uint16_t *DLC)
{
    int16_t i_count;

    if (ptr->i_head == ptr->i_tail)
    {
        return false; // buffer is empty
    }

    i_count = ptr->i_tail + 1;

    if (i_count >= ptr->i_maxlen)
    {
        i_count = 0;
    }

    memcpy(data, ptr->canA_tzRxC_buffer[ptr->i_tail].uiDataBuffer, 8);
    *msgID = ptr->canA_tzRxC_buffer[ptr->i_tail].u32_msgID;
    *DLC = ptr->canA_tzRxC_buffer[ptr->i_tail].i_DataLength;
    ptr->i_tail = i_count;

    return true;

}

/*=============================================================================
 @Function name : void CAN_fnRXevent(void)
 @brief  function to receive messages over CAN

 @param void
 @return void
 ============================================================================ */
void CANA_fnRXevent(void)
{

    //any commands for AO/DO control
    if (CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_3))
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_3, uirxMsgLPCIO);

        can_fnEnquedata(&uiRxbufferLPCIO, uirxMsgLPCIO,
                        CanaRegs.CAN_IF2ARB.bit.ID,
                        CanaRegs.CAN_IF2MCTL.bit.DLC);
    }

    if (CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_4))
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_4, uirxMsgLHCIO);

        can_fnEnquedata(&uiRxbufferLHCIO, uirxMsgLHCIO,
                        CanaRegs.CAN_IF2ARB.bit.ID,
                        CanaRegs.CAN_IF2MCTL.bit.DLC);
    }

    if (CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_11))
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_11, uirxMsgMS);

        can_fnEnquedata(&uiRxbufferMS, uirxMsgMS, CanaRegs.CAN_IF2ARB.bit.ID,
                        CanaRegs.CAN_IF2MCTL.bit.DLC);
    }

}

/*=============================================================================
 @Function name : void CAN_fnRXevent(void)
 @brief  function to receive messages over CAN

 @param void
 @return void
 ============================================================================ */
void CANA_fnTask(void)
{

    uint32_t ui32temp;

    can_fndequedata(&uiRxbufferLPCIO, uirxPrcsMsgLPCIO, &u32msgID,
                    &uiDataLength);

    //extracting msgID for individual messages of LPC

    ui32temp = (u32msgID & 0x00F00F0F);
    CANA_tzIORegs.uiMsgtypeLPCIO = (uint16_t) ((ui32temp & 0x00F00000) >> 20);
    CANA_tzIORegs.uiUnitID = (uint16_t) ((ui32temp & 0x00000F00) >> 8);

    CANA_tzIORegs.uiNodeLPCIO = (uint16_t) (ui32temp & 0x0F);

    //processing received messages of LPC

    cana_fnmsgPrcsLPCIO(CANA_tzIORegs.uiMsgtypeLPCIO, uirxPrcsMsgLPCIO,
                        CANA_tzIORegs.uiNodeLPCIO);

    can_fndequedata(&uiRxbufferLHCIO, uirxPrcsMsgLHCIO, &u32msgID,
                    &uiDataLength);

    //extracting msgID for individual messages of LHC

    ui32temp = (u32msgID & 0x00F0000F);
    CANA_tzIORegs.uiMsgtypeLHCIO = (uint16_t) (ui32temp >> 20);
    CANA_tzIORegs.uiNodeLHCIO = (uint16_t) (ui32temp & 0x0F);

    cana_fnmsgPrcsLHCIO(CANA_tzIORegs.uiMsgtypeLHCIO, uirxPrcsMsgLHCIO,
                        CANA_tzIORegs.uiNodeLHCIO);

    can_fndequedata(&uiRxbufferMS, uirxPrcsMsgMS, &u32msgID, &uiDataLength);

    cana_fnmsgPrcsMS(uirxPrcsMsgMS);

    //can transmit event

//
}

/*=============================================================================
 @Function name : void can_fnmsgProcess(void)
 @brief  function to Process LPCIO messages over CAN

 @param void
 @return void
 ============================================================================ */
static void cana_fnmsgPrcsLPCIO(uint16_t uiMsgtype, uint16_t *msgDataIO,
                                uint16_t uiNodeType)

{
    switch (uiMsgtype)
    {
    case IO_DI_MSGID:

        switch (uiNodeType)
        {

        case LPC_30:

            if (CANA_tzIOtimers.RxCntLPC30 != msgDataIO[0])
            {
                CANA_tzIOtimers.RxCntLPC30 = msgDataIO[0];

                if (msgDataIO[1] == msgDataIO[2])
                {
                    CANA_tzLPCDI_IORegs[CANA_mLPC30_IO].all = msgDataIO[1];
                    CANA_tzLPCAIFlt_IORegs[CANA_mLPC30_IO].all = msgDataIO[7];

                    CANA_tzIOflags.btLPC30CommnStart = true;
                    CANA_tzIOtimers.LPC30ComFailCnt = 0;

                }
            }
            break;

        case LPC_31:

            if (CANA_tzIOtimers.RxCntLPC31 != msgDataIO[0])
            {
                CANA_tzIOtimers.RxCntLPC31 = msgDataIO[0];

                if (msgDataIO[1] == msgDataIO[2])
                {
                    CANA_tzLPCDI_IORegs[CANA_mLPC31_IO].all = msgDataIO[1];
                    CANA_tzLPCAIFlt_IORegs[CANA_mLPC31_IO].all = msgDataIO[7];

                    CANA_tzIOflags.btLPC31CommnStart = true;
                    CANA_tzIOtimers.LPC31ComFailCnt = 0;
                }
            }
            break;
        }
        break;

    case IO_AIBLK1_MSGID:

        switch (uiNodeType)
        {

        case LPC_30:

            CANA_tzAIData_IORegs[CANA_mLPC30_IO].AI0_Data = ((msgDataIO[0] << 8)
                    | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC30_IO].AI1_Data = ((msgDataIO[2] << 8)
                    | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC30_IO].AI2_Data = ((msgDataIO[4] << 8)
                    | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC30_IO].AI3_Data = ((msgDataIO[6] << 8)
                    | (msgDataIO[7])) * 0.001;
            break;

        case LPC_31:

            CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI0_Data = ((msgDataIO[0] << 8)
                    | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI1_Data = ((msgDataIO[2] << 8)
                    | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI2_Data = ((msgDataIO[4] << 8)
                    | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI3_Data = ((msgDataIO[6] << 8)
                    | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.OXS_101 =
                    CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI3_Data;

            break;
        default:
            break;
        }
        break;

    case IO_AIBLK2_MSGID:

        switch (uiNodeType)
        {

        case LPC_30:

            CANA_tzAIData_IORegs[CANA_mLPC30_IO].AI4_Data = ((msgDataIO[0] << 8)
                    | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC30_IO].AI5_Data = ((msgDataIO[2] << 8)
                    | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC30_IO].AI6_Data = ((msgDataIO[4] << 8)
                    | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC30_IO].AI7_Data = ((msgDataIO[6] << 8)
                    | (msgDataIO[7])) * 0.001;
            break;

        case LPC_31:

            CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI4_Data = ((msgDataIO[0] << 8)
                    | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI5_Data = ((msgDataIO[2] << 8)
                    | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI6_Data = ((msgDataIO[4] << 8)
                    | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI7_Data = ((msgDataIO[6] << 8)
                    | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.HYS_101 =
                    CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI4_Data;
            CANA_tzAISensorData.HYS_501 =
                    CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI5_Data;
            CANA_tzAISensorData.HYS_401 =
                    CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI6_Data;
            CANA_tzAISensorData.HYS_102 =
                    CANA_tzAIData_IORegs[CANA_mLPC31_IO].AI7_Data;

            break;
        default:
            break;
        }
        break;

    case IO_THERMAL_MSGID:

        break;

    case IO_DI1_FREQ_MSGID:

        switch (uiNodeType)
        {
        case LPC_31:

            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI0_FreqData =
                    ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI1_FreqData =
                    ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI2_FreqData =
                    ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI3_FreqData =
                    ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.001;

            CANA_tzDISensorData.PURGE101 =
                    CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI0_FreqData;
            CANA_tzDISensorData.PURGE102 =
                    CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI1_FreqData;
            CANA_tzDISensorData.PURGE501 =
                    CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI2_FreqData;
            CANA_tzDISensorData.PURGE502 =
                    CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI3_FreqData;

            break;
        default:
            break;
        }
        break;

    case IO_DI2_FREQ_MSGID:

        switch (uiNodeType)
        {
        case LPC_31:

            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI4_FreqData =
                    ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI5_FreqData =
                    ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI6_FreqData =
                    ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI7_FreqData =
                    ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.001;

            CANA_tzDISensorData.PURGE401 =
                    CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI4_FreqData;
            CANA_tzDISensorData.PURGE402 =
                    CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI5_FreqData;

            break;
        default:
            break;
        }
        break;

    }

    CANA_tzIOtimers.LPC30ComFailCnt++;
    if (CANA_tzIOtimers.LPC30ComFailCnt >= 90000)
    {
        CANA_tzIOtimers.LPC30ComFailCnt = 90001;
        CANA_tzIOflags.LPC30Comfail = 0;
    }

    CANA_tzIOtimers.LPC31ComFailCnt++;
    if (CANA_tzIOtimers.LPC31ComFailCnt >= 90000)
    {
        CANA_tzIOtimers.LPC31ComFailCnt = 90001;
        CANA_tzIOflags.LPC31Comfail = 0;
    }
}

/*=============================================================================
 @Function name : void can_fnmsgProcess(void)
 @brief  function to Process LHCIO messages over CAN

 @param void
 @return void
 ============================================================================ */
static void cana_fnmsgPrcsLHCIO(uint16_t uiMsgtype, uint16_t *msgDataIO,
                                uint16_t uiNodeType)

{
    switch (uiMsgtype)
    {
    case IO_DI_MSGID:

        switch (uiNodeType)
        {

        case LHC_10:

            if (CANA_tzIOtimers.RxCntLHC10 != msgDataIO[0])
            {
                CANA_tzIOtimers.RxCntLHC10 = msgDataIO[0];

                if (msgDataIO[1] == msgDataIO[2])
                {
                    CANA_tzLHCDI_IORegs[CANA_mLHC10_IO].all = msgDataIO[1];
                    CANA_tzIORegs.CJC[CANA_mLHC10_IO] = ((msgDataIO[3] << 8)
                            | (msgDataIO[4])) * 0.001;
                    CANA_tzLHCAIFlt_IORegs[CANA_mLHC10_IO].all = msgDataIO[5];

                }
            }
            break;

        case LHC_11:

            if (CANA_tzIOtimers.RxCntLHC11 != msgDataIO[0])
            {
                CANA_tzIOtimers.RxCntLHC11 = msgDataIO[0];

                if (msgDataIO[1] == msgDataIO[2])
                {
                    CANA_tzLHCDI_IORegs[CANA_mLHC11_IO].all = msgDataIO[1];
                    CANA_tzLHCAIFlt_IORegs[CANA_mLHC11_IO].all = msgDataIO[7];
                }
            }
            break;
        }
        break;

    case IO_AIBLK1_MSGID:

        switch (uiNodeType)
        {

        case LHC_10:

            CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI0_Data = ((msgDataIO[0] << 8)
                    | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI1_Data = ((msgDataIO[2] << 8)
                    | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI2_Data = ((msgDataIO[4] << 8)
                    | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI3_Data = ((msgDataIO[6] << 8)
                    | (msgDataIO[7])) * 0.001;
            break;

        case LHC_11:

            CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI0_Data = ((msgDataIO[0] << 8)
                    | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI1_Data = ((msgDataIO[2] << 8)
                    | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI2_Data = ((msgDataIO[4] << 8)
                    | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI3_Data = ((msgDataIO[6] << 8)
                    | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.PRT_402 =
                    CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI3_Data;

            break;
        default:
            break;
        }
        break;

    case IO_AIBLK2_MSGID:

        switch (uiNodeType)
        {

        case LHC_10:

            CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI4_Data = ((msgDataIO[0] << 8)
                    | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI5_Data = ((msgDataIO[2] << 8)
                    | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI6_Data = ((msgDataIO[4] << 8)
                    | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI7_Data = ((msgDataIO[6] << 8)
                    | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.LVL_101 =
                    CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI4_Data;
            CANA_tzAISensorData.PRT_101 =
                    CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI5_Data;
            CANA_tzAISensorData.PRT_102 =
                    CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI6_Data;
            CANA_tzAISensorData.COS_101 =
                    CANA_tzAIData_IORegs[CANA_mLHC10_IO].AI7_Data;

            break;

        case LHC_11:

            CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI4_Data = ((msgDataIO[0] << 8)
                    | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI5_Data = ((msgDataIO[2] << 8)
                    | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI6_Data = ((msgDataIO[4] << 8)
                    | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI7_Data = ((msgDataIO[6] << 8)
                    | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.PRT_401 =
                    CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI5_Data;
            CANA_tzAISensorData.TE_401 =
                    CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI6_Data;
            CANA_tzAISensorData.DPT_401 =
                    CANA_tzAIData_IORegs[CANA_mLHC11_IO].AI7_Data;

            break;
        default:
            break;
        }
        break;

    case IO_THERMAL_MSGID:

        switch (uiNodeType)
        {
        case LHC_10:

            CANA_tzThermal_IORegs[CANA_mLHC10_IO].T0_Data = ((msgDataIO[0] << 8)
                    | (msgDataIO[1])) * 0.001;
            CANA_tzThermal_IORegs[CANA_mLHC10_IO].T1_Data = ((msgDataIO[2] << 8)
                    | (msgDataIO[3])) * 0.001;
            CANA_tzThermal_IORegs[CANA_mLHC10_IO].T2_Data = ((msgDataIO[4] << 8)
                    | (msgDataIO[5])) * 0.001;
            CANA_tzThermal_IORegs[CANA_mLHC10_IO].T3_Data = ((msgDataIO[6] << 8)
                    | (msgDataIO[7])) * 0.001;

            CANA_tzThermoCoupleData.KTC_401 =
                    CANA_tzThermal_IORegs[CANA_mLHC10_IO].T0_Data;
            CANA_tzThermoCoupleData.TTC_101 =
                    CANA_tzThermal_IORegs[CANA_mLHC10_IO].T1_Data;
            CANA_tzThermoCoupleData.TTC_301 =
                    CANA_tzThermal_IORegs[CANA_mLHC10_IO].T2_Data;
            CANA_tzThermoCoupleData.TTC_102 =
                    CANA_tzThermal_IORegs[CANA_mLHC10_IO].T3_Data;

            break;
        default:
            break;
        }
        break;

    case IO_DI1_FREQ_MSGID:
        break;

    case IO_DI2_FREQ_MSGID:
        break;

    default:
        break;

    }
}

/*=============================================================================
 @Function name : void can_fnmsgProcess(void)
 @brief  function to Process Master Safety messages over CAN

 @param void
 @return void
 ============================================================================ */
static void cana_fnmsgPrcsMS(uint16_t *msgDataMS)
{
    if (CANA_tzMSRegs.RxCntMS != msgDataMS[0])
    {
        CANA_tzMSRegs.RxCntMS = msgDataMS[0];
        CANA_tzMSRegs.StartCmd = msgDataMS[1];
        CANA_tzMSRegs.PresentStMS = msgDataMS[2];
        CANA_tzMSRegs.btMSComStart = true;
        CANA_tzMSRegs.MSComFailCnt = 0;

    }

    CANA_tzMSRegs.MSComFailCnt++;
    if (CANA_tzMSRegs.MSComFailCnt >= 90000)
    {
        CANA_tzMSRegs.btMSComStart = false;
        CANA_tzMSRegs.MSComFailCnt = 90000;
    }

    switch (CANA_tzMSRegs.PresentStMS)
    {
    case 0:
        STAT_tzStateMacMS.Present_st = MS_STANDBY;
        break;

    case 1:
        STAT_tzStateMacMS.Present_st = MS_PURGE;
        break;
    case 2:
        STAT_tzStateMacMS.Present_st = MS_IOPOWER;
        break;
    case 3:
        STAT_tzStateMacMS.Present_st = MS_ARMED_POWER;
        break;
    case 4:
        STAT_tzStateMacMS.Present_st = MS_FAULT;
        break;
    case 5:
        STAT_tzStateMacMS.Present_st = MS_SHUTDOWN;
        break;
    default:
        break;
    }

}

void CANA_fnTx()
{
    // Common Messages Irrespective of States

    canaTX_fnMS();

    switch (STAT_tzStateMac.Present_st)
    {

    case STAND_BY:

        break;

    case READY:

        control_waterloop();

        break;

    case STACK_CHECK:

        control_waterloop();

        break;

    case STACK_POWER:

        control_waterloop();

        break;

    default:
        break;
    }

    if (CANA_tzMSRegs.TxCntMS >= 255)
    {
        CANA_tzMSRegs.TxCntMS = 0;
    }

}

void CANA_fnCmdsForDigOPs(uint16_t ui16unitID, uint16_t ui16cab_ID,
                          uint16_t ui16nodeID, CANA_tzDIG_OP *ptrDigOP)
{
    CAN_setupMessageObject(
            CANA_BASE,
            CAN_mMAILBOX_8,
            (ui16unitID << 8) | (ui16cab_ID << 4) | (ui16nodeID)
                    | CANA_mTX_IOMSGID1,
            CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
            CAN_MSG_OBJ_NO_FLAGS,
            CAN_mLEN8);

    CANA_tzIOtimers.TxCntIOCom++;

    if (ui16cab_ID == 3)
    {
        ui16CabID = 0; // filling LPC Cabinet array
    }
    else if (ui16cab_ID == 1)
    {
        ui16CabID = 1; // filling LHC Cabinet array
    }

    ui16txMsgDataIO[0] = CANA_tzIOtimers.TxCntIOCom;
    ui16txMsgDataIO[1] = ptrDigOP->all;
    ui16txMsgDataIO[2] = ptrDigOP->all;

    if (CANA_tzIOtimers.TxCntIOCom == 255)
    {
        CANA_tzIOtimers.TxCntIOCom = 0;
    }

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_8, CAN_mLEN8, ui16txMsgDataIO);

}

void CANA_fnCmdsForAnaOPVs(uint16_t ui16unitID, uint16_t ui16cab_ID,
                           uint16_t ui16nodeID, can_tzAnaOPParams *ptrAOV)
{

    CAN_setupMessageObject(
            CANA_BASE,
            CAN_mMAILBOX_8,
            (ui16unitID << 8) | (ui16cab_ID << 4) | (ui16nodeID)
                    | CANA_mTX_IOMSGID3,
            CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
            CAN_MSG_OBJ_NO_FLAGS,
            CAN_mLEN8);

    if (ui16cab_ID == 3)
    {
        ui16CabID = 0; // filling LPC Cabinet array
    }
    else if (ui16cab_ID == 1)
    {
        ui16CabID = 1; // filling LHC Cabinet array
    }

    ui16txMsgDataIO[0] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV1) >> 8;
    ui16txMsgDataIO[1] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV1);
    ui16txMsgDataIO[2] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV2) >> 8;
    ui16txMsgDataIO[3] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV2);
    ui16txMsgDataIO[4] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV3) >> 8;
    ui16txMsgDataIO[5] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV3);
    ui16txMsgDataIO[6] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV4) >> 8;
    ui16txMsgDataIO[7] = (ptrAOV->CANA_tzAOV[ui16CabID][ui16nodeID].AOV4);

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_8, CAN_mLEN8, ui16txMsgDataIO);
}

void CANA_fnCmdsForAnaOPIs(uint16_t ui16unitID, uint16_t ui16cab_ID,
                           uint16_t ui16nodeID, can_tzAnaOPParams *ptrAOI)
{

    CAN_setupMessageObject(
            CANA_BASE,
            CAN_mMAILBOX_8,
            (ui16unitID << 8) | (ui16cab_ID << 4) | (ui16nodeID)
                    | CANA_mTX_IOMSGID2,
            CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
            CAN_MSG_OBJ_NO_FLAGS,
            CAN_mLEN8);

    if (ui16cab_ID == 3)
    {
        ui16CabID = 0; // filling LPC Cabinet array
    }
    else if (ui16cab_ID == 1)
    {
        ui16CabID = 1; // filling LHC Cabinet array
    }

    ui16txMsgDataIO[0] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI1) >> 8;
    ui16txMsgDataIO[1] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI1);
    ui16txMsgDataIO[2] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI2) >> 8;
    ui16txMsgDataIO[3] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI2);
    ui16txMsgDataIO[4] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI3) >> 8;
    ui16txMsgDataIO[5] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI3);
    ui16txMsgDataIO[6] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI4) >> 8;
    ui16txMsgDataIO[7] = (ptrAOI->CANA_tzAOI[ui16CabID][ui16nodeID].AOI4);

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_8, CAN_mLEN8, ui16txMsgDataIO);
}

static void canaTX_fnMS()
{
// Master Process to Master Safety

    CANA_tzMSRegs.TxCntMS++;

    CAN_setupMessageObject(
    CANA_BASE,
                           CAN_mMAILBOX_11,
                           CANA_mTX_MSMSGID1,
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    uiCANtxMsgDataMS[0] = CANA_tzMSRegs.TxCntMS;

    uiCANtxMsgDataMS[1] = STAT_tzStateMac.Present_st;
    uiCANtxMsgDataMS[2] = 0;

    uiCANtxMsgDataMS[3] = 0;
    uiCANtxMsgDataMS[4] = 0;

    uiCANtxMsgDataMS[5] = 0;

    uiCANtxMsgDataMS[6] = 0;
    uiCANtxMsgDataMS[7] = 0;

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_11, CAN_mLEN8, uiCANtxMsgDataMS);
}
/*==============================================================================
 End of File
 ==============================================================================*/
