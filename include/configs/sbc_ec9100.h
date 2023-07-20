#ifndef __SBC_EC9100_CONFIG_H__
#define __SBC_EC9100_CONFIG_H__

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
	(CFG_SYS_INIT_RAM_ADDR + CFG_SYS_INIT_SP_OFFSET)

#define CFG_SYS_FSL_ESDHC_ADDR       USDHC1_BASE_ADDR
#define CFG_SYS_FSL_USDHC_NUM        2

#define CFG_POWER_PFUZE3000_I2C_ADDR 0x08

#ifdef CONFIG_EC9100_BOOT_FROM_EMMC
#define EC9100_BOOT_TARGETS mmc1 mmc0
#define EC9100_MMC_DEV 1
#else
#define EC9100_BOOT_TARGETS mmc0 mmc1
#define EC9100_MMC_DEV 0
#endif

#define CFG_EXTRA_ENV_SETTINGS \
	"boot_targets=" __stringify(EC9100_BOOT_TARGETS) "\0" \
	"fdtfile=sbc_ec9100.dtb\0" \
	"fdtaddr_addr_r=0x83000000\0" \
	"fdtoverlay_addr_r=0x83100000\0" \
	"kernel_addr_r=0x80800000\n" \
	"kernel_comp_size=0x2800000\n" \
	"pxefile_addr_r=0x83200000\0" \
	"ramdisk_addr_r=0x83300000\0" \
	"scriptaddr=0x83400000\0" \
	"script_size_f=0x2000\0" \
	"image=zImage\0" \
	"console=ttymxc2\0" \
	"fdt_file=sbc_ec9100.dtb\0" \
	"fdt_addr=0x83000000\0" \
	"mmcdev=" __stringify(EC9100_MMC_DEV) "\0" \
	"mmcpart=1\0" \
	"mmcroot=/dev/mmcblk" __stringify(EC9100_MMC_DEV) "p2 rootwait rw\0" \
	"mmcargs=setenv bootargs console=${console},${baudrate} " \
		"root=${mmcroot}\0" \
	"loadaddr=0x80800000\0" \
	"loadimage=fatload mmc ${mmcdev}:${mmcpart} ${loadaddr} ${image}\0" \
	"loadfdt=fatload mmc ${mmcdev}:${mmcpart} ${fdt_addr} ${fdt_file}\0" \
	"mmcboot=echo Booting from mmc ...; " \
		"run mmcargs; " \
		"if run loadfdt; then " \
			"if run loadimage; then " \
				"bootz ${loadaddr} - ${fdt_addr}; " \
			"else " \
				"echo WARN: Cannot load the image; " \
			"fi; " \
		"fi;\0" \

#endif
