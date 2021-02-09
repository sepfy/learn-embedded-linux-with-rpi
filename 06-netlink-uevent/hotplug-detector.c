#include <linux/init.h>
#include <linux/module.h>
#include <linux/kobject.h>

struct hotplug_dev_t {
	struct kobject *hotplug_kobj;
	struct kset *hotplug_kset;
};

static struct hotplug_dev_t hotplug_dev;

int hotplug_dev_init(void)
{

	hotplug_dev.hotplug_kobj = kobject_create_and_add("hotplug_kobj",NULL);
	hotplug_dev.hotplug_kset = kset_create_and_add("hotplug_kset", NULL, hotplug_dev.hotplug_kobj);
	kobject_uevent(hotplug_dev.hotplug_kobj, KOBJ_ADD);
	return 0;
}


void hotplug_dev_exit(void)
{
	kset_unregister(hotplug_dev.hotplug_kset);
	kobject_del(hotplug_dev.hotplug_kobj);
}

module_init(hotplug_dev_init);
module_exit(hotplug_dev_exit);

MODULE_AUTHOR("Sepfy");
MODULE_LICENSE("GPL");
