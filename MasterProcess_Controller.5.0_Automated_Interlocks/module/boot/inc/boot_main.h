/*=============================================================================
Copyright Enarka India Pvt Ltd (EIPL) All Rights Reserved.
All trademarks are owned by Enarka India Private Limited
============================================================================ */

/*==============================================================================
 @file  boot_main.h
 @author OHMLAP0107   
 @date 03-Jan-2022

 @brief Description
==============================================================================*/

/*==============================================================================
 Includes
==============================================================================*/

#include "F2837xS_device.h"
#include "F2837xS_Examples.h"
#include <stdint.h>

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
#define BOOT_mCAN_NEW_FIRMWARE 0xAA
#define BOOT_mCAN_FLASH_ADDR                                   (0x120U)
#define BOOT_mCAN_ERROR_ADDR                                   (0x122U)
#define BOOT_mCAN_CRC32_ADDR                                    (0x128U)

#define BOOT_mCAN_FLASH_STARTED                                (0x11)
#define BOOT_mCAN_CRC_MISMATCH                                 (0xCC)

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

extern void boot_main(void);
extern bool is_can_flash_cmd_received(void);
/*==============================================================================
 Local Variables
==============================================================================*/
extern uint16_t uiFlashFlag;
/*==============================================================================
 Local Constants
==============================================================================*/

/*=============================================================================
 @brief infinite loop for the main where tasks are executed is defined here

 @param void
 @return void
 ============================================================================ */


/*==============================================================================
 End of File
==============================================================================*/
