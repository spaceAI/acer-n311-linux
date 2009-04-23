/* 
 * Copyright 2009 Vasily Khoruzhick <anarsoul at gmail dot com>
 *
 * LCD and BL power control driver for HP iPAQ RX1950
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/delay.h>
#include <linux/gpio.h>

#include <mach/hardware.h>
#include <mach/regs-gpio.h>
#include <mach/regs-gpioj.h>
#define MEMSTART 0x20000000
#define MEMOFFS 0x00
#define MEMSZ 0xb20

#undef LCD_DEBUG
#ifdef LCD_DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...)
#endif

static struct work_struct lock_work;

static struct platform_device rx1950_lcdpower = {
	.name = "n311-lcdpower",
	.id =  -1,
};

struct rx1950_lcdpower_data {
	struct platform_device	*pdev;
	struct platform_device	*owner;
	unsigned int		period;
	unsigned int		lcd_enabled;
	unsigned int		bl_enabled;
	unsigned int 		lcd_enabled_old;
	unsigned int 		bl_enabled_old;
};

int rx1950_lcdpower_lcd_enable(int enable)
{
 void __iomem *rom;
 rom=ioremap(MEMSTART+MEMOFFS, MEMSZ);
 if(!rom)
 {
  printk("n311nv: ioremap failed\n");
  goto err2;
 }
 DBG("n311nv: rom=%p\n",rom);
	if (enable) {
		__raw_writel(0x04700202, rom+0xb04);
		__raw_writel(0x54712bbf, rom+0xb0c);
		__raw_writel(0x03400000, rom+0xb10);
		__raw_writel(0x55555555, rom+0xb14);
		__raw_writel(0x28051005, rom+0xb18);
		__raw_writel(0x4047, rom+0xa00);
		DBG("%s: enabling lcd power\n", __func__);
	}
	else {
		__raw_writel(0x00700202, rom+0xb04);
		__raw_writel(0x50712bbf, rom+0xb0c);
		__raw_writel(0x1343ffff, rom+0xb10);
		__raw_writel(0xffffffff, rom+0xb14);
		__raw_writel(0x280d1005, rom+0xb18);
		__raw_writel(0x4044, rom+0xa00);
		DBG("%s: disabling lcd power\n", __func__);
	}
 iounmap(rom);
err2:
	return 0;	
}

EXPORT_SYMBOL_GPL(rx1950_lcdpower_lcd_enable);

int rx1950_lcdpower_bl_enable(int enable)
{
	s3c2410_gpio_setpin(S3C2410_GPB0, 0);
	s3c2410_gpio_pullup(S3C2410_GPB0, 0);

	s3c2410_gpio_cfgpin(S3C2410_GPB0,
			(enable) ? S3C2410_GPB0_TOUT0 : S3C2410_GPB0_OUTP);
	return 0;

}

EXPORT_SYMBOL_GPL(rx1950_lcdpower_bl_enable);

static void n311_lock_work(struct work_struct *work)
{
        DBG("Entered %s\n", __func__);
	int lock;
	lock = !(!gpio_get_value(S3C2410_GPG1));
	DBG("LOCK = %d\n", lock);
	rx1950_lcdpower_bl_enable(lock);
	rx1950_lcdpower_lcd_enable(lock);
}

static irqreturn_t lock_enabled(int irq, void *dev_id)
{
	/* Check if jack is inserted, and report
	 * GPG8 == 1 if jack if inserted, otherwise GPG8 == 0
	 */
	//n311_ext_control(s3c24xx_snd_devdata.codec);	

        /* Does not care about result */
        schedule_work(&lock_work);
	return IRQ_HANDLED;
}
static int rx1950_lcdpower_probe(struct platform_device *pdev)
{
	struct rx1950_lcdpower_data *lcd_power;
	int ret;

	lcd_power = kzalloc(sizeof(*lcd_power), GFP_KERNEL);
	if (!lcd_power) {
		dev_err(&pdev->dev, "no memory for state\n");
		ret = -ENOMEM;
		return ret;
	}

	printk("RX1950 lcd and bl power platform driver\n");
	platform_set_drvdata(pdev, lcd_power);
	if (request_irq(IRQ_EINT9, lock_enabled,
			IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
			"n311-lcdpower", pdev)) {
		DBG(KERN_ERR "n311_lcdpower: can't get irq.\n");
		/* FIXME: fix memory leak here! */
		return -ENOENT;
	}
        INIT_WORK(&lock_work, n311_lock_work);
	rx1950_lcdpower_lcd_enable(1);
	rx1950_lcdpower_bl_enable(1);

	return 0;
}

static int rx1950_lcdpower_remove(struct platform_device *pdev)
{
	struct rx1950_lcdpower_data *lcd_power = platform_get_drvdata(pdev);

	rx1950_lcdpower_lcd_enable(0);
	rx1950_lcdpower_bl_enable(0);
	kfree(lcd_power);
	return 0;
}

#define rx1950_lcdpower_suspend	NULL
#define rx1950_lcdpower_resume	NULL

static struct platform_driver rx1950_lcdpower_driver = {
	.driver		= {
		.name	= "n311-lcdpower",
		.owner	= THIS_MODULE,
	},
	.probe		= rx1950_lcdpower_probe,
	.remove		= rx1950_lcdpower_remove,
	.suspend	= rx1950_lcdpower_suspend,
	.resume		= rx1950_lcdpower_resume,
};

static int __init rx1950_lcdpower_init(void)
{
	return platform_driver_register(&rx1950_lcdpower_driver);
}
module_init(rx1950_lcdpower_init);

static void __exit rx1950_lcdpower_exit(void)
{
	platform_driver_unregister(&rx1950_lcdpower_driver);
}
module_exit(rx1950_lcdpower_exit);

MODULE_DESCRIPTION("LCD and BL power control driver for RX1950");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Vasily Khoruzhick <anarsoul at gmail dot com>");

