#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <asm/io.h>
#include <mach/platform.h>

#define GPIO_FSEL_INPUT  0
#define GPIO_FSEL_OUTPUT 1

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charlie, 2015");
MODULE_DESCRIPTION("Module, aussitot insere, aussitot efface");

/*
static int btn;
module_param(btn, int, 0);
MODULE_PARM_DESC(btn, "numéro du port du bouton");
*/

#define BTN_PIN 18

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

static uint32_t gpio_read(char pin)
{
    uint32_t ret = gpio_regs->gplev[pin / 32] & ( 1 << (pin % 32));
    return ret >> (pin % 32);
}

static int 
open_bp_ATD(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "open() ATD\n");
    return 0;
}

static ssize_t 
read_bp_ATD(struct file *file, char *buf, size_t count, loff_t *ppos) {
    ssize_t val;
    printk(KERN_DEBUG "read() ATD\n");

    val = gpio_read(BTN_PIN); //on retourne l'argument par pointeur dans buf

    return val;
}

static ssize_t 
write_bp_ATD(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "write() ATD\n");
    return count;
}

static int 
release_bp_ATD(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "close() ATD\n");
    return 0;
}

struct file_operations fops_bp =
{
    .open       = open_bp_ATD,
    .read       = read_bp_ATD,
    .write      = write_bp_ATD,
    .release    = release_bp_ATD
};

static int __init mon_module_init(void)
{
    printk(KERN_DEBUG "Hello World !\n");
    printk(KERN_DEBUG "btn=%d !\n", BTN_PIN);

    major = register_chrdev(0, "bp_ATD", &fops_bp);

    gpio_fsel(BTN_PIN, GPIO_FSEL_INPUT);

    return 0;
}

static void __exit mon_module_cleanup(void)
{
   printk(KERN_DEBUG "Goodbye World!\n");
   unregister_chrdev(major, "bp_ATD");
   gpio_fsel(BTN_PIN, GPIO_FSEL_OUTPUT);
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);