#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/i2c.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");

struct i2c_adapter *i2c_adapter;
struct i2c_client *i2c_client;

static struct i2c_board_info sht31_i2c[] __initdata = {
	{
		I2C_BOARD_INFO("sht31-i2c", 0x44),
	},
};

static const struct i2c_device_id sht31_id[] = {
	{
		.name = "sht31-i2c",
		.driver_data = 0,
	},
};

static int sht31_i2c_probe(struct i2c_client *client,
		const struct i2c_device_id *id)
{

	return 0;
}

static int sht31_i2c_remove(struct i2c_client *client)
{
	printk("[%s][%d]\n", __func__, __LINE__);
	return 0;
}

static struct i2c_driver sht31_i2c_driver = {
	.driver = {
		.name = "sht31-i2c-driver",
		.owner = THIS_MODULE,
	},
	.probe = sht31_i2c_probe,
	.remove = sht31_i2c_remove,
	.id_table = sht31_id,
};


static u8 crc(u8 *data, int size)
{
	u8 chk = 0xff;
	int i, j;
	for(i = 0; i < size; i++) {
		chk ^= data[i];
		for(j = 0; j < 8; j++) {
			if(chk & 0x80) {
				chk <<= 1;
				chk ^= 0x131;
			}
			else {
				chk <<= 1;
			}
		}
	}
	return chk;
}

static ssize_t attr_store(struct device *dev,
		struct device_attribute *attr,
		const char *buf, size_t count)
{
	char tmp[32] = {0};
	sprintf(tmp, "%s attr store\n", attr->attr.name);
	printk("attr_store\n");
	return count;
}

static ssize_t attr_show(struct device *dev,
		struct device_attribute *attr,
		char *buf)
{

        int val = 0;
	int temp, humi;

	u8 bytes[6] = {0};
	const u8 cmd[6] = {0};
	u32 s;

	printk("[%s][%d]\n", __func__, __LINE__);
	s = i2c_smbus_write_i2c_block_data(i2c_client, 0x24, 1, cmd);
	if(s < 0) {
		printk("[%s][%d]: i2c write block data failed\n", __func__, __LINE__);
		return -1;
	}

	mdelay(16);

	s = i2c_smbus_read_i2c_block_data(i2c_client, 0x00, 6, bytes);
	if(s < 0) {
		printk("[%s][%d]: i2c read block data failed\n", __func__, __LINE__);
		return -1;
	}

	temp = (bytes[0] << 8) + bytes[1];
	humi = (bytes[3] << 8) + bytes[4];

	if(bytes[2] != crc(&bytes[0], 2)) {
		temp = 0;
		printk("[%s][%d] Temperature CRC error\n", __func__, __LINE__);
	}

	if(bytes[5] != crc(&bytes[3], 2)) {
		humi = 0;
		printk("[%s][%d] Humidity CRC error\n", __func__, __LINE__);
	}

	if(strcmp(attr->attr.name, "temperature") == 0) {
		val = sprintf(buf, "%d\n", temp);
	}
	else {
		val = sprintf(buf, "%d\n", humi);
	}
        return val;

}

static struct class *sensor_class;
static struct device *sht31_sensor;

static DEVICE_ATTR(temperature, 0644, attr_show, attr_store);
static DEVICE_ATTR(humidity, 0644, attr_show, attr_store);


static struct platform_device *sht31_device;

static int sht31_probe(struct platform_device *pdev)
{
	printk("[%s][%d]\n", __func__, __LINE__);

	return 0;
}

static int sht31_remove(struct platform_device *pdev)
{
	printk("[%s][%d]\n", __func__, __LINE__);
	return 0;
}

static struct platform_driver sht31_driver = {
	.driver = {
		.name  = "sht31",
		.owner = THIS_MODULE,
	},
	.probe  = sht31_probe,
	.remove = sht31_remove,
};

static int __init sht31_init(void)
{
	int ret = 0;
	printk("[%s][%d]\n", __func__, __LINE__);

	// https://stackoverflow.com/questions/15532170/device-has-no-release-function-what-does-this-mean
	sht31_device = platform_device_alloc("sht31", 0);
	if(!sht31_device) {
		printk("[%s][%d]: Platform device alloc failed\n", __func__, __LINE__);
		return -1;
	}

	ret = platform_device_add(sht31_device);

	if(ret) {
		printk("[%s][%d]: Platform device add failed\n", __func__, __LINE__);
		return -1;
	}

	ret = platform_driver_register(&sht31_driver);
	if(ret) {
		printk("[%s][%d]: Platform driver register failed\n", __func__, __LINE__);
		return ret;
	}

	i2c_adapter = i2c_get_adapter(1);
	i2c_put_adapter(i2c_adapter);
	i2c_client = i2c_new_device(i2c_adapter, &sht31_i2c[0]);
	i2c_add_driver(&sht31_i2c_driver);

	sensor_class = class_create(THIS_MODULE, "sensor");
	if(IS_ERR(sensor_class)) {
		ret = PTR_ERR(sensor_class);
		printk("[%s][%d]: Create sensor class failed\n", __func__, __LINE__);
		return ret;
	}

	sht31_sensor = device_create(sensor_class, NULL, MKDEV(100,0), NULL, "sht31");
	if(IS_ERR(sht31_sensor)) {
		ret = PTR_ERR(sht31_sensor);
		printk("[%s][%d]: Create sht31 sensor device failed\n", __func__, __LINE__);
		return ret;
	}

	ret = device_create_file(sht31_sensor, &dev_attr_temperature);
	if(ret < 0) {
		printk("[%s][%d]: Create sht31 temperature file failed\n", __func__, __LINE__);
		return ret;
	}

	ret = device_create_file(sht31_sensor, &dev_attr_humidity);
	if(ret < 0) {
		printk("[%s][%d]: Create sht31 humidity file failed\n", __func__, __LINE__);
		return ret;
	}



	return ret;
}

static void __exit sht31_exit(void)
{
	printk("[%s][%d]\n", __func__, __LINE__);
	platform_device_unregister(sht31_device);
	platform_driver_unregister(&sht31_driver);
	i2c_unregister_device(i2c_client);
	i2c_del_driver(&sht31_i2c_driver);

	device_remove_file(sht31_sensor, &dev_attr_temperature);
	device_remove_file(sht31_sensor, &dev_attr_humidity);
	device_destroy(sensor_class, MKDEV(100, 0));
	class_destroy(sensor_class);
}

module_init(sht31_init);
module_exit(sht31_exit);

