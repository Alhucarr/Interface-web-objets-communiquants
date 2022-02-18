#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ATD");
MODULE_DESCRIPTION("Module, aussitot insere, aussitot efface");

static int btn;
module_param(btn, int, 0);
MODULE_PARM_DESC(btn, "numéro du port du bouton");

#define NBMAX_LED 32
static int leds[NBMAX_LED];
static int nbled;
module_param_array(leds, int, &nbled, 0);
MODULE_PARM_DESC(LEDS, "tableau des numéros de port LED");

static int major;

static int 
open_led_ATD(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "open()\n");
    return 0;
}

static ssize_t 
read_led_ATD(struct file *file, char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "read()\n");
    return count;
}

static ssize_t 
write_led_ATD(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "write()\n");
    return count;
}

static int 
release_led_ATD(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "close()\n");
    return 0;
}


struct file_operations fops_led =
{
    .open       = open_led_ATD,
    .read       = read_led_ATD,
    .write      = write_led_ATD,
    .release    = release_led_ATD 
};  

    
static int __init mon_module_init(void)
{
   int i;
   printk(KERN_DEBUG "vous savez moi je ne crois pas qu'il y ait de bonnes ou de mauvaises situations\n");
   printk(KERN_DEBUG "btn=%d !\n    ", btn);
   for (i=0; i < nbled; i++)
      printk(KERN_DEBUG "LED %d = %d\n", i, leds[i]);
    major = register_chrdev(0, "led_atd", &fops_led); // 0 est le numéro majeur qu'on laisse choisir par linux
   
   return 0;
}

static void __exit mon_module_cleanup(void)
{
   unregister_chrdev(major, "led_atd");
   printk(KERN_DEBUG "ciao les nullos\n");
}


module_init(mon_module_init);
module_exit(mon_module_cleanup);
