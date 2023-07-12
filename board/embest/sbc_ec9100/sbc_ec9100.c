#include <command.h>
#include <init.h>
#include <net.h>
#include <asm/arch/clock.h>
#include <asm/arch/iomux.h>
#include <asm/arch/imx-regs.h>
#include <asm/arch/crm_regs.h>
#include <asm/arch/mx6ul_pins.h>
#include <asm/arch/mx6-pins.h>
#include <asm/arch/sys_proto.h>
#include <asm/global_data.h>
#include <asm/gpio.h>
#include <asm/mach-imx/iomux-v3.h>
#include <asm/mach-imx/boot_mode.h>
#include <asm/mach-imx/mxc_i2c.h>
#include <asm/io.h>
#include <common.h>
#include <env.h>
#include <fsl_esdhc_imx.h>
#include <i2c.h>
#include <miiphy.h>
#include <linux/delay.h>
#include <linux/sizes.h>
#include <mmc.h>
#include <netdev.h>
#include <power/pmic.h>
#include <power/pfuze3000_pmic.h>
#include "../../freescale/common/pfuze.h"
#include <usb.h>
#include <usb/ehci-ci.h>

// TODO... set this from a config...
#define CONFIG_BOOT_FROM_SD

DECLARE_GLOBAL_DATA_PTR;

#define UART_PAD_CTRL  (PAD_CTL_PKE | PAD_CTL_PUE |             \
	PAD_CTL_PUS_100K_UP | PAD_CTL_SPEED_MED |               \
	PAD_CTL_DSE_40ohm   | PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define USDHC_PAD_CTRL (PAD_CTL_PKE | PAD_CTL_PUE |             \
	PAD_CTL_PUS_22K_UP  | PAD_CTL_SPEED_LOW |               \
	PAD_CTL_DSE_80ohm   | PAD_CTL_SRE_FAST  | PAD_CTL_HYS)

#define QSPI_PAD_CTRL1  \
	(PAD_CTL_SRE_FAST | PAD_CTL_SPEED_MED | \
	PAD_CTL_PKE | PAD_CTL_PUE | PAD_CTL_PUS_47K_UP | PAD_CTL_DSE_120ohm)

static iomux_v3_cfg_t const uart3_pads[] = {
	MX6_PAD_UART3_TX_DATA__UART3_DCE_TX | MUX_PAD_CTRL(UART_PAD_CTRL),
	MX6_PAD_UART3_RX_DATA__UART3_DCE_RX | MUX_PAD_CTRL(UART_PAD_CTRL),
};

static iomux_v3_cfg_t const usdhc1_pads[] = {
	MX6_PAD_SD1_CLK__USDHC1_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_CMD__USDHC1_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DATA0__USDHC1_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DATA1__USDHC1_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DATA2__USDHC1_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_SD1_DATA3__USDHC1_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),

	/* CD */
	MX6_PAD_SNVS_TAMPER1__GPIO5_IO01 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static iomux_v3_cfg_t const usdhc2_pads[] = {
	MX6_PAD_NAND_RE_B__USDHC2_CLK | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_WE_B__USDHC2_CMD | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA00__USDHC2_DATA0 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA01__USDHC2_DATA1 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA02__USDHC2_DATA2 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA03__USDHC2_DATA3 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA04__USDHC2_DATA4 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA05__USDHC2_DATA5 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA06__USDHC2_DATA6 | MUX_PAD_CTRL(USDHC_PAD_CTRL),
	MX6_PAD_NAND_DATA07__USDHC2_DATA7 | MUX_PAD_CTRL(USDHC_PAD_CTRL),

	/*
	 * RST_B
	 */
	MX6_PAD_NAND_ALE__GPIO4_IO10 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

static iomux_v3_cfg_t const quadspi_pads[] = {
	MX6_PAD_NAND_WP_B__QSPI_A_SCLK  | MUX_PAD_CTRL(QSPI_PAD_CTRL1),
	MX6_PAD_NAND_READY_B__QSPI_A_DATA00     | MUX_PAD_CTRL(QSPI_PAD_CTRL1),
	MX6_PAD_NAND_CE0_B__QSPI_A_DATA01       | MUX_PAD_CTRL(QSPI_PAD_CTRL1),
	MX6_PAD_NAND_CE1_B__QSPI_A_DATA02       | MUX_PAD_CTRL(QSPI_PAD_CTRL1),
	MX6_PAD_NAND_CLE__QSPI_A_DATA03 | MUX_PAD_CTRL(QSPI_PAD_CTRL1),
	MX6_PAD_NAND_DQS__QSPI_A_SS0_B  | MUX_PAD_CTRL(QSPI_PAD_CTRL1),
};

#define USDHC1_CD_GPIO  IMX_GPIO_NR(5, 1)
#define USDHC2_PWR_GPIO IMX_GPIO_NR(4, 10)

static struct fsl_esdhc_cfg usdhc_cfg[2] = {
        {USDHC1_BASE_ADDR, 0, 4},
        {USDHC2_BASE_ADDR, 0, 8},
};

static iomux_v3_cfg_t const iox_pads[] = {
	/* IOX_SDI */
	MX6_PAD_BOOT_MODE0__GPIO5_IO10 | MUX_PAD_CTRL(NO_PAD_CTRL),
	/* IOX_SHCP */
	MX6_PAD_BOOT_MODE1__GPIO5_IO11 | MUX_PAD_CTRL(NO_PAD_CTRL),
	/* IOX_STCP */
	MX6_PAD_SNVS_TAMPER7__GPIO5_IO07 | MUX_PAD_CTRL(NO_PAD_CTRL),
	/* IOX_nOE */
	MX6_PAD_SNVS_TAMPER8__GPIO5_IO08 | MUX_PAD_CTRL(NO_PAD_CTRL),
};

#define IOX_SDI IMX_GPIO_NR(5, 10)
#define IOX_STCP IMX_GPIO_NR(5, 7)
#define IOX_SHCP IMX_GPIO_NR(5, 11)
#define IOX_OE IMX_GPIO_NR(5, 18)

enum qn_func {
	qn_reset,
	qn_enable,
	qn_disable,
};

enum qn_level {
	qn_low = 0,
	qn_high = 1,
};

static enum qn_level seq[3][2] = {
	{0, 1}, {1, 1}, {0, 0}
};

static enum qn_func qn_output[8] = {
	qn_reset, qn_reset, qn_reset, qn_enable, qn_disable, qn_reset, qn_disable,
	qn_enable
};

static void
setup_iomux_uart(void)
{
	imx_iomux_v3_setup_multiple_pads(uart3_pads, ARRAY_SIZE(uart3_pads));
}

static void
iox74lv_init(void)
{
	int i;

	gpio_direction_output(IOX_OE, 0);

	for (i = 7; i >= 0; i--) {
		gpio_direction_output(IOX_SHCP, 0);
		gpio_direction_output(IOX_SDI, seq[qn_output[i]][0]);
		udelay(500);
		gpio_direction_output(IOX_SHCP, 1);
		udelay(500);
	}

	gpio_direction_output(IOX_STCP, 0);
	udelay(500);
	/*
	 * shift register will be output to pins
	 */
	gpio_direction_output(IOX_STCP, 1);

	for (i = 7; i >= 0; i--) {
		gpio_direction_output(IOX_SHCP, 0);
		gpio_direction_output(IOX_SDI, seq[qn_output[i]][1]);
		udelay(500);
		gpio_direction_output(IOX_SHCP, 1);
		udelay(500);
	}
        gpio_direction_output(IOX_STCP, 0);
	udelay(500);
	/*
	 * shift register will be output to pins
	 */
	gpio_direction_output(IOX_STCP, 1);

	gpio_direction_output(IOX_OE, 1);
};

int
dram_init(void)
{
	gd->ram_size = imx_ddr_size();
	return 0;
}

static int board_qspi_init(void)
{
	imx_iomux_v3_setup_multiple_pads(quadspi_pads, ARRAY_SIZE(quadspi_pads));
	enable_qspi_clk(0);

	return 0;
}

int
board_early_init_f(void)
{
	setup_iomux_uart();

	return 0;
}

int
board_mmc_init(struct bd_info *bis)
{
	int ret;

	imx_iomux_v3_setup_multiple_pads(usdhc1_pads, ARRAY_SIZE(usdhc1_pads));
	gpio_direction_input(USDHC1_CD_GPIO);
	usdhc_cfg[0].sdhc_clk = mxc_get_clock(MXC_ESDHC_CLK);
	ret = fsl_esdhc_initialize(bis, &usdhc_cfg[0]);
	if (ret)
		return ret;

	imx_iomux_v3_setup_multiple_pads(usdhc2_pads, ARRAY_SIZE(usdhc2_pads));
	gpio_direction_output(USDHC2_PWR_GPIO, 0);
	udelay(500);
	gpio_direction_output(USDHC2_PWR_GPIO, 1);
	usdhc_cfg[1].sdhc_clk = mxc_get_clock(MXC_ESDHC2_CLK);
	ret = fsl_esdhc_initialize(bis, &usdhc_cfg[1]);

	return ret;
}

static int
mmc_get_env_devno(void)
{
#if defined(CONFIG_BOOT_FROM_EMMC)
	return 1;
#elif defined(CONFIG_BOOT_FROM_SD)
	return 0;
#else
	uint32_t soc_sbmr;
	int dev_no;
	uint32_t bootsel;

	printf("Log: detect boot dev from SRC_SBMR register\n");

	soc_sbmr = readl(SRC_BASE_ADDR + 0x4);
	bootsel = (soc_sbmr & 0x000000FF) >> 6;

	/* Not SD/MMC, use default */
	if (bootsel != 1)
		return CONFIG_SYS_MMC_ENV_DEV;

	/* BOOT_CFG2[3] and BOOT_CFG2[4] */
	dev_no = (soc_sbmr & 0x00001800) >> 11;

	return dev_no;
#endif
}

int
board_mmc_getcd(struct mmc *mmc)
{
	struct fsl_esdhc_cfg *cfg = (struct fsl_esdhc_cfg *)mmc->priv;
	int ret = 0;

	switch (cfg->esdhc_base) {
		case USDHC1_BASE_ADDR:
			gpio_direction_input(USDHC1_CD_GPIO);
			ret = !gpio_get_value(USDHC1_CD_GPIO);
			break;
		case USDHC2_BASE_ADDR:
			ret = 1;
			break;
	}

	return ret;
}

int check_mmc_autodetect(void)
{
	char *autodetect_str = env_get("mmcautodetect");

	if ((autodetect_str != NULL) &&
	    (strcmp(autodetect_str, "yes") == 0)) {
		return 1;
	}

	return 0;
}

void board_late_mmc_init(void)
{
	char cmd[32];
        uint32_t dev_no = mmc_get_env_devno();

	if (!check_mmc_autodetect())
		return;

	env_set_ulong("mmcdev", dev_no);

	sprintf(cmd, "mmc dev %d", dev_no);
	run_command(cmd, 0);
}

int
board_init(void)
{
	imx_iomux_v3_setup_multiple_pads(iox_pads, ARRAY_SIZE(iox_pads));
	iox74lv_init();
	board_qspi_init();

	return 0;
}

int
board_late_init(void)
{
	return 0;
}
