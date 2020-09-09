#include <linux/module.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>
#include <linux/miscdevice.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include <linux/kernel.h>
#include <linux/types.h>

MODULE_LICENSE("GPL");

#define IOCTL_MISCDEV_SET 0x00
#define IOCTL_MISCDEV_GET 0x01

struct miscdev_data {
    int val;
    char data[64];
};

static int mydev_open(struct inode *inode, struct file *filp)
{
	printk("[%s][%d]\n", __func__, __LINE__);
	return 0;
}

static int
mydev_release(struct inode *inode, struct file *filp)
{
	printk("[%s][%d]\n", __func__, __LINE__);
	return 0;
}

static ssize_t
mydev_read(struct file *filp, char __user *buf, size_t count, loff_t *pos)
{
	char tmp[] = "Kernel says hello";
	printk("[%s][%d]\n", __func__, __LINE__);
	copy_to_user(buf, tmp, sizeof(tmp));
	*pos = 0;
	return 0;
}

static ssize_t
mydev_write(struct file *filp, const char __user *buf,
        size_t count, loff_t *pos)
{
	char tmp[128] = {0};
	printk("[%s][%d]\n", __func__, __LINE__);
	copy_from_user(tmp, buf, sizeof(tmp));
	printk("%s", tmp);
	return count;
}


static long mydev_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{

	struct miscdev_data data;
 
	memset(&data, 0, sizeof(data));
	switch (cmd) {
	case IOCTL_MISCDEV_SET:
		printk("[%s][%d]\n", __func__, __LINE__);
		copy_from_user(&data, (int __user *)arg, sizeof(data));
		printk("Set data: miscdev_data.val = %d, miscdev_data.data = %s\n", data.val, data.data); 
	   	break;
	case IOCTL_MISCDEV_GET:
		printk("[%s][%d]\n", __func__, __LINE__);
		sprintf(data.data, "Kernel says hi");
		data.val = 3;
		copy_to_user((int __user *)arg, &data, sizeof(data));
		break;
	default:
		break;
	}
	return 0;
}

static struct file_operations mydev_fops = {
	.owner = THIS_MODULE,
	.open = mydev_open,
	.release = mydev_release,
	.read = mydev_read,
	.write = mydev_write,
	.unlocked_ioctl = mydev_ioctl,
};

static struct miscdevice my_miscdev = {
	.minor      = 11,
	.name       = "misc_dev",
	.fops       = &mydev_fops,
};

static int __init init_modules(void)
{
	int ret;
	ret = misc_register(&my_miscdev);
	if (ret != 0) {
	printk("cannot register miscdev on minor=11 (err=%d)\n",ret);
	}

	return 0;
}

static void __exit exit_modules(void)
{
	misc_deregister(&my_miscdev);
}

module_init(init_modules);
module_exit(exit_modules);

