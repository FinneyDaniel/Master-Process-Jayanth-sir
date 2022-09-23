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
#include "canb_defs.h"
#include "cana_PSUCom.h"
#include "cana_vsc.h"
#include "hal/driverlib/can.h"
#include "state_machine.h"
#include "isr.h"
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

CANA_tzAI_IOREGS CANA_tzAIData_LPCIORegs[CANA_mTOTAL_LPCNODES],CANA_tzAIData_LHCIORegs[CANA_mTOTAL_LHCNODES];
CANA_tzDIFREQ_IOREGS CANA_tzAIDataFreq_IORegs[CANA_mTOTAL_IONODE];

CANA_tzTHERMAL_IOREGS CANA_tzThermal_IORegs[CANA_mTOTAL_IONODE];
CANA_tzAISENSOR_DATA CANA_tzAISensorData;
CANA_tzDISENSOR_DATA CANA_tzDISensorData;

CANA_tzTHERMOCOUPLE_DATA CANA_tzThermoCoupleData;
CANA_tzDO_IOREGS CANA_tzSetDO_IORegs;
can_tzAnaOPParams CANA_tzAnaOPParams;
CANA_tzDIG_OP CANA_tzDO[2][2];
CANA_tzDOREGS CANA_tzDOParams;

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

union CANA_tzACTIVE_IONODE_REGS CANA_tzActNodeRegs_IO;
union CANA_tzACTIVE_VSNODE_REGS CANA_tzActNodeRegs_VS;
union CANA_tzACTIVE_VS1NODE_REGS CANA_tzActNodeRegs_VS1;

union CANA_tzDOMS_STATUS_REGS CANA_tzActMS_DOStRegs;

union CANA_tzMP_FAULTS_REGS CANA_tzActMS_FaultRegs;

/*==============================================================================
 Macros
 ==============================================================================*/

CIRC_BUF_DEF(uiRxbufferLPCIO, 100);
CIRC_BUF_DEF(uiRxbufferLHCIO, 100);
CIRC_BUF_DEF(uiRxbufferMS, 50);
CIRC_BUF_DEF(uiRxbufferVSC, 100);

/*==============================================================================
 Local Function Prototypes
 ==============================================================================*/

void CANA_fnInitMBox(void);
void CANA_fnRXevent(void);
void CANA_fnTask(void);
void CANA_fnTx(void);
void CAN_fnComFailChk(void);

static void cana_fnmsgPrcsLPCIO(uint16_t uimsgID, uint16_t *msgData,
                                uint16_t uiNodeType);

static void cana_fnmsgPrcsLHCIO(uint16_t uimsgID, uint16_t *msgData,
                                uint16_t uiNodeType);

static void cana_fnmsgPrcsMS(uint16_t uiMsgtype, uint16_t *msgDataMS);
static float32_t limitAnalogSensorData(float32 SensorType);


void CANA_fnIOHrtBt();
void safeshutDown();

bool can_fnEnquedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t msgID,
                     uint16_t DLC);
bool can_fndequedata(can_tzcirc_buff *ptr, uint16_t *data, uint32_t *msgID,
                     uint16_t *DLC);

void CANA_fnMSTxCmds(uint16_t ui16CabiD, uint16_t NodeID,
                     CANA_tzDIG_OP *ptrDigOP);

void CANA_fnCmdsForAnaOPVs(uint16_t ui16unitID, uint16_t ui16cab_ID,
                           uint16_t ui16nodeID, can_tzAnaOPParams *ptrAO_V);

void CANA_fnCmdsForAnaOPIs(uint16_t ui16unitID, uint16_t ui16cab_ID,
                           uint16_t ui16nodeID, can_tzAnaOPParams *ptrAO_I);

void cana_CommisionMode();

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
uint16_t uirxMsgVSC[8] = { 0 };
uint16_t uirxPrcsMsgVSC[8] = { 0 };

uint32_t u32msgID1 = 0, u32msgID2 = 0, u32msgID3 = 0, u32msgID4 = 0;
uint16_t uiDataLength1 = 0, uiDataLength2 = 0, uiDataLength3 = 0,
        uiDataLength4 = 0;
uint16_t uiMsgtype = 0, uiNodeType = 0;
uint16_t uiCANtxMsgDataMS[8] = { 0 };
uint16_t ui16CabID = 0, ui16prev_value = 0;
uint16_t uiCabIDAO = 0, uiNodeIDAO = 0;
uint16_t ui16Cnt = 0;
uint16_t ui16StateTnstCnt = 0, ui16StateRstCnt = 0;
uint16_t testCabID = 0, testCabID1 = 0, testNodeID = 0, testCntVFD = 0, ui16ComsnCnt = 0;
float32 testEBV = 0;
uint16_t var = 0, ui16IOcnt = 0;
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

    //VSC receive event

    if (CAN_IsMessageReceived(CANA_BASE, CAN_mMAILBOX_12))
    {
        CAN_readMessage(CANA_BASE, CAN_mMAILBOX_12, uirxMsgVSC);

        can_fnEnquedata(&uiRxbufferVSC, uirxMsgVSC, CanaRegs.CAN_IF2ARB.bit.ID,
                        CanaRegs.CAN_IF2MCTL.bit.DLC);
        ui32CANAVSFailCnt1 = 0;

    }

    //can transmit event

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

    while (can_fndequedata(&uiRxbufferLPCIO, uirxPrcsMsgLPCIO, &u32msgID1,
                           &uiDataLength1))
    {

        //extracting msgID for individual messages of LPC

        ui32temp = (u32msgID1 & 0x00F00F0F);
        CANA_tzIORegs.uiMsgtypeLPCIO =
                (uint16_t) ((ui32temp & 0x00F00000) >> 20);
        CANA_tzIORegs.uiUnitID = (uint16_t) ((ui32temp & 0x00000F00) >> 8);

        CANA_tzIORegs.uiNodeLPCIO = (uint16_t) (ui32temp & 0x0F);

        //processing received messages of LPC

        cana_fnmsgPrcsLPCIO(CANA_tzIORegs.uiMsgtypeLPCIO, uirxPrcsMsgLPCIO,
                            CANA_tzIORegs.uiNodeLPCIO);
    }

    while (can_fndequedata(&uiRxbufferLHCIO, uirxPrcsMsgLHCIO, &u32msgID2,
                           &uiDataLength2))

    //extracting msgID for individual messages of LHC

    {
        ui32temp = (u32msgID2 & 0x00F0000F);
        CANA_tzIORegs.uiMsgtypeLHCIO = (uint16_t) (ui32temp >> 20);
        CANA_tzIORegs.uiNodeLHCIO = (uint16_t) (ui32temp & 0x0F);

        cana_fnmsgPrcsLHCIO(CANA_tzIORegs.uiMsgtypeLHCIO, uirxPrcsMsgLHCIO,
                            CANA_tzIORegs.uiNodeLHCIO);
    }

    while (can_fndequedata(&uiRxbufferMS, uirxPrcsMsgMS, &u32msgID3,
                           &uiDataLength3))
    {

        ui32temp = (u32msgID3 & 0x00F00000);
        CANA_tzMSRegs.uiMsgtype = (uint16_t) (ui32temp >> 20);

        cana_fnmsgPrcsMS(CANA_tzMSRegs.uiMsgtype, uirxPrcsMsgMS);
    }

    // VSC Processing

    while (can_fndequedata(&uiRxbufferVSC, uirxPrcsMsgVSC, &u32msgID4,
                           &uiDataLength4))
    {

        canA_VSCbuff.uiNodeID = u32msgID4 & 0xF;
        ui32temp = u32msgID4 & 0x00F00000;
        canA_VSCbuff.uiMsgType = (uint16_t) (ui32temp >> 20);

        cana_fnmsgPrcsVSC(canA_VSCbuff.uiNodeID, canA_VSCbuff.uiMsgType,
                          uirxPrcsMsgVSC);
    }
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
                CANA_tzLPCAIFlt_IORegs[CANA_mLPC30_IO].all = msgDataIO[5];

                CANA_tzIOflags.btLPC30CommnStart = true;
                CANA_tzIOtimers.LPC30ComFailCnt = 0;
                CANA_tzActNodeRegs_IO.bit.bt_LPC30 = 1;
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
                CANA_tzLPCAIFlt_IORegs[CANA_mLPC31_IO].all = msgDataIO[5];

                CANA_tzIOflags.btLPC31CommnStart = true;
                CANA_tzIOtimers.LPC31ComFailCnt = 0;
                CANA_tzActNodeRegs_IO.bit.bt_LPC31 = 1;

            }
        }
            break;
        }
        break;

    case IO_AIBLK1_MSGID:

        switch (uiNodeType)
        {

        case LPC_30:

            CANA_tzAIData_LPCIORegs[CANA_mLPC30_IO].AI0_Data = ((msgDataIO[0] << 8)
                    | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC30_IO].AI1_Data = ((msgDataIO[2] << 8)
                    | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC30_IO].AI2_Data = ((msgDataIO[4] << 8)
                    | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC30_IO].AI3_Data = ((msgDataIO[6] << 8)
                    | (msgDataIO[7])) * 0.001;
            break;

        case LPC_31:

            CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI0_Data = ((msgDataIO[0] << 8)
                    | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI1_Data = ((msgDataIO[2] << 8)
                    | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI2_Data = ((msgDataIO[4] << 8)
                    | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI3_Data = ((msgDataIO[6] << 8)
                    | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.HYS_401 =
                    CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI3_Data;

            break;
        default:
            break;
        }
        break;

    case IO_AIBLK2_MSGID:

        switch (uiNodeType)
        {

        case LPC_30:

            CANA_tzAIData_LPCIORegs[CANA_mLPC30_IO].AI4_Data = ((msgDataIO[0] << 8)
                    | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC30_IO].AI5_Data = ((msgDataIO[2] << 8)
                    | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC30_IO].AI6_Data = ((msgDataIO[4] << 8)
                    | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC30_IO].AI7_Data = ((msgDataIO[6] << 8)
                    | (msgDataIO[7])) * 0.001;
            break;

        case LPC_31:

            CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI4_Data = ((msgDataIO[0] << 8)
                    | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI5_Data = ((msgDataIO[2] << 8)
                    | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI6_Data = ((msgDataIO[4] << 8)
                    | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI7_Data = ((msgDataIO[6] << 8)
                    | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.HYS_101 =
                    CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI4_Data;
            CANA_tzAISensorData.HYS_102 =
                    CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI5_Data;
            CANA_tzAISensorData.OXS_101 =
                    CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI6_Data;
            CANA_tzAISensorData.HYS_501 =
                    CANA_tzAIData_LPCIORegs[CANA_mLPC31_IO].AI7_Data;

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
                    ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.01;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI1_FreqData =
                    ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.01;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI2_FreqData =
                    ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.01;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI3_FreqData =
                    ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.01;

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
                    ((msgDataIO[0] << 8) | (msgDataIO[1])) * 0.01;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI5_FreqData =
                    ((msgDataIO[2] << 8) | (msgDataIO[3])) * 0.01;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI6_FreqData =
                    ((msgDataIO[4] << 8) | (msgDataIO[5])) * 0.01;
            CANA_tzAIDataFreq_IORegs[CANA_mLPC31_IO].DI7_FreqData =
                    ((msgDataIO[6] << 8) | (msgDataIO[7])) * 0.01;

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


    CANA_tzAISensorData.HYS_101 = limitAnalogSensorData(
            CANA_tzAISensorData.HYS_101);
    CANA_tzAISensorData.HYS_102 = limitAnalogSensorData(
            CANA_tzAISensorData.HYS_102);
    CANA_tzAISensorData.HYS_501 = limitAnalogSensorData(
            CANA_tzAISensorData.HYS_501);
    CANA_tzAISensorData.HYS_401 = limitAnalogSensorData(
            CANA_tzAISensorData.HYS_401);
    CANA_tzAISensorData.OXS_101 = limitAnalogSensorData(
            CANA_tzAISensorData.OXS_101);
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
                        | (msgDataIO[4])) * 0.01;
                CANA_tzLHCAIFlt_IORegs[CANA_mLHC10_IO].all = msgDataIO[5];

                CANA_tzIOflags.btLHC10CommnStart = true;
                CANA_tzIOtimers.LHC10ComFailCnt = 0;
                CANA_tzActNodeRegs_IO.bit.bt_LHC10 = 1;

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
                CANA_tzLHCAIFlt_IORegs[CANA_mLHC11_IO].all = msgDataIO[5];
                CANA_tzIOflags.btLHC11CommnStart = true;
                CANA_tzIOtimers.LHC11ComFailCnt = 0;
                CANA_tzActNodeRegs_IO.bit.bt_LHC11 = 1;
            }
        }
            break;
        }
        break;

    case IO_AIBLK1_MSGID:

        switch (uiNodeType)
        {

        case LHC_10:

            CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI0_Data = ((msgDataIO[0] << 8)
                    | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI1_Data = ((msgDataIO[2] << 8)
                    | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI2_Data = ((msgDataIO[4] << 8)
                    | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI3_Data = ((msgDataIO[6] << 8)
                    | (msgDataIO[7])) * 0.001;
            break;

        case LHC_11:

            CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI0_Data = ((msgDataIO[0] << 8)
                    | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI1_Data = ((msgDataIO[2] << 8)
                    | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI2_Data = ((msgDataIO[4] << 8)
                    | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI3_Data = ((msgDataIO[6] << 8)
                    | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.PRT_402 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI3_Data;

            break;
        default:
            break;
        }
        break;

    case IO_AIBLK2_MSGID:

        switch (uiNodeType)
        {

        case LHC_10:

            CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI4_Data = ((msgDataIO[0] << 8)
                    | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI5_Data = ((msgDataIO[2] << 8)
                    | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI6_Data = ((msgDataIO[4] << 8)
                    | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI7_Data = ((msgDataIO[6] << 8)
                    | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.LVL_101 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI4_Data;
            CANA_tzAISensorData.PRT_101 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI5_Data;
            CANA_tzAISensorData.PRT_102 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI6_Data;
            CANA_tzAISensorData.COS_101 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC10_IO].AI7_Data;

            break;

        case LHC_11:

            CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI4_Data = ((msgDataIO[0] << 8)
                    | (msgDataIO[1])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI5_Data = ((msgDataIO[2] << 8)
                    | (msgDataIO[3])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI6_Data = ((msgDataIO[4] << 8)
                    | (msgDataIO[5])) * 0.001;
            CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI7_Data = ((msgDataIO[6] << 8)
                    | (msgDataIO[7])) * 0.001;

            CANA_tzAISensorData.PRT_401 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI5_Data;
            CANA_tzAISensorData.TE_401 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI6_Data;
            CANA_tzAISensorData.DPT_401 =
                    CANA_tzAIData_LHCIORegs[CANA_mLHC11_IO].AI7_Data;

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

            CANA_tzThermoCoupleData.TTC_101 =
                    CANA_tzThermal_IORegs[CANA_mLHC10_IO].T0_Data;
            CANA_tzThermoCoupleData.TTC_102 =
                    CANA_tzThermal_IORegs[CANA_mLHC10_IO].T1_Data;
            CANA_tzThermoCoupleData.TTC_301 =
                    CANA_tzThermal_IORegs[CANA_mLHC10_IO].T2_Data;
            CANA_tzThermoCoupleData.KTC_401 =
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

    CANA_tzAISensorData.LVL_101 = limitAnalogSensorData(
            CANA_tzAISensorData.LVL_101);
    CANA_tzAISensorData.PRT_101 = limitAnalogSensorData(
            CANA_tzAISensorData.PRT_101);
    CANA_tzAISensorData.PRT_102 = limitAnalogSensorData(
            CANA_tzAISensorData.PRT_102);
    CANA_tzAISensorData.PRT_401 = limitAnalogSensorData(
            CANA_tzAISensorData.PRT_401);
    CANA_tzAISensorData.PRT_402 = limitAnalogSensorData(
            CANA_tzAISensorData.PRT_402);
    CANA_tzAISensorData.TE_401 = limitAnalogSensorData(
            CANA_tzAISensorData.TE_401);
    CANA_tzAISensorData.COS_101 = limitAnalogSensorData(
            CANA_tzAISensorData.COS_101);
    CANA_tzAISensorData.OXS_101 = limitAnalogSensorData(
            CANA_tzAISensorData.OXS_101);
    CANA_tzAISensorData.DPT_401 = limitAnalogSensorData(
            CANA_tzAISensorData.DPT_401);

}

/*=============================================================================
 @Function name : void can_fnmsgProcess(void)
 @brief  function to Process Master Safety messages over CAN

 @param void
 @return void
 ============================================================================ */
static void cana_fnmsgPrcsMS(uint16_t uiMsgtype, uint16_t *msgDataMS)
{

    switch (uiMsgtype)
    {

    case 0:

        if (CANA_tzMSRegs.RxCntMS != msgDataMS[0])
        {
            CANA_tzMSRegs.RxCntMS = msgDataMS[0];
            CANA_tzMSRegs.StartCmd = msgDataMS[1];
            CANA_tzMSRegs.PresentStMS = msgDataMS[2];
            CANA_tzMSRegs.AOCmd = msgDataMS[3];
            CANA_tzActMS_FaultRegs.all = msgDataMS[4];
            CANA_tzMSRegs.AOVFan101 = msgDataMS[5];
            CANA_tzMSRegs.AOVFan501 = msgDataMS[6];
            CANA_tzMSRegs.AOVFan401 = msgDataMS[7];
            CANA_tzMSRegs.btMSComStart = true;
            CANA_tzMSRegs.MSComFailCnt = 0;

        }

        if ((CANA_tzMSRegs.AOCmd & 0x01) == 0x01)
        {
            CANA_tzMSRegs.TurnONPurge101 = 1;
        }
        else
        {
            CANA_tzMSRegs.TurnONPurge101 = 0;
        }

        if ((CANA_tzMSRegs.AOCmd & 0x02) == 0x02)
        {
            CANA_tzMSRegs.TurnONPurge501 = 1;
        }
        else
        {
            CANA_tzMSRegs.TurnONPurge501 = 0;
        }

        if ((CANA_tzMSRegs.AOCmd & 0x0004) == 0x0004)
        {
            CANA_tzMSRegs.TurnONPurge401 = 1;
        }
        else
        {
            CANA_tzMSRegs.TurnONPurge401 = 0;
        }
        break;

    case 1:

        CANA_tzActMS_DOStRegs.all = msgDataMS[0];

        break;

    default:
        break;

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

void CANA_fnComFailChk()
{

    uint16_t ui16temp;

    CANA_tzMSRegs.MSComFailCnt++;
    if (CANA_tzMSRegs.MSComFailCnt >= 3000)
    {
        CANA_tzMSRegs.btMSComStart = false;
        CANA_tzMSRegs.MSComFailCnt = 3001;
    }

    CANA_tzIOtimers.LHC10ComFailCnt++;
    if (CANA_tzIOtimers.LHC10ComFailCnt >= 3000)
    {
        CANA_tzIOtimers.LHC10ComFailCnt = 3001;
        CANA_tzIOflags.btLHC10CommnStart = false;
        CANA_tzActNodeRegs_IO.bit.bt_LHC10 = 0;

    }

    CANA_tzIOtimers.LHC11ComFailCnt++;
    if (CANA_tzIOtimers.LHC11ComFailCnt >= 3000)
    {
        CANA_tzIOtimers.LHC11ComFailCnt = 3001;
        CANA_tzIOflags.btLHC11CommnStart = false;
        CANA_tzActNodeRegs_IO.bit.bt_LHC11 = 0;

    }

    CANA_tzIOtimers.LPC30ComFailCnt++;
    if (CANA_tzIOtimers.LPC30ComFailCnt >= 3000)
    {
        CANA_tzIOtimers.LPC30ComFailCnt = 3001;
        CANA_tzActNodeRegs_IO.bit.bt_LPC30 = 0;
    }

    CANA_tzIOtimers.LPC31ComFailCnt++;
    if (CANA_tzIOtimers.LPC31ComFailCnt >= 3000)
    {
        CANA_tzIOtimers.LPC31ComFailCnt = 3001;
        CANA_tzActNodeRegs_IO.bit.bt_LPC31 = 0;
    }

    if (ui16CANAVSFailTrig1 == 1)
    {
        canA_VSCbuff.uiNodeID = 0;
    }

    if (canA_VSCbuff.uiNodeID != 1)
    {
        if (canA_tzVSC_info[1].CANfailCnt++ >= 2000)
        {
            canA_tzVSC_info[1].CANfailCnt = 2000;
            CANA_tzActNodeRegs_VS.bit.bt_VS1 = 0;

            for (ui16temp = 1; ui16temp <= 16; ui16temp++)
            {
                canA_tzVSC_info[1].f32Cellvolt[ui16temp] = 0;
            }
        }
    }
    else
    {
        canA_tzVSC_info[1].CANfailCnt = 0;
        CANA_tzActNodeRegs_VS.bit.bt_VS1 = 1;

    }

    if (canA_VSCbuff.uiNodeID != 2)
    {
        if (canA_tzVSC_info[2].CANfailCnt++ >= 2000)
        {
            canA_tzVSC_info[2].CANfailCnt = 2000;
            CANA_tzActNodeRegs_VS.bit.bt_VS2 = 0;
            for (ui16temp = 1; ui16temp <= 16; ui16temp++)
            {
                canA_tzVSC_info[2].f32Cellvolt[ui16temp] = 0;

            }
        }
    }
    else
    {
        canA_tzVSC_info[2].CANfailCnt = 0;
        CANA_tzActNodeRegs_VS.bit.bt_VS2 = 1;

    }

    if (canA_VSCbuff.uiNodeID != 3)
    {
        if (canA_tzVSC_info[3].CANfailCnt++ >= 2000)
        {
            canA_tzVSC_info[3].CANfailCnt = 2000;
            CANA_tzActNodeRegs_VS.bit.bt_VS3 = 0;
            for (ui16temp = 1; ui16temp <= 16; ui16temp++)
            {
                canA_tzVSC_info[3].f32Cellvolt[ui16temp] = 0;

            }
        }
    }
    else
    {
        canA_tzVSC_info[3].CANfailCnt = 0;
        CANA_tzActNodeRegs_VS.bit.bt_VS3 = 1;

    }

    if (canA_VSCbuff.uiNodeID != 4)
    {
        if (canA_tzVSC_info[4].CANfailCnt++ >= 2000)
        {
            canA_tzVSC_info[4].CANfailCnt = 2000;
            CANA_tzActNodeRegs_VS.bit.bt_VS4 = 0;
            for (ui16temp = 1; ui16temp <= 16; ui16temp++)
            {
                canA_tzVSC_info[4].f32Cellvolt[ui16temp] = 0;

            }
        }
    }
    else
    {
        canA_tzVSC_info[4].CANfailCnt = 0;
        CANA_tzActNodeRegs_VS.bit.bt_VS1 = 1;

    }

    if (canA_VSCbuff.uiNodeID != 5)
    {
        if (canA_tzVSC_info[5].CANfailCnt++ >= 2000)
        {
            canA_tzVSC_info[5].CANfailCnt = 2000;
            CANA_tzActNodeRegs_VS.bit.bt_VS5 = 0;
            for (ui16temp = 1; ui16temp <= 16; ui16temp++)
            {
                canA_tzVSC_info[5].f32Cellvolt[ui16temp] = 0;

            }
        }
    }
    else
    {
        canA_tzVSC_info[5].CANfailCnt = 0;
        CANA_tzActNodeRegs_VS.bit.bt_VS5 = 1;

    }

    if (canA_VSCbuff.uiNodeID != 6)
    {
        if (canA_tzVSC_info[6].CANfailCnt++ >= 2000)
        {
            canA_tzVSC_info[6].CANfailCnt = 2000;
            CANA_tzActNodeRegs_VS.bit.bt_VS6 = 0;
            for (ui16temp = 1; ui16temp <= 16; ui16temp++)
            {
                canA_tzVSC_info[6].f32Cellvolt[ui16temp] = 0;

            }
        }
    }
    else
    {
        canA_tzVSC_info[6].CANfailCnt = 0;
        CANA_tzActNodeRegs_VS.bit.bt_VS6 = 1;

    }

    if (canA_VSCbuff.uiNodeID != 7)
    {
        if (canA_tzVSC_info[7].CANfailCnt++ >= 2000)
        {
            canA_tzVSC_info[7].CANfailCnt = 2000;
            CANA_tzActNodeRegs_VS.bit.bt_VS7 = 0;
            for (ui16temp = 1; ui16temp <= 16; ui16temp++)
            {
                canA_tzVSC_info[7].f32Cellvolt[ui16temp] = 0;

            }
        }
    }
    else
    {
        canA_tzVSC_info[7].CANfailCnt = 0;
        CANA_tzActNodeRegs_VS.bit.bt_VS7 = 1;

    }

    if (canA_VSCbuff.uiNodeID != 8)
    {
        if (canA_tzVSC_info[8].CANfailCnt++ >= 2000)
        {
            canA_tzVSC_info[8].CANfailCnt = 2000;
            CANA_tzActNodeRegs_VS.bit.bt_VS8 = 0;
            for (ui16temp = 1; ui16temp <= 16; ui16temp++)
            {
                canA_tzVSC_info[8].f32Cellvolt[ui16temp] = 0;

            }
        }
    }
    else
    {
        canA_tzVSC_info[8].CANfailCnt = 0;
        CANA_tzActNodeRegs_VS.bit.bt_VS8 = 1;

    }

    if (canA_VSCbuff.uiNodeID != 9)
    {
        if (canA_tzVSC_info[9].CANfailCnt++ >= 2000)
        {
            canA_tzVSC_info[9].CANfailCnt = 2000;
            CANA_tzActNodeRegs_VS1.bit.bt_VS9 = 0;
            for (ui16temp = 1; ui16temp <= 16; ui16temp++)
            {
                canA_tzVSC_info[9].f32Cellvolt[ui16temp] = 0;

            }
        }
    }
    else
    {
        canA_tzVSC_info[9].CANfailCnt = 0;
        CANA_tzActNodeRegs_VS1.bit.bt_VS9 = 1;

    }

    if (canA_VSCbuff.uiNodeID != 10)
    {
        if (canA_tzVSC_info[10].CANfailCnt++ >= 2000)
        {
            canA_tzVSC_info[10].CANfailCnt = 2000;
            CANA_tzActNodeRegs_VS1.bit.bt_VS10 = 0;
            for (ui16temp = 1; ui16temp <= 16; ui16temp++)
            {
                canA_tzVSC_info[10].f32Cellvolt[ui16temp] = 0;

            }
        }
    }
    else
    {
        canA_tzVSC_info[10].CANfailCnt = 0;
        CANA_tzActNodeRegs_VS1.bit.bt_VS10 = 1;

    }

    if (canA_VSCbuff.uiNodeID != 11)
    {
        if (canA_tzVSC_info[11].CANfailCnt++ >= 2000)
        {
            canA_tzVSC_info[11].CANfailCnt = 2000;
            for (ui16temp = 1; ui16temp <= 16; ui16temp++)
            {
                canA_tzVSC_info[11].f32Cellvolt[ui16temp] = 0;
                CANA_tzActNodeRegs_VS1.bit.bt_VS11 = 0;

            }
        }
    }
    else
    {
        canA_tzVSC_info[11].CANfailCnt = 0;
        CANA_tzActNodeRegs_VS1.bit.bt_VS11 = 1;

    }

    if (canA_VSCbuff.uiNodeID != 12)
    {
        if (canA_tzVSC_info[12].CANfailCnt++ >= 2000)
        {
            canA_tzVSC_info[12].CANfailCnt = 2000;
            CANA_tzActNodeRegs_VS1.bit.bt_VS12 = 0;

            for (ui16temp = 1; ui16temp <= 16; ui16temp++)
            {
                canA_tzVSC_info[12].f32Cellvolt[ui16temp] = 0;
            }
        }
    }
    else
    {
        canA_tzVSC_info[12].CANfailCnt = 0;
        CANA_tzActNodeRegs_VS1.bit.bt_VS12 = 1;

    }

    if (canA_VSCbuff.uiNodeID != 13)
    {
        if (canA_tzVSC_info[13].CANfailCnt++ >= 2000)
        {
            canA_tzVSC_info[13].CANfailCnt = 2000;
            CANA_tzActNodeRegs_VS1.bit.bt_VS13 = 0;

            for (ui16temp = 1; ui16temp <= 16; ui16temp++)
            {
                canA_tzVSC_info[13].f32Cellvolt[ui16temp] = 0;
            }
        }
    }
    else
    {
        canA_tzVSC_info[13].CANfailCnt = 0;
        CANA_tzActNodeRegs_VS1.bit.bt_VS13 = 1;

    }

    if (canA_VSCbuff.uiNodeID != 14)
    {
        if (canA_tzVSC_info[14].CANfailCnt++ >= 2000)
        {
            canA_tzVSC_info[14].CANfailCnt = 2000;
            CANA_tzActNodeRegs_VS1.bit.bt_VS14 = 0;

            for (ui16temp = 1; ui16temp <= 16; ui16temp++)
            {
                canA_tzVSC_info[14].f32Cellvolt[ui16temp] = 0;
            }
        }
    }
    else
    {
        canA_tzVSC_info[14].CANfailCnt = 0;
        CANA_tzActNodeRegs_VS1.bit.bt_VS14 = 1;

    }

    if (canA_VSCbuff.uiNodeID != 15)
    {
        if (canA_tzVSC_info[15].CANfailCnt++ >= 2000)
        {
            canA_tzVSC_info[15].CANfailCnt = 2000;
            CANA_tzActNodeRegs_VS1.bit.bt_VS15 = 0;

            for (ui16temp = 1; ui16temp <= 16; ui16temp++)
            {
                canA_tzVSC_info[15].f32Cellvolt[ui16temp] = 0;
            }
        }
    }
    else
    {
        canA_tzVSC_info[15].CANfailCnt = 0;
        CANA_tzActNodeRegs_VS1.bit.bt_VS15 = 1;

    }

    if (canA_VSCbuff.uiNodeID != 16)
    {
        if (canA_tzVSC_info[16].CANfailCnt++ >= 2000)
        {
            canA_tzVSC_info[16].CANfailCnt = 2000;
            CANA_tzActNodeRegs_VS1.bit.bt_VS16 = 0;

            for (ui16temp = 1; ui16temp <= 16; ui16temp++)
            {
                canA_tzVSC_info[16].f32Cellvolt[ui16temp] = 0;
            }
        }
    }
    else
    {
        canA_tzVSC_info[16].CANfailCnt = 0;
        CANA_tzActNodeRegs_VS1.bit.bt_VS16 = 1;

    }

}

void CANA_fnTx()
{
    // Common Messages Irrespective of States

    // Below Commands Can be used for Manual Testing Mode

    CANA_tzMSRegs.HBCntMS++;
    if (CANA_tzMSRegs.HBCntMS >= 50)
    {
//        CANA_fnMSTxCmds(testCabID, testNodeID,
//                        &CANA_tzDO[testCabID][testNodeID]); //Heartbeat

        CANA_fnMSTxCmds(testCabID, testNodeID,
                        &CANA_tzDO[testCabID1][testNodeID]); //Heartbeat

        CANA_tzMSRegs.HBCntMS = 0;
    }

    if (STAT_tzStateMac.Present_st == COMMISSION)
    {
        ui16manualTesting = 1;
    }
    else
    {
        ui16manualTesting = 0;
    }

//    // Below 2 Messages can be removed after manual testing
//
//    CANA_tzMSRegs.ManualTestCntMS++;
//    if (CANA_tzMSRegs.ManualTestCntMS == 25)
//    {
//        CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV4 = (testCntVFD * 100);
//
//        CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
//                              &CANA_tzAnaOPParams); // Control Speed of Purge fan1
//    }
//    else if ((CANA_tzMSRegs.ManualTestCntMS == 50))
//    {
//        CANA_tzAnaOPParams.CANA_tzAOI[1][0].AOI1 = (testEBV * 10);
//
//        CANA_fnCmdsForAnaOPIs(CANA_tzIORegs.uiUnitID, 1, 0,
//                              &CANA_tzAnaOPParams);
//
//    }
//
//    else if (CANA_tzMSRegs.ManualTestCntMS > 100)
//    {
//        CANA_tzMSRegs.ManualTestCntMS = 0;
//
//    }



    ui16Cnt++;

    if (ui16Cnt >= 25)
    {

        if (CANA_tzMSRegs.TurnONPurge101 == 1)
        {
            CANA_tzMSRegs.fan101ResetCnt = 0;
            CANA_tzMSRegs.fan101SetCnt++;
            if (CANA_tzMSRegs.fan101SetCnt == 2)
            {
                CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV1 =
                        (CANA_tzMSRegs.AOVFan101 * 100);

                CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                                      &CANA_tzAnaOPParams); // Control Speed of Purge fan1
                CANA_tzMSRegs.fan101SetCnt = 0;
            }

        }
        else
        {

            CANA_tzMSRegs.fan101SetCnt = 0;
            CANA_tzMSRegs.fan101ResetCnt++;
            if (CANA_tzMSRegs.fan101ResetCnt == 2)
            {
                CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV1 = (0);

                CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                                      &CANA_tzAnaOPParams); // Control Speed of Purge fan1
                CANA_tzMSRegs.fan101ResetCnt = 0;
            }
        }

        if (CANA_tzMSRegs.TurnONPurge501 == 1)
        {
            CANA_tzMSRegs.fan501ResetCnt = 0;
            CANA_tzMSRegs.fan501SetCnt++;
            if (CANA_tzMSRegs.fan501SetCnt == 2)
            {
                CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV2 =
                        (CANA_tzMSRegs.AOVFan501 * 100);

                CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                                      &CANA_tzAnaOPParams); // Control Speed of Purge fan1
                CANA_tzMSRegs.fan501SetCnt = 0;
            }

        }
        else
        {

            CANA_tzMSRegs.fan501SetCnt = 0;
            CANA_tzMSRegs.fan501ResetCnt++;
            if (CANA_tzMSRegs.fan501ResetCnt == 2)
            {
                CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV2 = (0);

                CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                                      &CANA_tzAnaOPParams); // Control Speed of Purge fan1
                CANA_tzMSRegs.fan501ResetCnt = 0;
            }
        }

        if (CANA_tzMSRegs.TurnONPurge401 == 1)
        {
            CANA_tzMSRegs.fan401ResetCnt = 0;
            CANA_tzMSRegs.fan401SetCnt++;
            if (CANA_tzMSRegs.fan401SetCnt == 2)
            {
                CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV3 =
                        (CANA_tzMSRegs.AOVFan401 * 100);

                CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                                      &CANA_tzAnaOPParams); // Control Speed of Purge fan1
                CANA_tzMSRegs.fan401SetCnt = 0;
            }

        }
        else
        {

            CANA_tzMSRegs.fan401SetCnt = 0;
            CANA_tzMSRegs.fan401ResetCnt++;
            if (CANA_tzMSRegs.fan401ResetCnt == 2)
            {
                CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV3 = (0);

                CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                                      &CANA_tzAnaOPParams); // Control Speed of Purge fan1
                CANA_tzMSRegs.fan401ResetCnt = 0;
            }
        }

        ui16Cnt = 0;
    }


    switch (STAT_tzStateMac.Present_st)
    {




    case STAND_BY:

        ui16IOcnt++;

        switch (ui16IOcnt)
        {
        case 0:
            CANA_tzDO[0][0].all = 0x0;
            CANA_fnMSTxCmds(3, 1, &CANA_tzDO[0][1]);

            CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV4 = 0;

            CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                                  &CANA_tzAnaOPParams); // Turn OFF PMP101 VFD

            break;

        case 10:
            CANA_tzDO[0][1].all = 0x0;
            CANA_fnMSTxCmds(3, 1, &CANA_tzDO[0][1]);
            break;

        case 20:
            CANA_tzDO[1][0].all = 0x0;
            CANA_fnMSTxCmds(1, 0, &CANA_tzDO[1][0]);
            break;

        case 30:
            CANA_tzDO[1][1].all = 0x0;
            CANA_fnMSTxCmds(1, 1, &CANA_tzDO[1][1]);


            break;

        default:
            break;

        }

        if (ui16IOcnt >= 40)
        {
            ui16IOcnt = 40;
        }



        break;

    case READY:

        ui16IOcnt = 0;

        control_waterloop();

        break;

    case STACK_CHECK:

        ui16IOcnt = 0;

        control_waterloop();

        if (CANA_tzLHCIO1_AIFaultRegs.bit.PRT_102 == 0) // Check Cell Short Also LAter
        {
            ui16StateRstCnt = 0;
            ui16StateTnstCnt++;
            if (ui16StateTnstCnt > 10000)
            {
                ui16StateTnstCnt = 10000;

                CANB_tzSiteRxRegs.Start_cmd = 3;
                CANA_tzIOflags.faultAtStChkSt = 0;
            }
        }
        else
        {
            ui16StateTnstCnt = 0;
            ui16StateRstCnt++;
            if (ui16StateRstCnt > 1000)
            {
                ui16StateRstCnt = 1000;

                CANB_tzSiteRxRegs.Start_cmd = 2; // Remains in STACK CHECK Only
                CANA_tzIOflags.faultAtStChkSt = 1;

                // To be changed to fault State Later
            }
        }

        break;

    case STACK_POWER:

        ui16IOcnt = 0;
        control_waterloop();

        break;

    case FAULT:
        ui16IOcnt = 0;

        safeshutDown();

        break;

    case COMMISSION:
        ui16IOcnt = 0;

        ui16ComsnCnt++;
        if (ui16ComsnCnt >= 10)
        {
            cana_CommisionMode();
            ui16ComsnCnt = 0;
        }

        break;

    default:
        break;
    }

}

void CANA_fnMSTxCmds(uint16_t ui16CabiD, uint16_t NodeID,
                     CANA_tzDIG_OP *ptrDigOP)
{

    // Master Process to Master Safety

    CAN_setupMessageObject(
    CANA_BASE,
                           CAN_mMAILBOX_20,
                           CANA_mTX_MSMSGID1,
                           CAN_MSG_FRAME_EXT, CAN_MSG_OBJ_TYPE_TX, 0,
                           CAN_MSG_OBJ_NO_FLAGS,
                           CAN_mLEN8);

    CANA_tzMSRegs.TxCntMS++;

    if (CANA_tzMSRegs.TxCntMS >= 255)
    {
        CANA_tzMSRegs.TxCntMS = 0;
    }

    uiCANtxMsgDataMS[0] = CANA_tzMSRegs.TxCntMS;

    uiCANtxMsgDataMS[1] = STAT_tzStateMac.Present_st;

    uiCANtxMsgDataMS[2] = ptrDigOP->all;

    uiCANtxMsgDataMS[3] = ui16CabiD;
    uiCANtxMsgDataMS[4] = NodeID;

    uiCANtxMsgDataMS[5] = CONTROLtzFaultRegs.bit.LPCCurHealthy;

    uiCANtxMsgDataMS[6] = CANA_tzLHCIO1_AIFaultRegs.all;
    uiCANtxMsgDataMS[7] = CANA_tzLHCIO2_AIFaultRegs.all;

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_20, CAN_mLEN8, uiCANtxMsgDataMS);

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
            CAN_mMAILBOX_9,
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

    CAN_sendMessage(CANA_BASE, CAN_mMAILBOX_9, CAN_mLEN8, ui16txMsgDataIO);
}

void CANA_fnIOHrtBt()
{
//    CANA_tzIOtimers.HrtbtCntIOCom++;
//
//    switch (CANA_tzIOtimers.HrtbtCntIOCom)
//    {
//    case 1:
//
//        CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 3, 0, &CANA_tzDO[0][0]); //LPCIO1
//        break;
//
//    case 2:
//
//        CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 3, 1, &CANA_tzDO[0][1]); //LPCIO2
//        break;
//
//    case 3:
//
//        CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1, 0, &CANA_tzDO[1][0]); //LHCIO1
//        break;
//
//    case 4:
//
//        CANA_fnCmdsForDigOPs(CANA_tzIORegs.uiUnitID, 1, 1, &CANA_tzDO[1][1]); //LHCIO2
//        CANA_tzIOtimers.HrtbtCntIOCom = 0;
//
//        break;
//
//    default:
//        break;
//    }
}

void safeshutDown()
{

    CANA_tzMSRegs.ResetIOsInflt++;
    if (CANA_tzMSRegs.ResetIOsInflt >= 50)
    {

        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO].bit.DO0 = 0x0;
        CANA_tzDO[CANA_mLHC_CABID][CANA_mLHC10_IO].bit.DO3 = 0x0;

        CANA_fnMSTxCmds(1, 0, &CANA_tzDO[1][0]); //Turn OFF WSV101

        CANA_fnMSTxCmds(testCabID, testNodeID,
                        &CANA_tzDO[testCabID][testNodeID]); //Heartbeat
        CANA_tzMSRegs.ResetIOsInflt = 0;
    }

    CANA_tzMSRegs.ResetAOsInflt++;
    if (CANA_tzMSRegs.ResetAOsInflt == 50)
    {
        CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV4 = (testCntVFD * 100);

        CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                              &CANA_tzAnaOPParams); // Control Speed of Purge fan1
    }
    else if (CANA_tzMSRegs.ResetAOsInflt == 100)
    {
        CANA_tzAnaOPParams.CANA_tzAOI[1][0].AOI1 = (testEBV * 10);

        CANA_fnCmdsForAnaOPIs(CANA_tzIORegs.uiUnitID, 1, 0,
                              &CANA_tzAnaOPParams);

    }
    else if (CANA_tzMSRegs.ResetAOsInflt > 100)
    {
        CANA_tzMSRegs.ResetAOsInflt = 0;

    }
}


static float32_t limitAnalogSensorData(float32_t SensorType)
{
    if (SensorType > 21.0f)
    {
        SensorType = 21.0f;
    }
    if (SensorType < 3.0f)
    {
        SensorType = 3.0f;
    }
    return (float32_t)(SensorType);
}

void cana_CommisionMode()
{

    if (CANA_tzQueryType.PSU == QUERY_PROGPARAM)
    {
        CANA_tzQueryType.PSU = SET_VOLT;
    }
    if (ui16manualTesting == 1)
    {

        CANA_tzMSRegs.ManualTestCntMS++;

        if (CANA_tzMSRegs.ManualTestCntMS == 1)
        {

            CANA_fnMSTxCmds(testCabID, testNodeID,
                            &CANA_tzDO[testCabID1][testNodeID]); //Heartbeat
        }
        if (CANA_tzMSRegs.ManualTestCntMS == 10)
        {
            CANA_tzAnaOPParams.CANA_tzAOV[0][0].AOV4 = (testCntVFD * 100);

            CANA_fnCmdsForAnaOPVs(CANA_tzIORegs.uiUnitID, 3, 0,
                                  &CANA_tzAnaOPParams); // Control Speed of Purge fan1
        }
        else if ((CANA_tzMSRegs.ManualTestCntMS == 20))
        {
            CANA_tzAnaOPParams.CANA_tzAOI[1][0].AOI1 = (testEBV * 100);

            CANA_fnCmdsForAnaOPIs(CANA_tzIORegs.uiUnitID, 1, 0,
                                  &CANA_tzAnaOPParams);

        }

        else if (CANA_tzMSRegs.ManualTestCntMS > 21)
        {
            CANA_tzMSRegs.ManualTestCntMS = 0;

        }

//        if (CANA_tzQueryType.PSU == SET_VOLT)
//        {
//            CANA_tzTimerRegs.tzPSU.TxManCount++;
//
//            cana_fnSetVoltage_PSU(0, CANB_tzSiteRxRegs.f32VoltSet, 1);
//
//            if (CANA_tzTimerRegs.tzPSU.TxManCount > 2)
//            {
//                CANA_tzQueryType.PSU = QUERY_FLTS;
//
//                CANA_tzTimerRegs.tzPSU.TxManCount = 0;
//            }
//        }
//
//        else if (CANA_tzQueryType.PSU == QUERY_FLTS)
//        {
//            CANA_tzTimerRegs.tzPSU.TxManCount++;
//
//            cana_fnQryFaultsParam_PSU(CANA_tzTimerRegs.tzPSU.TxManCount);
//
//            if (CANA_tzTimerRegs.tzPSU.TxManCount > CANA_mTOTAL_PSUNODE)
//            {
//                CANA_tzTimerRegs.tzPSU.TxManCount = 0;
//
//                CANA_tzQueryType.PSU = SET_CURR;
//
//            }
//
//        }
//
//        else if (CANA_tzQueryType.PSU == SET_CURR)
//        {
//            CANA_tzTimerRegs.tzPSU.TxManCount++;
//
//            cana_fnSetCurrent_PSU(0, CANA_tzTxdRegs.tzPSUData.CurrentSet, 1);
//
//            if (CANA_tzTimerRegs.tzPSU.TxManCount > 2)
//            {
//                CANA_tzQueryType.PSU = QUERY_ACPARAMS;
//
//                CANA_tzTimerRegs.tzPSU.TxManCount = 0;
//            }
//        }
//
//        else if (CANA_tzQueryType.PSU == QUERY_ACPARAMS)
//        {
//            CANA_tzTimerRegs.tzPSU.TxManCount++;
//
//            cana_fnQryACParam_PSU(CANA_tzTimerRegs.tzPSU.TxManCount);
//
//            if (CANA_tzTimerRegs.tzPSU.TxManCount > 10)
//            {
//                CANA_tzTimerRegs.tzPSU.TxManCount = 0;
//
//                CANA_tzQueryType.PSU = QUERY_OP_PARAM;
//            }
//        }
//
//        else if (CANA_tzQueryType.PSU == QUERY_OP_PARAM)
//        {
//            CANA_tzTimerRegs.tzPSU.TxManCount++;
//
//            cana_fnQryOutputParam_PSU(CANA_tzTimerRegs.tzPSU.TxManCount);
//
//            if (CANA_tzTimerRegs.tzPSU.TxManCount > CANA_mTOTAL_PSUNODE)
//            {
//                CANA_tzTimerRegs.tzPSU.TxManCount = 0;
//                CANA_tzQueryType.PSU = TURN_ON;
//
//            }
//
//        }
//
//        else if (CANA_tzQueryType.PSU == TURN_ON)
//        {
//            cana_fnTurnON_PSU(0, PSUCommand, 1);
//
//            CANA_tzTimerRegs.tzPSU.TxManCount++;
//
//            if (CANA_tzTimerRegs.tzPSU.TxManCount > 2)
//            {
//                CANA_tzTimerRegs.tzPSU.TxManCount = 0;
//                CANA_tzQueryType.PSU = QUERY_FLTS;
//
//            }
//        }
//        else if (CANA_tzQueryType.PSU == QUERY_FLTS)
//        {
//            CANA_tzTimerRegs.tzPSU.TxManCount++;
//
//            cana_fnQryFaultsParam_PSU(CANA_tzTimerRegs.tzPSU.TxCount);
//
//            if (CANA_tzTimerRegs.tzPSU.TxManCount > CANA_mTOTAL_PSUNODE)
//            {
//                CANA_tzTimerRegs.tzPSU.TxManCount = 0;
//                CANA_tzQueryType.PSU = SET_VOLT;
//
//            }
//
//        }

    }
}
/*==============================================================================
 End of File
 ==============================================================================*/
