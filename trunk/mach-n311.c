/* linux/arch/arm/mach-s3c2410/mach-n311.c
 *
 * Copyright (c) 2008 blondquirk <blondquirk@gmail.com>
 * Copyright (c) 2008 polachok
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
*/

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/list.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/serial_core.h>
#include <linux/platform_device.h>
#include <linux/sysdev.h>

#include <asm/mach/arch.h>
#include <asm/mach/map.h>
#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/mach-types.h>

#include <asm/arch/regs-lcd.h>
#include <asm/arch/regs-irq.h>
#include <asm/arch/regs-gpio.h>
#include <asm/arch/fb.h>
#include <asm/arch/lcd.h>
#include <asm/arch/ts.h>
#include <asm/arch/mci.h>

#include <asm/plat-s3c/regs-serial.h>
#include <asm/plat-s3c/regs-timer.h>
#include <asm/plat-s3c/nand.h>
#include <asm/plat-s3c24xx/clock.h>
#include <asm/plat-s3c24xx/devs.h>
#include <asm/plat-s3c24xx/cpu.h>
#include <asm/plat-s3c24xx/irq.h>
#include <asm/plat-s3c24xx/pm.h>

#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/mtd/mtd.h>
#include <linux/mmc/mmc.h>
#include <linux/mmc/host.h>

#include <linux/gpio_keys.h>
#include <linux/input.h>

#include <linux/leds.h>

static struct map_desc n311_iodesc[] __initdata = {
	/* nothing */
};

#define UCON S3C2410_UCON_DEFAULT | S3C2410_UCON_UCLK
#define ULCON S3C2410_LCON_CS8 | S3C2410_LCON_PNONE | S3C2410_LCON_STOPB
#define UFCON S3C2410_UFCON_RXTRIG8 | S3C2410_UFCON_FIFOMODE

static struct s3c24xx_uart_clksrc n311_serial_clocks[] = {
	[0] = {
		.name   = "fclk",
		.divisor  = 9,
		.min_baud = 0,
		.max_baud = 0,
	},
};

static struct s3c2410_uartcfg n311_uartcfgs[] = {
	[0] = {
		.hwport	     = 0,
		.flags	     = 0,
		.ucon	     = 0x9fc5,
		.ulcon	     = 0x2b,
		.ufcon	     = 0xc1,
		.clocks	     = n311_serial_clocks,
		.clocks_size = ARRAY_SIZE(n311_serial_clocks),
	},
	[1] = {
		.hwport	     = 1,
		.flags	     = 0,
		.ucon	     = 0x1c5,
		.ulcon	     = 0x03,
		.ufcon	     = 0x31,
	},
	[2] = {
		.hwport	     = 2,
		.flags	     = 0,
		.ucon	     = 0x80c5,
		.ulcon	     = 0x03,
		.ufcon	     = 0x31,
	}
};

static struct mtd_partition n311_nand_part[] = {
	[0] = {
		.name		= "Whole Flash",
		.offset		= 0,
		.size		= MTDPART_SIZ_FULL,
		.mask_flags	= MTD_WRITEABLE,
	}
};

static struct s3c2410_nand_set n311_nand_sets[] = {
	[0] = {
		.name		= "Internal",
		.nr_chips	= 1,
		.nr_partitions	= ARRAY_SIZE(n311_nand_part),
		.partitions	= n311_nand_part,
	},
};

static struct s3c2410_platform_nand n311_nand_info = {
	.tacls		= 20,
	.twrph0		= 60,
	.twrph1		= 20,
	.nr_sets	= ARRAY_SIZE(n311_nand_sets),
	.sets		= n311_nand_sets,
};

static struct s3c2410_ts_mach_info n311_ts_cfg = {
	.delay = 40000,
	.presc = 32,
	.oversampling_shift = 3,
};

static void s3c2410_mmc_def_setpower(unsigned int to)
{
	s3c2410_gpio_cfgpin(S3C2410_GPA17, S3C2410_GPIO_OUTPUT);
	s3c2410_gpio_setpin(S3C2410_GPA17, to);
}

static struct s3c24xx_mci_pdata n311_mmc_cfg = {
	.gpio_detect  = S3C2410_GPF1,
	.set_power  = s3c2410_mmc_def_setpower,
	.ocr_avail  = MMC_VDD_32_33,
};

//Using the keys to restart the machine
static struct gpio_keys_button n311_buttons[] = {
 {KEY_DELETE,   S3C2410_GPF0, 1, "Power button"},
 {KEY_RESTART, S3C2410_GPF2, 0, "Reset button"},
 //{KEY_COFFEE, S3C2410_GPG1, 1, "Key lock"}, // it's a switch
 {KEY_LEFTALT, S3C2410_GPF4, 1, "Home button"},
 {KEY_LEFTCTRL, S3C2410_GPF5, 1, "Calendar button"},
 {KEY_C, S3C2410_GPF6, 1, "Contacts button"},
 {KEY_D, S3C2410_GPF7, 1, "Mail button"},
 {KEY_ENTER, S3C2410_GPG3, 1, "Ok button"},
 {KEY_LEFT, S3C2410_GPG4, 1, "Left button"},
 {KEY_DOWN, S3C2410_GPG5, 1, "Down button"},
 {KEY_UP, S3C2410_GPG6, 1, "Up button"},
 {KEY_RIGHT, S3C2410_GPG7, 1, "Right button"},
 //{KEY_PAUSE, S3C2410_GPD1, 0, "Case open pin"}, // it's a switch
};


static struct gpio_keys_platform_data n311_button_data = {
	.buttons	= n311_buttons,
	.nbuttons	= ARRAY_SIZE(n311_buttons),
};

static struct platform_device n311_button_device = {
	.name		= "gpio-keys",
	.id		= -1,
	.num_resources	= 0,
	.dev		= {.platform_data = &n311_button_data,}
};

static struct gpio_led n311_leds[] = {
	{
		.name = "blue-around",
		.gpio = S3C2410_GPD10,
		.active_low = 0,
		.default_trigger = "heartbeat",
	},
};

static struct gpio_led_platform_data n311_led_data = {
	.num_leds =     ARRAY_SIZE(n311_leds),
	.leds =         n311_leds,
};

static struct platform_device n311_gpio_leds = {
	.name =         "leds-gpio",
	.id =           -1,
	.dev = {
		.platform_data = &n311_led_data,
	}
};

static void n311_backlight_power(int on)
{
	s3c2410_gpio_setpin(S3C2410_GPB0, 0);
	s3c2410_gpio_pullup(S3C2410_GPB0, 0);

	s3c2410_gpio_cfgpin(S3C2410_GPB0,
			(on) ? S3C2410_GPB0_TOUT0 : S3C2410_GPB0_OUTP);
}

static void n311_lcd_power(int on)
{
	s3c2410_gpio_setpin(S3C2410_GPC0, on);
}

static void n311_set_brightness(int tcmpb0)
{
	unsigned long tcfg0;
	unsigned long tcfg1;
	unsigned long tcon;

	/* configure power on/off */
	n311_backlight_power(tcmpb0 ? 1 : 0);


	tcfg0=readl(S3C2410_TCFG0);
	tcfg1=readl(S3C2410_TCFG1);

	tcfg0 &= ~S3C2410_TCFG_PRESCALER0_MASK;
	tcfg0 |= 0x18;

	tcfg1 &= ~S3C2410_TCFG1_MUX0_MASK;
	tcfg1 |= S3C2410_TCFG1_MUX0_DIV2;

	writel(tcfg0, S3C2410_TCFG0);
	writel(tcfg1, S3C2410_TCFG1);
	writel(0x31, S3C2410_TCNTB(0));

	tcon = readl(S3C2410_TCON);
	tcon &= ~0x0F;
	tcon |= S3C2410_TCON_T0RELOAD;
	tcon |= S3C2410_TCON_T0MANUALUPD;

	writel(tcon, S3C2410_TCON);
	writel(0x31, S3C2410_TCNTB(0));
	writel(tcmpb0, S3C2410_TCMPB(0));

	/* start the timer running */
	tcon |= S3C2410_TCON_T0START;
	tcon &= ~S3C2410_TCON_T0MANUALUPD;
	writel(tcon, S3C2410_TCON);
}

static struct s3c2410_bl_mach_info n311_bl_cfg = {

	.backlight_max          = 0x2c,
	.backlight_default      = 0x16,
	.backlight_power	= n311_backlight_power,
	.set_brightness		= n311_set_brightness,
	.lcd_power		= n311_lcd_power
};

static struct platform_device *n311_devices[] __initdata = {
	&s3c_device_rtc,
	&s3c_device_usb,
	&s3c_device_lcd,
	&s3c_device_wdt,
	&s3c_device_i2c,
	&s3c_device_iis,
	&s3c_device_nand,
	&s3c_device_usbgadget,
	&s3c_device_ts,
	&s3c_device_sdi,
	&s3c_device_bl,
};

static void __init n311_map_io(void)
{
	s3c_device_nand.dev.platform_data = &n311_nand_info;
	s3c24xx_init_io(n311_iodesc, ARRAY_SIZE(n311_iodesc));
	s3c24xx_init_clocks(16934000);
	s3c24xx_init_uarts(n311_uartcfgs, ARRAY_SIZE(n311_uartcfgs));
}

static void __init n311_init_machine(void)
{
	platform_add_devices(n311_devices, ARRAY_SIZE(n311_devices));
	set_s3c2410ts_info(&n311_ts_cfg);
	s3c_device_sdi.dev.platform_data = &n311_mmc_cfg;
 	platform_device_register(&n311_button_device);
	platform_device_register(&n311_gpio_leds);
	set_s3c2410bl_info(&n311_bl_cfg);
	s3c2410_pm_init();
	/* wake up source */
  	s3c_irq_wake(IRQ_EINT1, 1);

        /* Configure the LEDs (even if we have no LED support)*/
 	s3c2410_gpio_cfgpin(S3C2410_GPD10, S3C2410_GPD10_OUTP);
	s3c2410_gpio_setpin(S3C2410_GPD10, 1);
}


MACHINE_START(N311, "N311")
	/* Maintainer: blondquirk <blondquirk@gmail.com> */
	.phys_io	= S3C2410_PA_UART,
	.io_pg_offst	= (((u32)S3C24XX_VA_UART) >> 18) & 0xfffc,
	.boot_params	= S3C2410_SDRAM_PA + 0x100,

	.init_irq	= s3c24xx_init_irq,
	.map_io		= n311_map_io,
	.init_machine	= n311_init_machine,
	.timer		= &s3c24xx_timer,
MACHINE_END
