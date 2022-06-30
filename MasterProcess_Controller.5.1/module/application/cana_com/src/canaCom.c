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

CANA_tzAO_IOREGS CANA_tzAOData_IORegs[CANA_mTOTAL_IONODE],
        CANA_tzSetAOData_IORegs[CANA_mTOTAL_IONODE];
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
union CANA_tzLPCDIFLT_IOREGS CANA_tzLPCDIFaultRegs[CANA_mTOTAL_LPCNODES];
union CANA_tzLHCDIFLT_IOREGS CANA_tzLHCDIFaultRegs[CANA_mTOTAL_LHCNODES];

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

bool can_fnEnquedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t msgID,
                     uint16_t DLC);
bool can_fndequedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t *msgID,
                     uint16_t *DLC);
static void cana_fnmsgPrcsMS(uint16_t *msgDataMS);

/*==============================================================================
 Local Variables
 ==============================================================================*/

uint16_t uirxMsgLPCIO[8] = { 0 };
uint16_t uirxMsgLHCIO[8] = { 0 };
uint16_t uirxMsgMS[8] = { 0 };

uint16_t uirxPrcsMsgLPCIO[8] = { 0 };
uint16_t uirxPrcsMsgLHCIO[8] = { 0 };
uint16_t uirxPrcsMsgMS[8] = { 0 };

uint32_t u32msgID = 0;
uint16_t uiDataLength = 0;
uint16_t uiMsgtype = 0, uiNodeType = 0;
uint16_t uiCANtxMsgDataMS[8] = { 0 };

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

    if (CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_7))
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_7, uirxMsgMS);

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

    ui32temp = (u32msgID & 0x00F0000F);
    CANA_tzIORegs.uiMsgtypeLPCIO = (uint16_t) (ui32temp >> 20);
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

        case LPC_130:

            if (CANA_tzIOtimers.RxCntLPC130 != msgDataIO[0])
            {
                CANA_tzIOtimers.RxCntLPC130 = msgDataIO[0];

                if (msgDataIO[1] == msgDataIO[2])
                {
                    CANA_tzLPCDI_IORegs[CANA_mLPC130_IO].all = msgDataIO[1];
                    CANA_tzLPCAIFlt_IORegs[CANA_mLPC130_IO].all = msgDataIO[7];

                    CANA_tzIOflags.btLPC130CommnStart = true;
                    CANA_tzIOtimers.LPC130ComFailCnt = 0;

                }
            }
            break;

        case LPC_131:

            if (CANA_tzIOtimers.RxCntLPC131 != msgDataIO[0])
            {
                CANA_tzIOtimers.RxCntLPC131 = msgDataIO[0];

                if (msgDataIO[1] == msgDataIO[2])
                {
                    CANA_tzLPCDI_IORegs[CANA_mLPC131_IO].all = msgDataIO[1];
                    CANA_tzLPCAIFlt_IORegs[CANA_mLPC131_IO].all = msgDataIO[7];

                    CANA_tzIOflags.btLPC131CommnStart = true;
                    CANA_tzIOtimers.LPC131ComFailCnt = 0;
                }
            }
            break;
        }
        break;

    case IO_AIBLK1_MSGID:

        switch (uiNodeType)
        {

        case LPC_130:

            CANA_tzAIData_IORegs[CANA_mLPC130_IO].AI0_Data =
                    ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC130_IO].AI1_Data =
                    ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC130_IO].AI2_Data =
                    ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC130_IO].AI3_Data =
                    ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.001;
            break;

        case LPC_131:

            CANA_tzAIData_IORegs[CANA_mLPC131_IO].AI0_Data =
                    ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC131_IO].AI1_Data =
                    ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC131_IO].AI2_Data =
                    ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC131_IO].AI3_Data =
                    ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.OXS_101 =
                    CANA_tzAIData_IORegs[CANA_mLPC131_IO].AI3_Data;

            break;
        default:
            break;
        }
        break;

    case IO_AIBLK2_MSGID:

        switch (uiNodeType)
        {

        case LPC_130:

            CANA_tzAIData_IORegs[CANA_mLPC130_IO].AI4_Data =
                    ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC130_IO].AI5_Data =
                    ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC130_IO].AI6_Data =
                    ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC130_IO].AI7_Data =
                    ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.001;
            break;

        case LPC_131:

            CANA_tzAIData_IORegs[CANA_mLPC131_IO].AI4_Data =
                    ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC131_IO].AI5_Data =
                    ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC131_IO].AI6_Data =
                    ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLPC131_IO].AI7_Data =
                    ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.HYS_101 =
                    CANA_tzAIData_IORegs[CANA_mLPC131_IO].AI4_Data;
            CANA_tzAISensorData.HYS_501 =
                    CANA_tzAIData_IORegs[CANA_mLPC131_IO].AI5_Data;
            CANA_tzAISensorData.HYS_401 =
                    CANA_tzAIData_IORegs[CANA_mLPC131_IO].AI6_Data;
            CANA_tzAISensorData.HYS_102 =
                    CANA_tzAIData_IORegs[CANA_mLPC131_IO].AI7_Data;

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
        case LPC_131:

            CANA_tzAIDataFreq_IORegs[CANA_mLPC131_IO].DI0_FreqData =
                    ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC131_IO].DI1_FreqData =
                    ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC131_IO].DI2_FreqData =
                    ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC131_IO].DI3_FreqData =
                    ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.001;

            CANA_tzDISensorData.PURGE101 =
                    CANA_tzAIDataFreq_IORegs[CANA_mLPC131_IO].DI0_FreqData;
            CANA_tzDISensorData.PURGE102 =
                    CANA_tzAIDataFreq_IORegs[CANA_mLPC131_IO].DI1_FreqData;
            CANA_tzDISensorData.PURGE501 =
                    CANA_tzAIDataFreq_IORegs[CANA_mLPC131_IO].DI2_FreqData;
            CANA_tzDISensorData.PURGE502 =
                    CANA_tzAIDataFreq_IORegs[CANA_mLPC131_IO].DI3_FreqData;

            break;
        default:
            break;
        }
        break;

    case IO_DI2_FREQ_MSGID:

        switch (uiNodeType)
        {
        case LPC_131:

            CANA_tzAIDataFreq_IORegs[CANA_mLPC131_IO].DI4_FreqData =
                    ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC131_IO].DI5_FreqData =
                    ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC131_IO].DI6_FreqData =
                    ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC131_IO].DI7_FreqData =
                    ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.001;

            CANA_tzDISensorData.PURGE401 =
                    CANA_tzAIDataFreq_IORegs[CANA_mLPC131_IO].DI4_FreqData;
            CANA_tzDISensorData.PURGE402 =
                    CANA_tzAIDataFreq_IORegs[CANA_mLPC131_IO].DI5_FreqData;

            break;
        default:
            break;
        }
        break;

    }

    CANA_tzIOtimers.LPC130ComFailCnt++;
    if (CANA_tzIOtimers.LPC130ComFailCnt >= 90000)
    {
        CANA_tzIOtimers.LPC130ComFailCnt = 90001;
        CANA_tzIOflags.LPC130Comfail = 0;
    }

    CANA_tzIOtimers.LPC131ComFailCnt++;
    if (CANA_tzIOtimers.LPC131ComFailCnt >= 90000)
    {
        CANA_tzIOtimers.LPC131ComFailCnt = 90001;
        CANA_tzIOflags.LPC131Comfail = 0;
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

        case LHC_110:

            if (CANA_tzIOtimers.RxCntLHC110 != msgDataIO[0])
            {
                CANA_tzIOtimers.RxCntLHC110 = msgDataIO[0];

                if (msgDataIO[1] == msgDataIO[2])
                {
                    CANA_tzLHCDI_IORegs[CANA_mLHC110_IO].all = msgDataIO[1];
                    CANA_tzIORegs.CJC[CANA_mLHC110_IO] = ((msgDataIO[3] << 8)
                            | (msgDataIO[4])) * 0.001;
                    CANA_tzLHCAIFlt_IORegs[CANA_mLHC110_IO].all = msgDataIO[7];

                }
            }
            break;

        case LHC_111:

            if (CANA_tzIOtimers.RxCntLHC111 != msgDataIO[0])
            {
                CANA_tzIOtimers.RxCntLHC111 = msgDataIO[0];

                if (msgDataIO[1] == msgDataIO[2])
                {
                    CANA_tzLHCDI_IORegs[CANA_mLHC111_IO].all = msgDataIO[1];
                    CANA_tzLHCAIFlt_IORegs[CANA_mLHC111_IO].all = msgDataIO[7];
                }
            }
            break;
        }
        break;

    case IO_AIBLK1_MSGID:

        switch (uiNodeType)
        {

        case LHC_110:

            CANA_tzAIData_IORegs[CANA_mLHC110_IO].AI0_Data =
                    ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC110_IO].AI1_Data =
                    ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC110_IO].AI2_Data =
                    ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC110_IO].AI3_Data =
                    ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.001;
            break;

        case LHC_111:

            CANA_tzAIData_IORegs[CANA_mLHC111_IO].AI0_Data =
                    ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC111_IO].AI1_Data =
                    ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC111_IO].AI2_Data =
                    ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC111_IO].AI3_Data =
                    ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.PRT_402 =
                    CANA_tzAIData_IORegs[CANA_mLHC111_IO].AI3_Data;

            break;
        default:
            break;
        }
        break;

    case IO_AIBLK2_MSGID:

        switch (uiNodeType)
        {

        case LHC_110:

            CANA_tzAIData_IORegs[CANA_mLHC110_IO].AI4_Data =
                    ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC110_IO].AI5_Data =
                    ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC110_IO].AI6_Data =
                    ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC110_IO].AI7_Data =
                    ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.LVL_101 =
                    CANA_tzAIData_IORegs[CANA_mLHC110_IO].AI4_Data;
            CANA_tzAISensorData.PRT_101 =
                    CANA_tzAIData_IORegs[CANA_mLHC110_IO].AI5_Data;
            CANA_tzAISensorData.PRT_102 =
                    CANA_tzAIData_IORegs[CANA_mLHC110_IO].AI6_Data;
            CANA_tzAISensorData.COS_101 =
                    CANA_tzAIData_IORegs[CANA_mLHC110_IO].AI7_Data;

            break;

        case LHC_111:

            CANA_tzAIData_IORegs[CANA_mLHC111_IO].AI4_Data =
                    ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC111_IO].AI5_Data =
                    ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC111_IO].AI6_Data =
                    ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_IORegs[CANA_mLHC111_IO].AI7_Data =
                    ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.PRT_401 =
                    CANA_tzAIData_IORegs[CANA_mLHC111_IO].AI5_Data;
            CANA_tzAISensorData.TE_401 =
                    CANA_tzAIData_IORegs[CANA_mLHC111_IO].AI6_Data;
            CANA_tzAISensorData.DPT_401 =
                    CANA_tzAIData_IORegs[CANA_mLHC111_IO].AI7_Data;

            break;
        default:
            break;
        }
        break;

    case IO_THERMAL_MSGID:

        switch (uiNodeType)
        {
        case LHC_110:

            CANA_tzThermal_IORegs[CANA_mLHC110_IO].T0_Data =
                    ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.001;
            CANA_tzThermal_IORegs[CANA_mLHC110_IO].T1_Data =
                    ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.001;
            CANA_tzThermal_IORegs[CANA_mLHC110_IO].T2_Data =
                    ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.001;
            CANA_tzThermal_IORegs[CANA_mLHC110_IO].T3_Data =
                    ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.001;

            CANA_tzThermoCoupleData.KTC_401 =
                    CANA_tzThermal_IORegs[CANA_mLHC110_IO].T0_Data;
            CANA_tzThermoCoupleData.TTC_101 =
                    CANA_tzThermal_IORegs[CANA_mLHC110_IO].T1_Data;
            CANA_tzThermoCoupleData.TTC_301 =
                    CANA_tzThermal_IORegs[CANA_mLHC110_IO].T2_Data;
            CANA_tzThermoCoupleData.TTC_102 =
                    CANA_tzThermal_IORegs[CANA_mLHC110_IO].T3_Data;

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

    // Master Process to Master Safety

    CANA_tzMSRegs.TxCntMS++;

    CAN_setupMessageObject(
    CANA_BASE,
                           CAN_mMAILBOX_8,
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

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_8, CAN_mLEN8, uiCANtxMsgDataMS);

    switch (STAT_tzStateMac.Present_st)
    {

    case STAND_BY:

        break;

    case READY:
        break;

    case STACK_CHECK:
        break;

    case STACK_POWER:
        break;

    default:
        break;
    }

    if (CANA_tzMSRegs.TxCntMS >= 255)
    {
        CANA_tzMSRegs.TxCntMS = 0;
    }

}

/*==============================================================================
 End of File
 ==============================================================================*/
