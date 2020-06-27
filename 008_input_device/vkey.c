#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>

static struct input_dev *vkey;

static ssize_t store(struct kobject *kobj,
			struct kobj_attribute *attr,
			const char *buf, size_t n)
{
    while (1) {
        if (strcmp(attr->attr.name, "up")) {
            input_report_key(vkey, BTN_0, 0);
            input_sync(vkey);
            break;
        }
        if (strcmp(attr->attr.name, "down")) {
            input_report_key(vkey, BTN_0, 1);
            input_sync(vkey);
            break;
        }
        if (strcmp(attr->attr.name, "click")) {
            input_report_key(vkey, BTN_0, 1);
            input_sync(vkey);
            mdelay(5);
            input_report_key(vkey, BTN_0, 0);
            input_sync(vkey);
            break;
        }
    }

    return n;
}

static ssize_t show(struct kobject *kobj,
			struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "echo anything to me\n");
}

static struct kobject *kobj;
static struct kobj_attribute down_attribute =
	__ATTR(down, 0664, show, store);
static struct kobj_attribute up_attribute =
	__ATTR(up, 0664, show, store);
static struct kobj_attribute click_attribute =
	__ATTR(click, 0664, show, store);
static struct attribute *attrs[] = 
{
	&down_attribute.attr,
	&up_attribute.attr,
	&click_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};
static struct attribute_group attr_group = 
{
	.attrs = attrs,
};

static int __init vkey_init(void)
{
    int ret;

    kobj = kobject_create_and_add("vkey", NULL); // /sys/mysysfs
    if (!kobj) {
        return -ENOMEM;
    }

    ret = sysfs_create_group(kobj, &attr_group);
	if (ret) {
		kobject_put(kobj);
        return ret;
    }

    vkey = input_allocate_device();
    if (!vkey) {
        printk(KERN_ERR "button.c: Not enough memory\n");
        return -ENOMEM;
    }

    vkey->name = "vkey"; // to avoid unspecified device warning
    vkey->evbit[0] = BIT_MASK(EV_KEY);
    vkey->keybit[BIT_WORD(BTN_0)] = BIT_MASK(BTN_0);

    ret = input_register_device(vkey);
    if (ret) {
        printk(KERN_ERR "button.c: Failed to register device\n");
        kobject_put(kobj);
        return ret;
    }

    return 0;
}

static void __exit vkey_exit(void)
{
    kobject_put(kobj);
    input_unregister_device(vkey);
}

module_init(vkey_init);
module_exit(vkey_exit);
MODULE_LICENSE("Dual BSD/GPL");
