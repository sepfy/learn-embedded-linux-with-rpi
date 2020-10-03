#include <linux/module.h>   
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>

#define BUZZER 23
#define BUTTON 21

static short int button_irq = 0;
static unsigned long flags = 0;

int buzzer_trigger = 0;
u32 trigger_jiffies;

static ssize_t attr_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{

	printk("[%s][%d]\n", __func__, __LINE__);
	buzzer_trigger = simple_strtoul(buf, NULL, 10);
	gpio_set_value(BUZZER, buzzer_trigger);
	return count;
}

static ssize_t attr_show(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{
        int val = 0;
	printk("[%s][%d]\n", __func__, __LINE__);
        val = sprintf(buf, "%d\n", buzzer_trigger);
        return val;
}

static struct class *mydev_class;
static struct device *dev;

static DEVICE_ATTR(data, 0644, attr_show, attr_store);

static irqreturn_t button_isr(int irq, void *data)
{
	local_irq_save(flags);
	printk("[%s][%d]\n", __func__, __LINE__);
	if((jiffies - trigger_jiffies) > 10)
	{
		trigger_jiffies = jiffies;
		gpio_set_value(BUZZER, buzzer_trigger);
		buzzer_trigger = buzzer_trigger ? 0 : 1;
	}  
	local_irq_restore(flags);
	return IRQ_HANDLED;
}


int init_module(void)
{
	int ret = 0;
	printk("Init my dev\n");
	mydev_class = class_create(THIS_MODULE, "mydev_class");
	if(IS_ERR(mydev_class)) {
		ret = PTR_ERR(mydev_class);
		printk(KERN_ALERT "Failed to create class.\n");
		return ret;
	}

	dev = device_create(mydev_class, NULL, MKDEV(100,0), NULL, "dev");
	if(IS_ERR(dev)) {
		ret = PTR_ERR(dev);
		printk(KERN_ALERT "Failed to create device.\n");
		return ret;
	}

	ret = device_create_file(dev, &dev_attr_data);
	if(ret < 0) {
		printk(KERN_ALERT "Failed to create attribute file.\n");
		return ret;
	}

	printk("[%s][%d]\n", __func__, __LINE__);
	trigger_jiffies = jiffies;

	if(!gpio_is_valid(BUZZER))
		return -1;
	if(gpio_request(BUZZER, "BUZZER") < 0)
		return -1;

	gpio_direction_output(BUZZER, 0 );

	if(!gpio_is_valid(BUTTON))
		return -1;
	if(gpio_request(BUTTON, "BUTTON") < 0)
		return -1;

	button_irq = gpio_to_irq(BUTTON);
	if(request_irq(button_irq, button_isr ,IRQF_TRIGGER_RISING, NULL, NULL))
		return -1;

	return 0;
}


void cleanup_module(void)
{
	printk("[%s][%d]\n", __func__, __LINE__);
	device_remove_file(dev, &dev_attr_data);
	device_destroy(mydev_class, MKDEV(100, 0));
	class_destroy(mydev_class);

	gpio_set_value(BUZZER, 0);
	gpio_free(BUZZER);
	free_irq(button_irq, NULL);
	gpio_free(BUTTON);
}


MODULE_LICENSE("GPL");
