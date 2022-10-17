/*=============================================================================
 Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
 All trademarks are owned by Enarka India Private Limited
 ============================================================================ */

/*==============================================================================
 @file  cana_PSUCom.h
 @author DEEPTI.K
 @date 01-Jul-2022

 @brief Description
 ==============================================================================*/
#ifndef MODULE_APPLICATION_CANA_PSUCOM_C_INC_CANA_PSUCOM_H_
#define MODULE_APPLICATION_CANA_PSUCOM_C_INC_CANA_PSUCOM_H_

/*==============================================================================
 Includes
 ==============================================================================*/

#include "F28x_Project.h"
#include "inc/hw_can.h"
#include <stdint.h>
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"

/*==============================================================================
 Defines
 ==============================================================================*/

// General msg IDs used for msg id creation
#define CANA_mBASEID_FOR_TRANSMIT_PSU                                                                        (0x10000000U)
#define CANA_mBASEID_FOR_PRODUCT_PSU                                                                         (0x01000000U)
#define CANA_mBASEID_FOR_FAMILY_PSU                                                                          (0x00100000U)
#define CANA_mBASEID_FOR_MODEL_PSU                                                                           (0x000B0000U)
#define CANA_mBASEID_FOR_GLOBAL_TURNON_PSU                                                                   (0x00000004U)
#define CANA_mBASEID_FOR_GLOBAL_VOLTSET_PSU                                                                  (0x00000005U)
#define CANA_mBASEID_FOR_GLOBAL_CURRSET_PSU                                                                  (0x00000006U)

#define CANA_mBASEID_FOR_MEASPARAMS_PSU                                                                          (0x00000020U)
#define CANA_mBASEID_FOR_PROGPARAMS_PSU                                                                          (0x00000020U)
#define CANA_mBASEID_FOR_FAULTS_PSU                                                                              (0x00000020U)
#define CANA_mBASEID_FOR_ACPARAMS_PSU                                                                            (0x00000020U)
#define CANA_mBASEID_FOR_IMP_PSU                                                                                 (0x00000015U)

#define CANA_mMSGID_GLOBAL_COMMAND_TO_TURNON_PSU                                                             (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU +  CANA_mBASEID_FOR_GLOBAL_TURNON_PSU)
#define CANA_mMSGID_GLOBAL_COMMAND_TO_VOLTSET_PSU                                                            (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU +  CANA_mBASEID_FOR_GLOBAL_VOLTSET_PSU)
#define CANA_mMSGID_GLOBAL_COMMAND_TO_CURRSET_PSU                                                            (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU +  CANA_mBASEID_FOR_GLOBAL_CURRSET_PSU)

#define CANA_mMSGID_QRY_MODELSPC_MEASPARAM_PSU                                                              (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU + CANA_mBASEID_FOR_MODEL_PSU + CANA_mBASEID_FOR_MEASPARAMS_PSU)
#define CANA_mMSGID_QRY_MODELSPC_PROGPARAM_PSU                                                              (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU + CANA_mBASEID_FOR_MODEL_PSU + CANA_mBASEID_FOR_PROGPARAMS_PSU)
#define CANA_mMSGID_QRY_MODELSPC_FAULTS_PSU                                                                 (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU + CANA_mBASEID_FOR_MODEL_PSU + CANA_mBASEID_FOR_FAULTS_PSU)
#define CANA_mMSGID_QRY_MODELSPC_ACPARAM_PSU                                                                (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU + CANA_mBASEID_FOR_MODEL_PSU + CANA_mBASEID_FOR_ACPARAMS_PSU)
#define CANA_mMSGID_QRY_MODELSPC_IMPSET_PSU                                                                 (CANA_mBASEID_FOR_TRANSMIT_PSU + CANA_mBASEID_FOR_PRODUCT_PSU + CANA_mBASEID_FOR_FAMILY_PSU + CANA_mBASEID_FOR_MODEL_PSU + CANA_mBASEID_FOR_IMP_PSU)

#define CANA_mTURNOFF_DCDC                                        (0x0)
#define CANA_mTURNON_DCDC                                         (0xAA)
#define CANA_mTURNON_FEC                                          (0xBB)
#define CANA_mTOTAL_PSUNODE                                                                                  (27U)

/*==============================================================================
 Enums
 ==============================================================================*/

/*==============================================================================
 Structures
 ==============================================================================*/
typedef struct CANA_zPSUDATA
{
    uint16_t uiStart_cmd;
    float32_t f32SetVolt;
    uint16_t uiVoltInt[CANA_mTOTAL_PSUNODE + 1];
    uint16_t uiVoltFrac[CANA_mTOTAL_PSUNODE + 1];
    float32 f32OutVolt[CANA_mTOTAL_PSUNODE + 1];
    uint16_t uiCurrInt[CANA_mTOTAL_PSUNODE + 1];
    uint16_t uiCurrFrac[CANA_mTOTAL_PSUNODE + 1];
    float32 f32OutCurr[CANA_mTOTAL_PSUNODE + 1];
    uint16_t uiProgVoltInt[CANA_mTOTAL_PSUNODE + 1];
    uint16_t uiProgVoltFrac[CANA_mTOTAL_PSUNODE + 1];
    float32 f32ProgVolt[CANA_mTOTAL_PSUNODE + 1];
    uint16_t uiProgCurrInt[CANA_mTOTAL_PSUNODE + 1];
    uint16_t uiProgCurrFrac[CANA_mTOTAL_PSUNODE + 1];
    float32_t f32ProgCurr[CANA_mTOTAL_PSUNODE + 1];
    float32_t ACparamFreq[26];
    float32 AClineVolt[26];
    float32 AClineCurr[26];

} CANA_tzPSUDATA;

typedef struct CAN_zRXDRegs
{
    CANA_tzPSUDATA tzPSUData;

} CANA_tzRXDRegs;

typedef struct CANA_zPSUTXDATA
{
    float32_t VoltageSet;
    float32_t CurrentSet;
    float32_t TotalCurrentSet;
    float32_t TotalISetTemp;

} CANA_tzPSUTXDATA;

typedef struct CANA_zTXDRegs
{
    CANA_tzPSUTXDATA tzPSUData;

} CANA_tzTXDRegs;

typedef struct CANA_zPSURegs
{

    uint16_t MsgID1;
    uint16_t MsgID2;
    uint16_t MsgID3;
    uint16_t MsgID4;

    uint16_t MBox5;
    uint16_t MBox6;
    uint16_t MBox7;
    uint16_t MBox8;

    uint16_t Node;
    uint16_t HiNode;

} CANA_tzPSURegs;

// ENUMS
typedef enum
{
    IDLE_PSU,
    SET_VOLT,
    SET_CURR,
    QUERY_PROGPARAM,
    TURN_ON,
    TURN_ONFEC,
    TURN_OFF,
    QUERY_OP_PARAM,
    QUERY_FLTS,
    QUERY_ACPARAMS,
    SAVE_SETTINGS,
    IMPEDANCE_MODE,
    LOCAL_REMOTE

} CANA_tzPSUQUERY;

typedef struct
{
    CANA_tzPSUQUERY PSU;

} CANA_tzQUERYTYPE;

// Active Node Register
struct CANA_tzACTNODE_BITS
{
    uint16_t bt_rsvd :1;        // Bit 0
    uint16_t bt_node1 :1;       // Bit 1
    uint16_t bt_node2 :1;       // Bit 2
    uint16_t bt_node3 :1;       // Bit 3
    uint16_t bt_node4 :1;       // Bit 4
    uint16_t bt_node5 :1;       // Bit 5
    uint16_t bt_node6 :1;       // Bit 6
    uint16_t bt_node7 :1;       // Bit 7
    uint16_t bt_node8 :1;       // Bit 8
    uint16_t bt_node9 :1;       // Bit 9
    uint16_t bt_node10 :1;       // Bit 10
    uint16_t bt_node11 :1;       // Bit 11
    uint16_t bt_node12 :1;       // Bit 12
    uint16_t bt_node13 :1;       // Bit 13
    uint16_t bt_node14 :1;       // Bit 14
    uint16_t bt_node15 :1;       // Bit 15
};

struct CANA_tzACTNODE1_BITS
{
    uint16_t bt_rsvd1 :1;       // Bit 16
    uint16_t bt_node16 :1;       // Bit 17
    uint16_t bt_node17 :1;       // Bit 18
    uint16_t bt_node18 :1;       // Bit 19
    uint16_t bt_node19 :1;       // Bit 20
    uint16_t bt_node20 :1;       // Bit 21
    uint16_t bt_node21 :1;       // Bit 22
    uint16_t bt_node22 :1;       // Bit 23
    uint16_t bt_node23 :1;       // Bit 24
    uint16_t bt_node24 :1;       // Bit 25
    uint16_t bt_node25 :1;       // Bit 26
    uint16_t bt_node26 :1;       // Bit 27
    uint16_t bt_node27 :1;       // Bit 28
    uint16_t bt_node28 :1;       // Bit 29
    Uint16 bt_node29 :1;       // Bit 30
    Uint16 bt_node30 :1;       // Bit 31

};

union CANA_tzACTNODE_REGS
{
    uint16_t all;
    struct CANA_tzACTNODE_BITS bit;
};

union CANA_tzACTNODE1_REGS
{
    uint16_t all;
    struct CANA_tzACTNODE1_BITS bit;
};

// Fault RegisterDCDC
struct CANA_tzFAULT_DCDCBITS
{
    uint16_t bt_AC_Fault :1;                           // Bit 0
    uint16_t bt_Otp_Fault :1;                          // Bit 1
    uint16_t bt_Fold_Fault :1;                         // Bit 2
    uint16_t bt_Ovp_Fault :1;                          // Bit 3
    uint16_t bt_ShutOff_Fault :1;                      // Bit 4
    uint16_t bt_Out_Fault :1;                          // Bit 5
    uint16_t bt_Interlock_Fault :1;                    // Bit 6
    uint16_t bt_Fan_Fault :1;                          // Bit 7
    uint16_t bt_Reserved :1;                           // Bit 8
    uint16_t bt_PFC_DC_Bus_Fault :1;                   // Bit 9
    uint16_t bt_Internal_Communication_Fault :1;       // Bit 10   I2C, SPI
    uint16_t bt_Eeprom_Checksum_Failure :1;            // Bit 11
    uint16_t bt_On_Board_Adc_Failure :1;               // Bit 12
    uint16_t bt_Illegal_Math_Performed :1;             // Bit 13
    uint16_t bt_Reserved_2 :1;                         // Bit 14
    uint16_t bt_Reserved_3 :1;                         // Bit 15
};

union CANA_tzFAULT_DCDCREGS
{
    Uint16 all;
    struct CANA_tzFAULT_DCDCBITS bit;
};

/**
 * @brief structure to store fault register-PFC
 */
struct CANA_tzFAULT_FECHBITS
{
    uint32_t dc_bus_overcurrent :1;  // bit8
    uint32_t grid_current_violation :1; // bit9
    uint32_t inst_grid_rstcurrent_violation :1; // bit10
    uint32_t system_fault :1; // bit11
    uint32_t over_temp_voilation :1; // bit12
    uint32_t freq_out_of_Range :1; // bit13
    uint32_t inputphase_missing :1; // bit14
    uint32_t healthStatus :1;      // bit15
};

union CANA_tzFAULT_FECHREGS
{
    Uint16 all;
    struct CANA_tzFAULT_FECHBITS bit;
};

struct CANA_tzFAULT_FECLBITS
{
    uint32_t grid_ac_undervolt :1;    // bit0
    uint32_t hardware_fault :1;       // bit1
    uint32_t grid_ac_overvolt :1;     // bit2
    uint32_t pll_nosync :1;  // bit3
    uint32_t dc_bus_overvolt :1;      // bit4
    uint32_t dc_bus_rstovervolt :1;   // bit5
    uint32_t dc_bus_undervolt :1;     // bit6
    uint32_t dc_bus_undervolt1 :1;   // bit7
};

union CANA_tzFAULT_FECLREGS
{
    Uint16 all;
    struct CANA_tzFAULT_FECLBITS bit;
};

typedef struct CANA_zPSUTIMER
{
    uint16_t secondCnt;
    uint16_t TxCount;
    uint16_t TxManCount;

    uint16_t TxCntPSUCmds;
    uint16_t RxCntPSUCmds;
    uint16_t CANfailCnt[CANA_mTOTAL_PSUNODE + 1];
    uint16_t CurRampUpcnt;
    uint16_t CurRampDowncnt;
    uint16_t InstShutDowncnt;
    uint16_t SiteCommandRUT_ACK;
    uint16_t ComsnCnt;


} CANA_tzPSUTIMER;

typedef struct CANA_zTIMERRegs
{
    CANA_tzPSUTIMER tzPSU;

} CANA_tzTIMERRegs;

/*============================================================================
 Macros
 ==============================================================================*/

/*==============================================================================
 Extern/Public Function Prototypes
 ==============================================================================*/

extern void CANA_fnPSUTX_Event(void);
extern void CANA_fnPSURX_Event(void);
extern void cana_fnTurnON_PSU(int32 i32nodeID, int32 i32ON_command,
                              int32 i32global);
extern void cana_fnQryProgParam_PSU(int32 i32nodeID);
extern void cana_fnQryOutputParam_PSU(int32 i32nodeID);
extern void cana_fnQryFaultsParam_PSU(int32 i32nodeID);
extern void cana_fnQryACParam_PSU(int32 i32nodeID);
extern void cana_fnSendImpCmds(int32 i32nodeID);

extern void cana_fnSetVoltage_PSU(int32 i32nodeID, float32 f32voltageValue,
                                  Uint16 ui16global);

extern void cana_fnSetCurrent_PSU(int32 i32nodeID, float32 f32CurrentValue,
                           Uint16 ui16global);
extern void cana_fnReadMBox_PSU(void);
extern void cana_fnNodeCheckLogic_PSU(void);
extern void cana_fnTimeoutFaultsLogic_PSU(void);
/*==============================================================================
 Extern/Public Variables
 ==============================================================================*/

// PSU related Structures
extern CANA_tzRXDRegs CANA_tzRxdRegs;
extern CANA_tzTXDRegs CANA_tzTxdRegs;
extern CANA_tzTIMERRegs CANA_tzTimerRegs;
extern CANA_tzQUERYTYPE CANA_tzQueryType;
extern CANA_tzPSURegs CANA_PSURegs;

extern union CANA_tzACTNODE_REGS CANA_tzActNodeRegs_PSU;
extern union CANA_tzACTNODE1_REGS CANA_tzActNode1Regs_PSU;
extern union CANA_tzFAULT_DCDCREGS CANA_tzDCDCFaultRegs[29];
extern union CANA_tzFAULT_FECHREGS CANA_tzFECHFaultRegs[29];
extern union CANA_tzFAULT_FECLREGS CANA_tzFECLFaultRegs[29];

// PSU related Variables
extern uint16_t ui16txMsgDataPSU[8];
extern uint16_t ui16RxMsgDataPSU[8];
extern uint16_t ui16Rx1MsgDataPSU[8];
extern uint16_t ui16Rx2MsgDataPSU[8];
extern uint16_t ui16Rx3MsgDataPSU[8];
extern uint16_t ui16CANAPSUFailTrig1;
extern uint32_t ui32CANAPSUFailCnt1;
extern uint16_t PSUCommand,ui16manualTesting;

/*==============================================================================
 Extern/Public Constants
 ==============================================================================*/

#endif /* MODULE_APPLICATION_CANA_PSUCOM_C_INC_CANA_PSUCOM_H_ */
