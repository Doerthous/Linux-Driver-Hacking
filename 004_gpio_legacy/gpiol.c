#include <linux/of.h>
#include <linux/of_device.h>
#include <linux/of_gpio.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>


static struct gpiol
{
    unsigned gpio;
    unsigned irq;
} gpiol_instance;

static const struct of_device_id gpiol_dt_ids[] = {
    { .compatible = "dts,gpiol", },
    { }
};
MODULE_DEVICE_TABLE(of, gpiol_dt_ids);

static irqreturn_t gpiol_handle_irq(int irq, void *data)
{
    //struct platform_device *pdev = data;
    printk(KERN_INFO "interrupt!\n");

    return IRQ_HANDLED;
}

static int gpiol_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct device_node *np = pdev->dev.of_node;
    struct gpiol *gpiol = &gpiol_instance;
    int ret;


    gpiol->gpio = of_get_gpio(np, 0); 
    if (!gpio_is_valid(gpiol->gpio)) {
        pr_info("invalid gpio line: %d\n", gpiol->gpio);
        return -EINVAL;
    }
    pr_info("gpiol line: %d\n", gpiol->gpio);

    ret = gpio_request(gpiol->gpio, "gpiol-gpio");
    if (ret < 0) {
        dev_err(dev, "gpio_request failed with %d\n", ret);
        return ret;
    }

    gpiol->irq = gpio_to_irq(gpiol->gpio);
    if (gpiol->irq < 0) {
        dev_err(dev, "GPIO %d has no interrupt\n", gpiol->gpio);
        ret = -EINVAL;
        goto err;
    }

    ret = request_irq(gpiol->irq, gpiol_handle_irq,
                IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING,
                pdev->name, pdev);
    if (ret) {
        dev_err(dev, "request_irq return %d\n", ret);
        goto err;
    }

    platform_set_drvdata(pdev, gpiol);

    return 0;

err:
    gpio_free(gpiol->gpio);
    return ret;
}

static int gpiol_remove(struct platform_device *pdev)
{
    struct gpiol *gpiol;

    gpiol = platform_get_drvdata(pdev);
    free_irq(gpiol->irq, pdev);
    gpio_free(gpiol->gpio);

    return 0;
}

static struct platform_driver gpiol_driver = {
    .driver = {
        .name   = "dts-gpiol",
        .of_match_table = gpiol_dt_ids,
    },
    .probe  = gpiol_probe,
    .remove = gpiol_remove,
};

module_platform_driver(gpiol_driver);

MODULE_LICENSE("GPL v2");
