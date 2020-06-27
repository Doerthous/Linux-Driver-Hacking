#include <linux/init.h>
#include <linux/module.h>
#include <linux/kthread.h>

/**
  * ndelay, udelay, mdelay
  * usleep_range, msleep,  msleep_interruptible, ssleep
  */
#include <linux/delay.h>

static struct task_struct *thrd;

static int thread(void *data)
{
    while (!kthread_should_stop()) {
        ssleep(1);
        printk(KERN_INFO "wake up.\n");
    }

    return 0;
}

static int kthrd_init(void)
{
    thrd = kthread_run(thread, 0, "kthrd");
    if (IS_ERR_OR_NULL(thrd)) {
        printk(KERN_ERR "kthread_run failed with %ld.\n", PTR_ERR(thrd));
        return PTR_ERR(thrd);
    }

	return 0;
}
 
static void kthrd_exit(void)
{
    int ret; 
    if (thrd > 0) {
        ret = kthread_stop(thrd);
        printk(KERN_INFO "thread return %d.\n", ret);
    }
}
 
MODULE_LICENSE("Dual BSD/GPL");
module_init(kthrd_init);
module_exit(kthrd_exit);
