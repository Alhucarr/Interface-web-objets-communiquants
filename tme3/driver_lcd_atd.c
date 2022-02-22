#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <asm/io.h>
#include <linux/slab.h>
#include <mach/platform.h>
#include <asm/delay.h>
#include <linux/uaccess.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Charlie, 2015");
MODULE_DESCRIPTION("Module, aussitot insere, aussitot efface");

/*******************************************************************************
 * GPIO Pins
 ******************************************************************************/
#define RS 7
#define E  27
#define D4 22
#define D5 23
#define D6 24
#define D7 25

#define GPIO_INPUT  0
#define GPIO_OUTPUT 1

#define RPI_BLOCK_SIZE  0xB4
#define RPI_GPIO_BASE   0x20200000

struct gpio_s {
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
};
volatile struct gpio_s *gpio_regs = (struct gpio_s *)__io_address(GPIO_BASE);

/*******************************************************************************
 * GPIO Operations 
 ******************************************************************************/

void gpio_config(int gpio, int value)
{
    int regnum = gpio / 10;
    int offset = (gpio % 10) * 3;
    gpio_regs->gpfsel[regnum] &= ~(0x7 << offset);
    gpio_regs->gpfsel[regnum] |= ((value & 0x7) << offset);
}

void gpio_write(int gpio, int value)
{
    int regnum = gpio / 32;
    int offset = gpio % 32;
    if (value&1)
        gpio_regs->gpset[regnum] = (0x1 << offset);
    else
        gpio_regs->gpclr[regnum] = (0x1 << offset);
}

/*******************************************************************************
 * LCD's Instructions ( source = doc )
 * Ces constantes sont utilisées pour former les mots de commandes
 * par exemple dans la fonction lcd_init()
 ******************************************************************************/

/* commands */
#define LCD_CLEARDISPLAY        0b00000001
#define LCD_RETURNHOME          0b00000010
#define LCD_ENTRYMODESET        0b00000100
#define LCD_DISPLAYCONTROL      0b00001000
#define LCD_CURSORSHIFT         0b00010000
#define LCD_FUNCTIONSET         0b00100000
#define LCD_SETCGRAMADDR        0b01000000
#define LCD_SETDDRAMADDR        0b10000000

/* flags for display entry mode : combine with LCD_ENTRYMODESET */
#define LCD_EM_RIGHT            0b00000000
#define LCD_EM_LEFT             0b00000010
#define LCD_EM_DISPLAYSHIFT     0b00000001
#define LCD_EM_DISPLAYNOSHIFT   0b00000000

/* flags for display on/off control : combine with LCD_DISPLAYCONTROL */
#define LCD_DC_DISPLAYON        0b00000100
#define LCD_DC_DISPLAYOFF       0b00000000
#define LCD_DC_CURSORON         0b00000010
#define LCD_DC_CURSOROFF        0b00000000
#define LCD_DC_BLINKON          0b00000001
#define LCD_DC_BLINKOFF         0b00000000

/* flags for display/cursor shift : combine with LCD_CURSORSHIFT */
#define LCD_CS_DISPLAYMOVE      0b00001000
#define LCD_CS_CURSORMOVE       0b00000000
#define LCD_CS_MOVERIGHT        0b00000100
#define LCD_CS_MOVELEFT         0b00000000

/* flags for function set : combine with LCD_FUNCTIONSET */
#define LCD_FS_8BITMODE         0b00010000
#define LCD_FS_4BITMODE         0b00000000
#define LCD_FS_2LINE            0b00001000
#define LCD_FS_1LINE            0b00000000
#define LCD_FS_5x10DOTS         0b00000100
#define LCD_FS_5x8DOTS          0b00000000

static int major;

/*******************************************************************************
 * LCD's Operations
 ******************************************************************************/

int ligne_actuelle = 0;
int decalage = 0;
/* generate E signal */
void lcd_strobe(void)
{
    gpio_write(E, 1);
    udelay(1);
    gpio_write(E, 0);
}

/* send 4bits to LCD : valable pour les commande et les data */

void lcd_write4bits(int data)
{
    /* first 4 bits */
    gpio_write(D7, data>>7);
    gpio_write(D6, data>>6);
    gpio_write(D5, data>>5);
    gpio_write(D4, data>>4);
    lcd_strobe();

    /* second 4 bits */
    gpio_write(D7, data>>3);
    gpio_write(D6, data>>2);
    gpio_write(D5, data>>1);
    gpio_write(D4, data>>0);
    lcd_strobe();
}

void lcd_command(int cmd)
{
    gpio_write(RS, 0);
    lcd_write4bits(cmd);
    udelay(2000);               // certaines commandes sont lentes 
}

void lcd_data(int character)
{
    gpio_write(RS, 1);
    lcd_write4bits(character);
    udelay(1000);
    printk(KERN_DEBUG "[ATD] Char : %c \n", character);
}

void lcd_init(void)
{
    gpio_write(E, 0);
    lcd_command(0b00110011);    /* initialization */
    lcd_command(0b00110010);    /* initialization */
    lcd_command(LCD_FUNCTIONSET | LCD_FS_4BITMODE | LCD_FS_2LINE | LCD_FS_5x8DOTS);
    lcd_command(LCD_DISPLAYCONTROL | LCD_DC_DISPLAYON | LCD_DC_CURSOROFF);
    lcd_command(LCD_ENTRYMODESET | LCD_EM_RIGHT | LCD_EM_DISPLAYNOSHIFT);
}

void lcd_clear(void)
{
    lcd_command(LCD_CLEARDISPLAY);
    lcd_command(LCD_RETURNHOME);

}

void lcd_message(const char *txt)
{
    int a[] = { 0, 0x40, 0x14, 0x54 };
    int len = 20-decalage;
    int i, l,cpt=0;
    for (i = 0,l=0; (l<4) && (i<strlen(txt)); l++){
            lcd_command(LCD_SETDDRAMADDR + a[ligne_actuelle]);	
        for (; (i < (l + 1) * len) && (i < strlen(txt)); i++) {
            printk(KERN_DEBUG "[ATD] Texte : %c \n", txt[i]);
            lcd_data(txt[i]);
            cpt++;
            if((cpt == (len-1)) && (cpt < strlen(txt))){
                lcd_command(LCD_SETDDRAMADDR + a[ligne_actuelle]);
            }
        }
    }
}

void write4lines(const char *txt) {
    int i;
    int a[] = { 0, 0x40, 0x14, 0x54 };

    for(i = 0; (i < strlen(txt)) && (i < 20); i++) {
        lcd_command(LCD_SETDDRAMADDR + a[0]+i);
        lcd_data(txt[i]);
        lcd_command(LCD_SETDDRAMADDR + a[1]+i);
        lcd_data(txt[i]);
        lcd_command(LCD_SETDDRAMADDR + a[2]+i);
        lcd_data(txt[i]);
        lcd_command(LCD_SETDDRAMADDR + a[3]+i);
        lcd_data(txt[i]);
    }
}

void
lcd_set_cursor(int x, int y) {
	int a[] = { 0, 0x40, 0x14, 0x54 };
    decalage = y;
    ligne_actuelle = x;
    lcd_command(LCD_SETDDRAMADDR + a[x] + y);	
	// si le curseur ne s'affiche pas peut être décommenter
	lcd_command(LCD_DISPLAYCONTROL | LCD_DC_DISPLAYON | LCD_DC_CURSORON);
}

static int 
open_lcd_ATD(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "open() ATD\n");

    /* Setting up GPIOs to output */
    gpio_config(RS, GPIO_OUTPUT);
    gpio_config(E,  GPIO_OUTPUT);
    gpio_config(D4, GPIO_OUTPUT);
    gpio_config(D5, GPIO_OUTPUT);
    gpio_config(D6, GPIO_OUTPUT);
    gpio_config(D7, GPIO_OUTPUT);

    file->private_data = kmalloc(500, GFP_KERNEL);
    return 0;
}

static ssize_t 
read_lcd_ATD(struct file *file, char *buf, size_t count, loff_t *ppos) {
    printk(KERN_DEBUG "read() ATD\n");
    return count;
}

static ssize_t 
write_lcd_ATD(struct file *file, const char *buf, size_t count, loff_t *ppos) {
    copy_from_user(file->private_data, buf, count+1);
    lcd_set_cursor(0,0);
    lcd_message(file->private_data);
    printk(KERN_DEBUG "write() ATD\n");
    return count;
}

static int 
release_lcd_ATD(struct inode *inode, struct file *file) {
    printk(KERN_DEBUG "close() ATD\n");

    kfree(file->private_data);
    return 0;
}

struct file_operations fops_lcd =
{
    .open       = open_lcd_ATD,
    .read       = read_lcd_ATD,
    .write      = write_lcd_ATD,
    .release    = release_lcd_ATD 
};

static int __init mon_module_init(void)
{
    printk(KERN_DEBUG "[ATD] Kernel module LCD insere\n");

    /* initialization */
    lcd_init();
    lcd_clear();

    major = register_chrdev(0, "driver_lcd_atd", &fops_lcd);

    return 0;
}

static void __exit mon_module_cleanup(void)
{
    printk(KERN_DEBUG "[ATD] Kernel module LCD retire\n");
    unregister_chrdev(major, "lcd_ATD");
}

module_init(mon_module_init);
module_exit(mon_module_cleanup);