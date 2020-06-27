#include <linux/init.h>
#include <linux/module.h>

static int hello_init(void){
	printk(KERN_ALERT "Hello, modules world!\n");
	return 0;
}
 
static void hello_exit(void){
	printk(KERN_ALERT "Goodby, cruel world\n");
}
 
MODULE_LICENSE("Dual BSD/GPL");
module_init(hello_init);
module_exit(hello_exit);