#ifndef __SBC_EC9100_CONFIG_H__
#define __SBC_EC9100_CONFIG_H__

//#define CONFIG_MXC_UART_BASE UART3_BASE
#define CFG_MXC_UART_BASE UART3_BASE

#include <asm/arch-mx6/imx-regs.h>
#include "mx6_common.h"
#include <asm/mach-imx/gpio.h>

#define PHYS_SDRAM                 MMDC0_ARB_BASE_ADDR
#define CFG_SYS_SDRAM_BASE      PHYS_SDRAM
#define CFG_SYS_INIT_RAM_ADDR   IRAM_BASE_ADDR
#define CFG_SYS_INIT_RAM_SIZE   IRAM_SIZE
#define CFG_SYS_INIT_SP_OFFSET \
	(CFG_SYS_INIT_RAM_SIZE - GENERATED_GBL_DATA_SIZE)
#define CFG_SYS_INIT_SP_ADDR \
	(CFG_SYS_INIT_RAM_ADDR + CONFIG_SYS_INIT_SP_OFFSET)

#define CFG_SYS_FSL_ESDHC_ADDR       USDHC1_BASE_ADDR

#endif
