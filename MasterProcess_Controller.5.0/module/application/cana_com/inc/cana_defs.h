/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================

 @file  oi_can.h
 @author DEEPTI.K
 @date 06-Sep-2021

 @brief Description of CANA module defs that has PSU DATA, IO CARD and VS CARD DATA
 ==============================================================================*/

#ifndef MODULE_CANA_COM_INC_CANA_DEFS_H_
#define MODULE_CANA_COM_INC_CANA_DEFS_H_

/*==============================================================================
 Includes
 ==============================================================================*/

#include <module/application/cana_com/inc/cana_defs.h>
#include "F28x_Project.h"
#include "inc/hw_can.h"
#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

/*==============================================================================
 Defines
 ==============================================================================*/

#define CANA_mTOTAL_PSUNODE                                                                                  (28U)
#define CANA_mTOTAL_IONODE                                                                                   (4U)
#define CANA_mTOTAL_VSNODE                                                                                   (12U)

#define CANA_mNODEID_MASKPSU                                                                                  (0x0000FF00U)
#define CANA_mNODEID_MASKIO                                                                                  (0x0000FF00U)
#define CANA_mNODEID_MASKVS                                                                                  (0x0000FF00U)

#define CANA_mPSUTIMEOUT                                                                                      (150U)
#define CANA_mIOTIMEOUT                                                                                      (150U)

//Data length
#define CANA_mONE_BYTE                                             (1U)
#define CANA_mTWO_BYTE                                             (2U)
#define CANA_mTHREE_BYTE                                           (3U)
#define CANA_mFOUR_BYTE                                            (4U)
#define CANA_mFIVE_BYTE                                            (5U)
#define CANA_mSIX_BYTE                                             (6U)
#define CANA_mSEVEN_BYTE                                           (7U)
#define CANA_mEIGHT_BYTE                                           (8U)

#define CANA_mFLASH_WRITE_FAIL                                     (0x02U)

#define CANA_mFLASH_ERRORBYTE                                      (0xF0U)
#define CANA_mAPP_FLASH_LEN                                              (0x012000)
#define CANA_mRX_BUFFER_SIZE                                   (8U)

#define CAN_mFLASH_ERR_COMM_TIMEOUT                                      (0x0001)
#define CAN_mFLASH_WRITE_FAIL                                            (0x0002)
#define CAN_mFLASH_CHKSUM_MISMATCH                                       (0x0003)
#define CAN_mBYTES                                                       (7U)
#define CAN_mERROR_BYTE                                                  (0xF0)
#define CAN_mBUFFER_SIZE                                                 (224U)

#define CAN_mPOLYNOMIAL                                                 (0x04C11DB7L)
#define APPROX_1_SECOND_DELAY                                           (5000000UL)

#define CANA_mRX_MSGID1                                                 (0x11042130)
#define CANA_mRX_MSGID2                                                 (0x11042110)
//#define CANA_mRX_MSGID3                                                 (0x11742000)

#define CAN_mMAILBOX_1                                                  (1U)
#define CAN_mMAILBOX_2                                                  (2U)
#define CAN_mMAILBOX_3                                                  (3U)
#define CAN_mMAILBOX_4                                                  (4U)
#define CAN_mMAILBOX_5                                                  (5U)
#define CAN_mMAILBOX_6                                                  (6U)
#define CAN_mMAILBOX_7                                                  (7U)
#define CAN_mMAILBOX_8                                                  (8U)
#define CAN_mMAILBOX_9                                                  (9U)
#define CAN_mMAILBOX_10                                                 (10U)
#define CAN_mMAILBOX_11                                                 (11U)
#define CAN_mMAILBOX_12                                                 (12U)
#define CAN_mMAILBOX_13                                                 (13U)
#define CAN_mMAILBOX_14                                                 (14U)
#define CAN_mMAILBOX_15                                                 (15U)
#define CAN_mMAILBOX_16                                                 (16U)
#define CAN_mMAILBOX_17                                                 (17U)
#define CAN_mMAILBOX_18                                                 (18U)
#define CAN_mMAILBOX_19                                                 (19U)
#define CAN_mMAILBOX_20                                                 (20U)
#define CAN_mMAILBOX_21                                                 (21U)
#define CAN_mMAILBOX_22                                                 (22U)
#define CAN_mMAILBOX_23                                                 (23U)
#define CAN_mMAILBOX_24                                                 (24U)
#define CAN_mMAILBOX_25                                                 (25U)
#define CAN_mMAILBOX_26                                                 (26U)
#define CAN_mMAILBOX_27                                                 (27U)
#define CAN_mMAILBOX_28                                                 (28U)
#define CAN_mMAILBOX_29                                                 (29U)
#define CAN_mMAILBOX_30                                                 (30U)
#define CAN_mMAILBOX_31                                                 (31U)
#define CAN_mMAILBOX_32                                                 (32U)

#define CAN_mLEN1                                                       (1U)
#define CAN_mLEN2                                                       (2U)
#define CAN_mLEN3                                                       (3U)
#define CAN_mLEN4                                                       (4U)
#define CAN_mLEN5                                                       (5U)
#define CAN_mLEN6                                                       (6U)
#define CAN_mLEN7                                                       (7U)
#define CAN_mLEN8                                                       (8U)

// General msg IDs used for msg id creation

#define CANA_mLPC130_IO                                                  (0U)
#define CANA_mLPC131_IO                                                  (1U)
#define CANA_mLHC110_IO                                                  (0U)
#define CANA_mLHC111_IO                                                  (1U)

#define    IO_DI_MSGID                          (0U)
#define    IO_AIBLK1_MSGID                      (1U)
#define    IO_AIBLK2_MSGID                      (2U)
#define    IO_THERMAL_MSGID                     (3U)
#define    IO_DI1_FREQ_MSGID                    (4U)
#define    IO_DI2_FREQ_MSGID                    (5U)

#define LPC_130                                 (0U)
#define LPC_131                                 (1U)
#define LHC_110                                 (0U)
#define LHC_111                                 (1U)

//#define CANA_mBASEID_FOR_TRANSMIT_PSU                                                                        (0x10000000U)
//#define CANA_mBASEID_FOR_PRODUCT_PSU                                                                         (0x01000000U)
//#define CANA_mBASEID_FOR_FAMILY_PSU                                                                          (0x00100000U)
//#define CANA_mBASEID_FOR_MODEL_PSU                                                                           (0x000B0000U)
//#define CANA_mBASEID_FOR_GLOBAL_TURNON_PSU                                                                   (0x00000004U)
//#define CANA_mBASEID_FOR_GLOBAL_VOLTSET_PSU                                                                  (0x00000005U)
//#define CANA_mBASEID_FOR_GLOBAL_CURRSET_PSU                                                                  (0x00000006U)
//
//#define CANA_mBASEID_FOR_MEASPARAMS_PSU                                                                          (0x00000020U)
//#define CANA_mBASEID_FOR_PROGPARAMS_PSU                                                                          (0x00000020U)
//#define CANA_mBASEID_FOR_FAULTS_PSU                                                                              (0x00000020U)
//#define CANA_mBASEID_FOR_ACPARAMS_PSU                                                                            (0x00000020U)
//
//#define CANA_mMSGID_GLOBAL_COMMAND_TO_TURNON_PSU                                                             (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU +  CANA_mBASEID_FOR_GLOBAL_TURNON_PSU)
//#define CANA_mMSGID_GLOBAL_COMMAND_TO_VOLTSET_PSU                                                            (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU +  CANA_mBASEID_FOR_GLOBAL_VOLTSET_PSU)
//#define CANA_mMSGID_GLOBAL_COMMAND_TO_CURRSET_PSU                                                            (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU +  CANA_mBASEID_FOR_GLOBAL_CURRSET_PSU)
//
//#define CANA_mMSGID_QRY_MODELSPC_MEASPARAM_PSU                                                              CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU + CANA_mBASEID_FOR_MODEL_PSU + CANA_mBASEID_FOR_MEASPARAMS_PSU
//#define CANA_mMSGID_QRY_MODELSPC_PROGPARAM_PSU                                                              CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU + CANA_mBASEID_FOR_MODEL_PSU + CANA_mBASEID_FOR_PROGPARAMS_PSU
//#define CANA_mMSGID_QRY_MODELSPC_FAULTS_PSU                                                                 CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU + CANA_mBASEID_FOR_MODEL_PSU + CANA_mBASEID_FOR_FAULTS_PSU
//#define CANA_mMSGID_QRY_MODELSPC_ACPARAM_PSU                                                                CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU + CANA_mBASEID_FOR_MODEL_PSU + CANA_mBASEID_FOR_ACPARAMS_PSU
//
//#define CANA_mTURNOFF_DCDC                                        (0x0)
//#define CANA_mTURNON_DCDC                                         (0xAA)
//#define CANA_mTURNON_FEC                                          (0xBB)
//
//#define CANA_mMSGID_QRY_MODELSPC_IO                               (0x1421000A)
//#define CANA_mLPC100_IO                                          (0U)
//#define CANA_mLHC101_IO                                         (1U)
//#define CANA_mLHC102_IO                                       (2U)
//
//#define CANA_mTXLPC100_IO                                        (1U)
//#define CANA_mTXLHC101_IO                                       (2U)
//#define CANA_mTXLHC102_IO                                       (3U)
//
//#define CANA_mDONTCARE_IO                                         (2U)
//#define CANA_mDO_SET_IO                                           (1U)
//#define CANA_mDO_CLEAR_IO                                         (0U)
//
//#define CANA_mGENERAL_QRY_IO                                      (0U)
//#define CANA_mCOMMAND_QRY_AIBLK1_IO                               (1U)
//#define CANA_mCOMMAND_QRY_AIBLK2_IO                               (2U)
//#define CANA_mCOMMAND_QRY_THERMO_IO                               (3U)
//#define CANA_mCOMMAND_QRY_GENERIC_IO                               (4U)
//#define CANA_mCOMMAND_EBV_TURNON_IO                               (5U)
//
//#define CANA_mQRY_AIBLK1_IO                                       (0x01U)
//#define CANA_mQRY_AIBLK2_IO                                       (0x02U)
//#define CANA_mQRY_THERMO_IO                                       (0x03U)
//#define CANA_mQRY_GENERIC_IO                                      (0x04U)
//
//#define CANA_mDEFAULT_SETDO_LPC100_IO                             (0x00U)
//#define CANA_mDEFAULT_SETDO_LHC101_IO                             (0x00U)
//#define CANA_mDEFAULT_SETDO_LHC102_IO                             (0x00U)
//
//#define CANA_mDEFAULT_SETAI_LPC100_IO                             (0x00U)
//#define CANA_mDEFAULT_SETAI_LHC101_IO                             (0x00U)
//#define CANA_mDEFAULT_SETAI_LHC102_IO                             (0x00U)
//
//#define CANA_mLPCIO_SETAI_FAILCNT_RESET        (0U)
//#define CANA_mLPCIO_SETAI_FAILCNT_SET          (5U)
//
//#define CANA_mLPCIO_PURGEFAN1_ONCNT_RESET      (0U)
//#define CANA_mLPCIO_PURGEFAN1_ONCNT_SET        (5U)
//
//#define CANA_mLPCIO_PURGEFAN1_OFFCNT_RESET     (0U)
//#define CANA_mLPCIO_PURGEFAN1_OFFCNT_SET       (5U)
//
//#define CANA_mLPCIO_PURGEFAN2_ONCNT_RESET      (0U)
//#define CANA_mLPCIO_PURGEFAN2_ONCNT_SET        (5U)
//
//#define CANA_mLPCIO_PURGEFAN2_OFFCNT_RESET      (0U)
//#define CANA_mLPCIO_PURGEFAN2_OFFCNT_SET        (5U)
//
//#define CANA_mLPCIO_PURGEFAN3_ONCNT_RESET       (0U)
//#define CANA_mLPCIO_PURGEFAN3_ONCNT_SET         (5U)
//
//#define CANA_mLPCIO_PURGEFAN3_OFFCNT_RESET      (0U)
//#define CANA_mLPCIO_PURGEFAN3_OFFCNT_SET        (5U)
//
//#define CANA_mLPCIO_DCFAN_ONCNT_RESET           (0U)
//#define CANA_mLPCIO_DCFAN_ONCNT_SET             (5U)
//
//#define CANA_mLPCIO_DCFAN_OFFCNT_RESET          (0U)
//#define CANA_mLPCIO_DCFAN_OFFCNT_SET            (5U)
//
//#define CANA_mLPCIO_ACFAN_ONCNT_RESET           (0U)
//#define CANA_mLPCIO_ACFAN_ONCNT_SET             (5U)
//
//#define CANA_mLPCIO_ACFAN_OFFCNT_RESET          (0U)
//#define CANA_mLPCIO_ACFAN_OFFCNT_SET            (5U)
//
//#define CANA_mLPCIO_HYS_QRYCNT_RESET            (0U)
//#define CANA_mLPCIO_HYS_QRYCNT_SET              (5U)
//
//#define CANA_mLPCIO_WLS_QRYCNT_RESET            (0U)
//#define CANA_mLPCIO_WLS_QRYCNT_SET              (5U)
//
//#define CANA_mLPCIO_LHCONCNT_RESET              (0U)
//#define CANA_mLPCIO_LHCONCNT_SET                (5U)
//
//#define CANA_mLPCIO_LHCOFFCNT_RESET             (0U)
//#define CANA_mLPCIO_LHCOFFCNT_SET               (5U)
//
//#define CANA_mLHC1IO_SETAI_FAIL_RESET           (0U)
//#define CANA_mLHC1IO_SETAI_FAIL_SET             (5U)
//
//#define CANA_mLHC2IO_SETAI_FAIL_RESET           (0U)
//#define CANA_mLHC2IO_SETAI_FAIL_SET             (5U)
//
//#define CANA_mLHC1IO_LVLQRY_FAILCNT_RESET       (0U)
//#define CANA_mLHC1IO_LVLQRY_FAILCNT_SET         (5U)
//
//#define CANA_mLHC2IO_LVLQRY_FAILCNT_RESET       (0U)
//#define CANA_mLHC2IO_LVLQRY_FAILCNT_SET         (5U)
//
//#define CANA_mLHC1IO_TURNON_WSV_RESET           (0U)
//#define CANA_mLHC1IO_TURNON_WSV_SET             (5U)
//
//#define CANA_mLHC1IO_TURNON_WSV_RESET1           (0U)
//#define CANA_mLHC1IO_TURNON_WSV_SET1             (5U)
//
//#define CANA_mLHC1IO_TURNOFF_WSV_RESET          (0U)
//#define CANA_mLHC1IO_TURNOFF_WSV_SET            (5U)
//
//#define CANA_mLHC1IO_TURNOFF_WSV_RESET1          (0U)
//#define CANA_mLHC1IO_TURNOFF_WSV_SET1            (5U)
//
//#define CANA_mLPCIO_TURNON_VFD_RESET            (0U)
//#define CANA_mLPCIO_TURNON_VFD_SET              (5U)
//
//#define CANA_mLPCIO_TURNOFF_VFD_RESET            (0U)
//#define CANA_mLPCIO_TURNOFF_VFD_SET              (5U)
//
//#define CANA_mLPCIO_TURNON_VFD_RESET1            (0U)
//#define CANA_mLPCIO_TURNON_VFD_SET1              (5U)
//
//#define CANA_mLPCIO_TURNON_EBV_RESET            (0U)
//#define CANA_mLPCIO_TURNON_EBV_SET              (5U)
//
//#define CANA_mLPCIO_TURNOFF_EBV_RESET            (0U)
//#define CANA_mLPCIO_TURNOFF_EBV_SET              (5U)
//
//// General msg IDs used for msg id creation
//#define CANA_mBASEID_FOR_TRANSMIT_VS                                                                        0x10000000
//#define CANA_mBASEID_FOR_PRODUCT_VS                                                                         0x04000000
//#define CANA_mBASEID_FOR_FAMILY_VS                                                                          0x00100000
//#define CANA_mBASEID_FOR_MODEL_VS                                                                           0x00010000
//#define CANA_mBASEID_FOR_CELLVOLT_VS                                                                        0x00000001
//
//#define CANA_mMSGID_QRY_NODESPC_CELLVOLT_VS                                                                CANA_mBASEID_FOR_TRANSMIT_VS + CANA_mBASEID_FOR_PRODUCT_VS + CANA_mBASEID_FOR_FAMILY_VS + CANA_mBASEID_FOR_MODEL_VS + CANA_mBASEID_FOR_CELLVOLT_VS

/*==============================================================================
 Defines
 ==============================================================================*/
#define CANA_mMAILBOX_1                                            (1U)
#define CANA_mMAILBOX_2                                            (2U)
#define CANA_mMAILBOX_3                                            (3U)
#define CANA_mMAILBOX_4                                            (4U)
#define CANA_mMAILBOX_5                                            (5U)
#define CANA_mMAILBOX_6                                            (6U)
#define CANA_mMAILBOX_7                                            (7U)
#define CANA_mMAILBOX_8                                            (8U)
#define CANA_mMAILBOX_9                                            (9U)
#define CANA_mMAILBOX_10                                           (10U)
#define CANA_mMAILBOX_11                                           (11U)
#define CANA_mMAILBOX_12                                           (12U)
#define CANA_mMAILBOX_13                                           (13U)
#define CANA_mMAILBOX_14                                           (14U)
#define CANA_mMAILBOX_15                                           (15U)
#define CANA_mMAILBOX_16                                           (16U)
#define CANA_mMAILBOX_17                                           (17U)
#define CANA_mMAILBOX_18                                           (18U)
#define CANA_mMAILBOX_19                                           (19U)
#define CANA_mMAILBOX_20                                           (20U)
#define CANA_mMAILBOX_21                                           (21U)
#define CANA_mMAILBOX_22                                           (22U)
#define CANA_mMAILBOX_23                                           (23U)
#define CANA_mMAILBOX_24                                           (24U)
#define CANA_mMAILBOX_25                                           (25U)
#define CANA_mMAILBOX_26                                           (26U)
#define CANA_mMAILBOX_27                                           (27U)
#define CANA_mMAILBOX_28                                           (28U)
#define CANA_mMAILBOX_29                                           (29U)
#define CANA_mMAILBOX_30                                           (30U)
#define CANA_mMAILBOX_31                                           (31U)
#define CANA_mMAILBOX_32                                           (32U)

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/

typedef struct CAN_zRX_CIRCULAR_BUF
{
    uint16_t uiDataBuffer[CANA_mRX_BUFFER_SIZE];
    uint32_t u32_msgID;
    int16_t i_DataLength;

} CAN_tzRX_CIRCULAR_BUF;

typedef struct can_zcirc_buff
{
    CAN_tzRX_CIRCULAR_BUF canA_tzRxC_buffer[20];
    int16_t i_head;
    int16_t i_tail;
    const int16_t i_maxlen;
} can_tzcirc_buff;

typedef struct CANA_zAO_IOREGS
{
    uint16_t AO_1;
    uint16_t AO_2;
    uint16_t AO_3;
    uint16_t AO_4;

} CANA_tzAO_IOREGS;

typedef struct CANA_zAI_IOREGS
{
    float32_t AI0_Data;
    float32_t AI1_Data;
    float32_t AI2_Data;
    float32_t AI3_Data;
    float32_t AI4_Data;
    float32_t AI5_Data;
    float32_t AI6_Data;
    float32_t AI7_Data;
} CANA_tzAI_IOREGS;

typedef struct CANA_zIOREGS
{

    uint16_t Node;
    uint16_t RxCntLPC130;
    uint16_t RxCntLPC131;
    uint16_t RxCntLHC110;
    uint16_t RxCntLHC111;
    uint16_t uiMsgtypeLPCIO;
    uint16_t uiMsgtypeLHCIO;
    uint16_t uiNodeLPCIO;
    uint16_t uiNodeLHCIO;
    float32_t CJC[CANA_mTOTAL_IONODE];

} CANA_tzIOREGS;


struct CANA_tzDI_IO_BITS
{
    uint16_t DI_bit0 :1;
    uint16_t DI_bit1 :1;
    uint16_t DI_bit2 :1;
    uint16_t DI_bit3 :1;
    uint16_t DI_bit4 :1;
    uint16_t DI_bit5 :1;
    uint16_t DI_bit6 :1;
    uint16_t DI_bit7 :1;
};

union CANA_tzDI_IOREGS
{
    uint16_t all;
    struct CANA_tzDI_IO_BITS bit;
};


struct CANA_tzAIFLT_IO_BITS
{
    uint16_t AIFlt_bit0 :1;
    uint16_t AIFlt_bit1 :1;
    uint16_t AIFlt_bit2 :1;
    uint16_t AIFlt_bit3 :1;
    uint16_t AIFlt_bit4 :1;
    uint16_t AIFlt_bit5 :1;
    uint16_t AIFlt_bit6 :1;
    uint16_t AIFlt_bit7 :1;
};

union CANA_tzAIFLT_IOREGS
{
    uint16_t all;
    struct CANA_tzAIFLT_IO_BITS bit;
};

typedef struct CANA_zTHERMAL_IOREGS
{
    float32_t T0_Data;
    float32_t T1_Data;
    float32_t T2_Data;
    float32_t T3_Data;
} CANA_tzTHERMAL_IOREGS;

typedef struct CANA_zAISENSOR_DATA
{
    float32_t HYS_101;
    float32_t HYS_501;
    float32_t HYS_401;
    float32_t HYS_102;
    float32_t OXS_101;
    float32_t LVL_101;
    float32_t PRT_101;
    float32_t PRT_102;
    float32_t COS_101;
    float32_t PRT_401;
    float32_t PRT_402;
    float32_t TE_401;
    float32_t DPT_401;



} CANA_tzAISENSOR_DATA;

typedef struct CANA_zDISENSOR_DATA
{
    float32_t PURGE101;
    float32_t PURGE102;
    float32_t PURGE501;
    float32_t PURGE502;
    float32_t PURGE401;
    float32_t PURGE402;

} CANA_tzDISENSOR_DATA;

typedef struct CANA_zTHERMOCOUPLE_DATA
{
    float32_t TTC_101;
    float32_t TTC_102;
    float32_t TTC_301;

    float32_t KTC_401;
    float32_t KTC_402;
    float32_t KTC_801;
    float32_t CJC_Temp;

} CANA_tzTHERMOCOUPLE_DATA;

typedef struct CANA_zDIFREQ_IOREGS
{
    float32_t DI0_FreqData;
    float32_t DI1_FreqData;
    float32_t DI2_FreqData;
    float32_t DI3_FreqData;
    float32_t DI4_FreqData;
    float32_t DI5_FreqData;
    float32_t DI6_FreqData;
    float32_t DI7_FreqData;
} CANA_tzDIFREQ_IOREGS;


/*==============================================================================
 Macros
 ==============================================================================*/

#define CIRC_BUF_DEF(x,y)                \
    can_tzcirc_buff x = {                 \
        .i_head = 0,                      \
        .i_tail = 0,                      \
        .i_maxlen = y                     \
    }

/*==============================================================================
 Extern/Public Function Prototypes
 ==============================================================================*/

extern void CANA_FlashEvent(void);
extern void CANA_fnInitMBox(void);
extern void CANA_fnRXevent(void);
extern void CANA_fnTask(void);
/*==============================================================================
 Extern/Public Variables
 ==============================================================================*/

extern CANA_tzIOREGS CANA_tzIORegs;
extern CANA_tzAO_IOREGS CANA_tzAOData_IORegs[CANA_mTOTAL_IONODE],
        CANA_tzSetAOData_IORegs[CANA_mTOTAL_IONODE];

extern CANA_tzTHERMAL_IOREGS CANA_tzThermal_IORegs[CANA_mTOTAL_IONODE];
extern CANA_tzAISENSOR_DATA CANA_tzAISensorData;
extern CANA_tzDISENSOR_DATA CANA_tzDISensorData;

extern CANA_tzTHERMOCOUPLE_DATA CANA_tzThermoCoupleData;
extern CANA_tzDIFREQ_IOREGS CANA_tzAIDataFreq_IORegs[CANA_mTOTAL_IONODE];
extern union CANA_tzDI_IOREGS CANA_tzDI_IORegs[CANA_mTOTAL_IONODE];
extern union CANA_tzAIFLT_IOREGS CANA_tzAIFlt_IORegs[CANA_mTOTAL_IONODE];

/*==============================================================================
 Extern/Public Constants
 ==============================================================================*/

#endif /* MODULE_CANA_COM_INC_CANA_DEFS_H_ */
