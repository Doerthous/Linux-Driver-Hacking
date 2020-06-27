#include <linux/init.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/iio/iio.h>
#include <linux/iio/buffer.h>


struct iiopd_priv
{
    int nothing;
};


static int iiopd_read_raw(struct iio_dev *iio_dev,
              const struct iio_chan_spec *chan,
            int *val, int *val2, long mask)
{
    int ret;


    dev_dbg(iio_dev->dev.parent, "iiopd_read_raw\n");

    ret = -EINVAL;
    while (1) {
        if (chan->type == IIO_CURRENT) {
            if (chan->channel == 0) {
                ret = IIO_VAL_INT;
                *val = 0;
                break;
            }
            if (chan->channel == 1) {
                ret = IIO_VAL_INT;
                *val = 100;
                break;
            }
        }
        if (chan->type == IIO_VOLTAGE) {
            if (mask == IIO_CHAN_INFO_RAW) {
                /* 321 + 654*1e-6 */
                *val = 321;
                *val2 = 654;
                ret = IIO_VAL_INT_PLUS_NANO;
                break;
            }
            if (mask == IIO_CHAN_INFO_SCALE) {
                /* 123 + 456*1e-6 */
                *val = 123;
                *val2 = 456;
                ret = IIO_VAL_INT_PLUS_MICRO;
                break;
            }
            if (mask == IIO_CHAN_INFO_OFFSET) {
                /* 1 + 4*1e-6 dB */
                *val = 1;
                *val2 = 4;
                ret = IIO_VAL_INT_PLUS_MICRO_DB;
                break;
            }
            if (mask == IIO_CHAN_INFO_PROCESSED) {
                /* 3/6 */
                *val = 3;
                *val2 = 6;
                ret = IIO_VAL_FRACTIONAL;
                break;
            }
        }
    }
    
    return ret;
}

static const struct iio_info iiopd_iio_info = {
    .read_raw       = iiopd_read_raw,
};

// {direction}_{type}[index]_[modifier]_{info}
static const struct iio_chan_spec iiopd_chan_spec[] = {
    /* 1 voltage channel with 4 info */
    { 
        .type = IIO_VOLTAGE,
        .info_mask_separate = 
            BIT(IIO_CHAN_INFO_RAW) | 
            BIT(IIO_CHAN_INFO_SCALE) |
            BIT(IIO_CHAN_INFO_OFFSET) |
            BIT(IIO_CHAN_INFO_PROCESSED),
    },

    /* 3 current channel */
    {   
        .type = IIO_CURRENT,
        .indexed = 1,
        .channel = 0,
        .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
        .info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),
    },
    {   
        .type = IIO_CURRENT,
        .indexed = 1,
        .channel = 1,
        .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
        .info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),
    },
    {   
        .type = IIO_CURRENT,
        .indexed = 1,
        .channel = 2,
        .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
        .info_mask_shared_by_type = BIT(IIO_CHAN_INFO_SCALE),
    },

    /* 3 light channel */
    { 
        .type = IIO_LIGHT,
        .modified = 1,
        .channel2 = IIO_MOD_LIGHT_RED,
        .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
    },
    { 
        .type = IIO_LIGHT,
        .modified = 1,
        .channel2 = IIO_MOD_LIGHT_GREEN,
        .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
    },
    { 
        .type = IIO_LIGHT,
        .modified = 1,
        .channel2 = IIO_MOD_LIGHT_BLUE,
        .info_mask_separate = BIT(IIO_CHAN_INFO_RAW),
    },

    /* iio buffer */
    {
        .type = IIO_ACCEL,
        .modified = 1,
        .channel2 = IIO_MOD_X,
        /* other stuff here */
        .scan_index = 0,
        .scan_type = {
                .sign = 's',
                .realbits = 12,
                .storagebits = 16,
                .shift = 4,
                .endianness = IIO_LE,
        },
    },
};



static const struct of_device_id iiopd_match[] = {
    { .compatible = "dts,pd", },
    {},
};
MODULE_DEVICE_TABLE(of, iiopd_match);

static int iiopd_probe(struct platform_device *pdev)
{
    struct device *dev = &pdev->dev;
    struct iiopd_priv *iiopd;
    struct iio_dev *iio;
    int ret;


    iio = iio_device_alloc(sizeof(*iiopd));
    if (!iio) {
        dev_err(dev, "Failed to allocate IIO device\n");
        return -ENOMEM;
    }

    iiopd = iio_priv(iio);
    iio->name = pdev->name;
    iio->dev.parent = &pdev->dev;
    iio->info = &iiopd_iio_info;
    iio->modes = INDIO_DIRECT_MODE;
    iio->channels = iiopd_chan_spec;
    iio->num_channels = ARRAY_SIZE(iiopd_chan_spec);

    ret = iio_device_register(iio);
    if (ret < 0) {
        dev_err(dev, "Failed to register IIO device\n");
        iio_device_free(iio);
        return ret;
    }

    platform_set_drvdata(pdev, iio);

    return ret;
}

static int iiopd_remove(struct platform_device *pdev)
{
    struct iio_dev *iio;

    iio = platform_get_drvdata(pdev);
    iio_device_unregister(iio);
    iio_device_free(iio);

    return 0;
}

static struct platform_driver iiopd_driver = {
    .probe      = iiopd_probe,
    .remove     = iiopd_remove,
    .driver     = {
        .name   = "dts-iiopd",
        .owner  = THIS_MODULE,
        .of_match_table = iiopd_match,
    },
};

module_platform_driver(iiopd_driver);


MODULE_LICENSE("Dual BSD/GPL");


