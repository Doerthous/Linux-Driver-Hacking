#include <linux/input.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/workqueue.h>

static struct input_dev *vmouse_dev;
static struct workqueue_struct *wq;
static struct work_struct wk;


static void work(struct work_struct *work)
{
    input_report_key(vmouse_dev, BTN_LEFT, 1);
    input_sync(vmouse_dev);
    msleep(50);
    input_report_key(vmouse_dev, BTN_LEFT, 0);
    input_sync(vmouse_dev);
}


static ssize_t store(struct kobject *kobj,
			struct kobj_attribute *attr,
			const char *buf, size_t n)
{
    int x, y;

    while (1) {
        if (strcmp(attr->attr.name, "x") == 0) {
            if (kstrtoint(buf, 10, &x) == 0) {
                input_report_abs(vmouse_dev, ABS_X, x);
                input_sync(vmouse_dev);
                break;
            }
        }
        if (strcmp(attr->attr.name, "y") == 0) {
            if (kstrtoint(buf, 10, &y) == 0) {
                input_report_abs(vmouse_dev, ABS_Y, y);
                input_sync(vmouse_dev);
                break;
            }
        }
        if (strcmp(attr->attr.name, "click") == 0) {
            if (!queue_work(wq, &wk)) {
                printk(KERN_ERR "work was already on a queue.\n");
            }
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
static struct kobj_attribute x_attribute =
	__ATTR(x, 0664, show, store);
static struct kobj_attribute y_attribute =
	__ATTR(y, 0664, show, store);
static struct kobj_attribute click_attribute =
	__ATTR(click, 0664, show, store);
static struct attribute *attrs[] = 
{
	&x_attribute.attr,
	&y_attribute.attr,
    &click_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};
static struct attribute_group attr_group = 
{
	.attrs = attrs,
};

static int __init vmouse_init(void)
{
    int ret;

    kobj = kobject_create_and_add("vmouse", NULL); // /sys/mysysfs
    if (!kobj) {
        return -ENOMEM;
    }

    ret = sysfs_create_group(kobj, &attr_group);
	if (ret) {
		kobject_put(kobj);
        return ret;
    }

    vmouse_dev = input_allocate_device();
    if (!vmouse_dev) {
        printk(KERN_ERR "button.c: Not enough memory\n");
        return -ENOMEM;
    }

    vmouse_dev->name = "vmouse"; // to avoid unspecified device warning
    vmouse_dev->evbit[0] = BIT_MASK(EV_KEY)|BIT_MASK(EV_ABS);
    vmouse_dev->keybit[BIT_WORD(BTN_LEFT)] = BIT_MASK(BTN_LEFT);
    vmouse_dev->absbit[BIT_WORD(ABS_X)] |= BIT_MASK(ABS_X);
    vmouse_dev->absbit[BIT_WORD(ABS_Y)] |= BIT_MASK(ABS_Y);
    input_set_abs_params(vmouse_dev, ABS_X, 0,255,0,0);
    input_set_abs_params(vmouse_dev, ABS_Y, 0,255,0,0);

    ret = input_register_device(vmouse_dev);
    if (ret) {
        printk(KERN_ERR "button.c: Failed to register device\n");
        kobject_put(kobj);
        return ret;
    }

    wq = alloc_workqueue("wq", 0, 0);
    INIT_WORK(&wk, work);

    return 0;
}

static void __exit vmouse_exit(void)
{
    flush_work(&wk);
    destroy_workqueue(wq);
    kobject_put(kobj);
    input_unregister_device(vmouse_dev);
}

module_init(vmouse_init);
module_exit(vmouse_exit);
MODULE_LICENSE("Dual BSD/GPL");
