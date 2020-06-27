#include <linux/init.h>
#include <linux/module.h>
#include <linux/workqueue.h>
#include <linux/completion.h>

static struct workqueue_struct *wq;
static struct work_struct wk;
static struct delayed_work dwk;
static struct completion comp;

static void delayed_work(struct work_struct *work)
{
    printk("%s\n", __func__);

    complete(&comp);
}
static void work(struct work_struct *work)
{
    printk("%s\n", __func__);

    reinit_completion(&comp);

    INIT_DELAYED_WORK(&dwk, delayed_work);
    if (!queue_delayed_work(wq, &dwk, 5*HZ)) {
        printk(KERN_ERR "delayed work was already on a queue.\n");
    }

    wait_for_completion(&comp);
    printk("%s done\n", __func__);
}

static int wq_init(void)
{
    wq = alloc_workqueue("wq", 0, 0);
    if (wq == NULL) {
        printk(KERN_ERR "create workqueue failed.\n");
        return -ENOMEM;
    }

    init_completion(&comp);

    INIT_WORK(&wk, work);
    if (!queue_work(wq, &wk)) {
        printk(KERN_ERR "work was already on a queue.\n");
    }

	return 0;
}
 
static void wq_exit(void)
{
    if (wq) {
        flush_work(&wk);
        // cancel delay timer, immediately enqueue the work
        flush_delayed_work(&dwk);

        destroy_workqueue(wq);
    }
}
 
MODULE_LICENSE("Dual BSD/GPL");
module_init(wq_init);
module_exit(wq_exit);
