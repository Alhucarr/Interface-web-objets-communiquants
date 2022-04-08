#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>
#include <mach/platform.h>

#define GPIO_FSEL_INPUT  0
#define GPIO_FSEL_OUTPUT 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charlie, 2015");
MODULE_DESCRIPTION("Module, aussitot insere, aussitot efface");

//#define NBMAX_LED 2
//static int leds[NBMAX_LED];
//static int nbled;
//module_param_array(leds, int, &nbled, 0);
//MODULE_PARM_DESC(LEDS, "tableau des numéros de port LED");

#define LED_PIN 4

static int major;

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

static void gpio_write(bool val)
{
        if (val)
            gpio_regs->gpset[LED_PIN / 32] = (1 << (LED_PIN % 32));
        else
           gpio_regs->gpclr[LED_PIN / 32] = (1 << (LED_PIN % 32));
    }

static int 
open_led_ATD(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "open() ATD\n");
    return 0;
}

static ssize_t 
read_led_ATD(struct file *file, char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "read() ATD\n");
    return count;
}

static ssize_t 
write_led_ATD(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    int val = (int)buf[0];
    if(val == 49)
        val = 1;
    else
        val = 0;

    printk(KERN_DEBUG "[ATD] write led (val) = %d\nchar = %c\n", val, buf[0]);
    gpio_write(val); // buf[0] contient l'indice (0 ou 1) de la LED et buf[1] la valeur

    return count;
}

static int 
release_led_ATD(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "close() ATD\n");
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
    printk(KERN_DEBUG "Hello World ! ATD\n");
    /*
    for (i=0; i < nbled; i++)
       printk(KERN_DEBUG "LED %d = %d\n", i, leds[i]);*/

   major = register_chrdev(0, "led_ATD", &fops_led);
   gpio_fsel(LED_PIN, GPIO_FSEL_OUTPUT);

    return 0;
}

static void __exit mon_module_cleanup(void)
{
   printk(KERN_DEBUG "Goodbye World! ATD\n");
   unregister_chrdev(major, "led_ATD");
   gpio_fsel(LED_PIN, GPIO_FSEL_INPUT);
   //gpio_fsel(leds[1], GPIO_FSEL_INPUT);
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);