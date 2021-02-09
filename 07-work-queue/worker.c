#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/device.h>

static int __init worker_init(void)
{
	return 0;
}

static void __exit worker_exit(void)
{
}

module_init(worker_init);
module_exit(worker_exit);

MODULE_AUTHOR("Sepfy");
MODULE_LICENSE("GPL");
