#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/pinctrl/pinctrl.h>

static void led_release(struct device *dev)
{

}
static struct platform_device led_device = {
    .name = "led",
    .id = 99,
    .dev = {
        .release = led_release,
    }
};


static const struct of_device_id led_match[] = {
    { .compatible = "ttgj,leds", },
    {},
};
MODULE_DEVICE_TABLE(of, led_match);


static int led_probe(struct platform_device *pdev)
{
    int ret = 0;
    struct pinctrl *p;
    struct pinctrl_state *s;

    printk(KERN_INFO "probe!\n");

    p = devm_pinctrl_get(&pdev->dev);
    if (IS_ERR(p)) {
        dev_warn(&pdev->dev, "devm_pinctrl_get, errno: %ld", PTR_ERR(p));
        return PTR_ERR(p);
    }

    s = pinctrl_lookup_state(p, "0"/*PINCTRL_STATE_DEFAULT*/);
    if (IS_ERR(s)) {
        dev_warn(&pdev->dev, "pinctrl_lookup_state, errno: %ld", PTR_ERR(s));
        return PTR_ERR(s);
    }

    ret = pinctrl_select_state(p, s);
    if (ret < 0) {
        dev_warn(&pdev->dev, "pinctrl_select_state, errno: %d", ret);
        return ret;
    }


    /*
    struct pinctrl *p;

    p = devm_pinctrl_get_select_default(&pdev->dev);
    if (IS_ERR(p)) {
       dev_warn(&pdev->dev, "pins are not configured from the driver, errno: %ld", PTR_ERR(p));
       return PTR_ERR(p);
    }
    */

    return 0;
}

static int led_remove(struct platform_device *pdev)
{
    printk(KERN_INFO "remove!\n");
    return 0;
}

static struct platform_driver led_driver = {
    .probe      = led_probe,
    .remove     = led_remove,
    .driver     = {
        .name   = "ttgj-leds",
        .owner  = THIS_MODULE,
        .of_match_table = led_match,
    },
};

static int __init led_init(void)
{
    printk(KERN_INFO "init!\n");
    platform_device_register(&led_device);
    return platform_driver_register(&led_driver);
}

static void __exit led_exit(void)
{
    printk(KERN_INFO "exit\n");
    platform_driver_unregister(&led_driver);
    platform_device_unregister(&led_device);
}


MODULE_LICENSE("Dual BSD/GPL");
module_init(led_init);
module_exit(led_exit);

