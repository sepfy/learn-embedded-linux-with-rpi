#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/device.h>

#define BUTTON 8

struct smart_button_t {
	int gpio;
	short int irq;
	struct input_dev *input_dev;
	u8 keybit;
};

static struct smart_button_t smart_button = {-1, -1, NULL, 0};

static irqreturn_t smart_button_isr(int irq, void *data)
{
	int state = 0;
	state = gpio_get_value(smart_button.gpio);

	input_event(smart_button.input_dev, EV_KEY, smart_button.keybit, state);
	input_sync(smart_button.input_dev);
	return IRQ_HANDLED;
}


static int __init smart_button_init(void)
{
	int ret;

	if(!gpio_is_valid(smart_button.gpio)) {
		printk("GPIO in invalid\n");
		return -EINVAL;
	}

	if(gpio_request(smart_button.gpio, "smart-button") < 0) {
		printk("Request GPIO failed\n");
		return -EINVAL;
	}

	smart_button.irq = gpio_to_irq(smart_button.gpio);
	if(request_irq(smart_button.irq, smart_button_isr ,IRQF_TRIGGER_FALLING | IRQF_TRIGGER_RISING | IRQF_ONESHOT, "smart-button", NULL)) {
		printk("Request IRQ failed\n");
		return -EINVAL;
	}

	smart_button.input_dev = input_allocate_device();
	if(smart_button.input_dev == NULL) {
		return -ENOMEM;
	}

	smart_button.input_dev->name = "smart-button1";
	smart_button.keybit = 0x47;
	smart_button.gpio = BUTTON;

	set_bit(EV_KEY, smart_button.input_dev->evbit);
	set_bit(smart_button.keybit, smart_button.input_dev->keybit);

	ret = input_register_device(smart_button.input_dev);

	if(ret) {
		printk("Failed to register %s\n", smart_button.input_dev->name);
		return ret;
	}

	return ret;
}

static void __exit smart_button_exit(void)
{
	input_unregister_device(smart_button.input_dev);
	input_free_device(smart_button.input_dev);
	free_irq(smart_button.irq, NULL);
	gpio_free(smart_button.gpio);
}

module_init(smart_button_init);
module_exit(smart_button_exit);

MODULE_AUTHOR("Sepfy");
MODULE_LICENSE("GPL");
