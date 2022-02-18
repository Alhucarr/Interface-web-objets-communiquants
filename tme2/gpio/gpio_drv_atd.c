#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <mach/platform.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("ATD");
MODULE_DESCRIPTION("Meilleur driver pour les GPIO");

static int btn;
module_param(btn, int, 0);
MODULE_PARM_DESC(btn, "numéro du port du bouton");

#define NBMAX_LED 32
static int leds[NBMAX_LED];
static int nbled;
module_param_array(leds, int, &nbled, 0);
MODULE_PARM_DESC(LEDS, "tableau des numéros de port LED");

#define GPIO_FSEL_INPUT  0
#define GPIO_FSEL_OUTPUT 1

static int major;

static const int LED0 = 4;



struct gpio_s
{
    uint32_t gpfsel[7];
    uint32_t gpset[3];
    uint32_t gpclr[3];
    uint32_t gplev[3];
    uint32_t gpeds[3];
    uint32_t gpren[3];
    uint32_t gpfen[3];
    uint32_t gphen[3];
    uint32_t gplen[3];
    uint32_t gparen[3];
    uint32_t gpafen[3];
    uint32_t gppud[1];
    uint32_t gppudclk[3];
    uint32_t test[1];
}
volatile *gpio_regs = (struct gpio_s *)__io_address(GPIO_BASE);


static void gpio_fsel(int pin, int fun)
{
    uint32_t reg = pin / 10;
    uint32_t bit = (pin % 10) * 3;
    uint32_t mask = 0b111 << bit;
    gpio_regs->gpfsel[reg] = (gpio_regs->gpfsel[reg] & ~mask) | ((fun << bit) & mask);
}

static void gpio_write(int pin, bool val)
{
    if (val)
        gpio_regs->gpset[pin / 32] = (1 << (pin % 32));
    else
        gpio_regs->gpclr[pin / 32] = (1 << (pin % 32));
}

static char gpio_read(int pin)
{
    if((gpio_regs->gplev[pin / 32]) & ((1 << (pin % 32)) != 0)) 
        return 1;
    return 0;
}

static int 
open_gpio_drv(struct inode *inode, struct file *file) {
    int i; 
    printk(KERN_DEBUG "open()\n");
    // selection des leds en OUTPUT
    for(i=0; i < nbled; i++) 
        gpio_fsel(leds[i], GPIO_FSEL_OUTPUT);
    return 0;
}

static ssize_t 
read_gpio_drv(struct file *file, char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "read()\n");
    *buf = gpio_read(btn) ? '0' : '1';
    return gpio_read(btn);
}

static ssize_t 
write_gpio_drv(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    // on definit ici la premiere valeur du buf comme le numero de led
    // et la deuxieme comme la valeur qu'on veut lui appliquer (ON ou OFF)
    printk(KERN_DEBUG "write()\n");     
    printk(KERN_DEBUG "led : %d, val : %d\n", buf[0], buf[1]);
    if(*buf == "0")
        gpio_write(leds[0], 0);
    else 
        gpio_write(leds[0], 1);
    return count;
}

static int 
release_gpio_drv(struct inode *inode, struct file *file) {
    int i; 
    printk(KERN_DEBUG "close()\n");
    // deselection des leds
    for(i=0; i < nbled; i++) 
        gpio_fsel(leds[i], GPIO_FSEL_INPUT);
    return 0;
}

struct file_operations fops_led =
{
    .open       = open_gpio_drv,
    .read       = read_gpio_drv,
    .write      = write_gpio_drv,
    .release    = release_gpio_drv 
};  

static int __init mon_module_init(void)
{
   int i;
   printk(KERN_DEBUG "vous savez moi je ne crois pas qu'il y ait de bonnes ou de mauvaises situations\n");
   printk(KERN_DEBUG "btn=%d !\n    ", btn);
   for (i=0; i < nbled; i++)
      printk(KERN_DEBUG "LED %d = %d\n", i, leds[i]);
      
    major = register_chrdev(0, "gpio_atd", &fops_led); // 0 est le numéro majeur qu'on laisse choisir par linux
    
   return 0;
}

static void __exit mon_module_cleanup(void)
{
    unregister_chrdev(major, "gpio_atd");
    printk(KERN_DEBUG "ciao les nullos\n");
}


module_init(mon_module_init);
module_exit(mon_module_cleanup);