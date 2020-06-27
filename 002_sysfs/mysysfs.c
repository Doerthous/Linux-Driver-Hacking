#include <linux/init.h>
#include <linux/module.h>

static ssize_t store(struct kobject *kobj,
            struct kobj_attribute *attr,
            const char *buf, size_t n)
{
    printk(KERN_INFO "your input: %s\n", buf);
        return n;
}

static ssize_t show(struct kobject *kobj,
            struct kobj_attribute *attr, char *buf)
{
    return sprintf(buf, "%s\n", "Hello mysysfs");
}

static struct kobject *kobj;
struct mysysfs_attribute {
    struct attribute	attr;
    ssize_t (*show)(struct kobject *kobj, struct kobj_attribute *attr,
            char *buf);
    ssize_t (*store)(struct kobject *kobj, struct kobj_attribute *attr,
             const char *buf, size_t n);
};
static struct mysysfs_attribute attrs[] = {
    __ATTR(item, S_IRUGO | S_IWUSR, show, store),
};

static int __init mysysfs_init(void)
{
    int ret = 0, i;

    printk(KERN_INFO "init!\n");

    kobj = kobject_create_and_add("mysysfs", NULL); // /sys/mysysfs
    //kobj = kobject_create_and_add("mysysfs", kernel_kobj); // /sys/kernel/mysysfs
    if (!kobj) {
        return -ENOMEM;
    }

    for (i = 0; i < ARRAY_SIZE(attrs); i++) {
        ret = sysfs_create_file(kobj, &attrs[i].attr);
        if (ret != 0) {
            printk(KERN_ERR "create index %d error\n", i);
            kobject_put(kobj);
            return ret;
        }
    }

    return 0;
}

static void __exit mysysfs_exit(void)
{
    kobject_put(kobj);
    printk(KERN_INFO "exit\n");
}


MODULE_LICENSE("Dual BSD/GPL");
module_init(mysysfs_init);
module_exit(mysysfs_exit);
