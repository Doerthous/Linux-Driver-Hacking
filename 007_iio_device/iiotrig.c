// TODO: No completed

#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_device.h>
#include <linux/iio/trigger.h>
#include <linux/timer.h>

static void timeout(struct timer_list *timer)
{
    printk(KERN_INFO "timeout\n");
	timer->expires += HZ;
	add_timer(timer);
}

static struct timer_list timer = 
{
	.function = timeout,
};

static int sample_trigger_state(struct iio_trigger *trig, bool state)
{
	printk("switch state %d\n", state);
	if (state) {
		timer.expires = jiffies + HZ;
		add_timer(&timer);
	}
	else {
		del_timer(&timer);
	}
	return 0;
}
static int sample_validate_device(struct iio_trigger *trig,
			       struct iio_dev *indio_dev)
{
	return 0;
}
struct iio_trigger_ops trigger_ops = 
{
	.set_trigger_state = sample_trigger_state,
	.validate_device = sample_validate_device,
};
struct iio_trigger trig;



static int iiotrig_probe(struct platform_device *pdev)
{
	struct iio_trigger *trigger;

	trigger = iio_trigger_alloc("trig-%s-%d", "tmrtrig", 0);
	trigger->ops = &trigger_ops;
	iio_trigger_register(trigger);

	platform_set_drvdata(pdev, trigger);

	return 0;
}

static int iiotrig_remove(struct platform_device *pdev)
{
	struct iio_trigger *trigger;

	trigger = platform_get_drvdata(pdev);
	iio_trigger_unregister(trigger);
	iio_trigger_free(trigger);
    return 0;
}
static const struct of_device_id iiotrig_match[] = {
    { .compatible = "dts,pd", },
    {},
};
MODULE_DEVICE_TABLE(of, iiotrig_match);
static struct platform_driver iiotrig_driver = {
    .probe      = iiotrig_probe,
    .remove     = iiotrig_remove,
    .driver     = {
        .name   = "dts-pd",
        .owner  = THIS_MODULE,
        .of_match_table = iiotrig_match,
    },
};

module_platform_driver(iiotrig_driver);

MODULE_LICENSE("Dual BSD/GPL");
