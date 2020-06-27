#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>

static const struct of_device_id pd_match[] = {
    { .compatible = "dts,pd", },
    {},
};
MODULE_DEVICE_TABLE(of, pd_match);


static int pd_probe(struct platform_device *pdev)
{
    printk(KERN_INFO "probe!\n");
    return 0;
}

static int pd_remove(struct platform_device *pdev)
{
    printk(KERN_INFO "remove!\n");
    return 0;
}

static struct platform_driver pd_driver = {
    .probe      = pd_probe,
    .remove     = pd_remove,
    .driver     = {
        .name   = "dts-pd",
        .owner  = THIS_MODULE,
        .of_match_table = pd_match,
    },
};

static int __init pd_init(void)
{
    printk(KERN_INFO "init!\n");
    return platform_driver_register(&pd_driver);
}

static void __exit pd_exit(void)
{
    printk(KERN_INFO "exit\n");
    platform_driver_unregister(&pd_driver);
}


MODULE_LICENSE("Dual BSD/GPL");
module_init(pd_init);
module_exit(pd_exit);
