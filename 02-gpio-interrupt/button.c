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

static irqreturn_t button_isr(int irq, void *data)
{
	local_irq_save(flags);
	printk("[%s][%d]\n", __func__, __LINE__);
	if((jiffies - trigger_jiffies) > 10)
	{
		trigger_jiffies = jiffies;
		gpio_set_value(BUZZER, buzzer_trigger);
		buzzer_trigger = buzzer_trigger ? (0):(1);
	}  
	local_irq_restore(flags);
	return IRQ_HANDLED;
}


int init_module(void)
{

	printk("[%s][%d]\n", __func__, __LINE__);
	trigger_jiffies = jiffies;

	if(!gpio_is_valid(BUZZER))
		return -1;
	if(gpio_request(BUZZER, "BUZZER") < 0)
		return -1;

	gpio_direction_output(BUZZER, 0 );

	if(!gpio_is_valid(BUTTON))
		return -1;
	if(gpio_request(BUTTON,"BUTTON") < 0)
		return -1;

	button_irq = gpio_to_irq(BUTTON);
	if(request_irq(button_irq, button_isr ,IRQF_TRIGGER_RISING, NULL, NULL))
		return -1;

	return 0;
}


void cleanup_module(void)
{
	gpio_set_value(BUZZER, 0);
	gpio_free(BUZZER);
	free_irq(button_irq, NULL);
	gpio_free(BUTTON);
}


MODULE_LICENSE("GPL");
