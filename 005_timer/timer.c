/**
 * @file timer.c
 * @author doerthous
 * @brief test for linux kernel timer, kernel version 4.19.97
 * 
 * @copyright Copyright (c) 2020
 */

#include <linux/init.h>
#include <linux/module.h>
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

static int timer_init(void)
{
	timer.expires = jiffies + HZ;
	add_timer(&timer);
	return 0;
}
 
static void timer_exit(void)
{
	del_timer(&timer);
}
 
MODULE_LICENSE("Dual BSD/GPL");
module_init(timer_init);
module_exit(timer_exit);
