/*******************************************************************************
 * lcdr_user.c - Controleur pour LCd HD44780 ( 20x4 )
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

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

// Q1: A quoi sert le mot clé volatile
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
volatile struct gpio_s *gpio_regs;

/*******************************************************************************
 * GPIO Operations 
 ******************************************************************************/

// Q2: Expliquer la présence des drapeaux dans open() et mmap().
int gpio_setup(void)
{

    int mmap_fd = open("/dev/mem", O_RDWR | O_SYNC);
    if (mmap_fd < 0) {
        return -1;
    }
    gpio_regs = mmap(NULL, RPI_BLOCK_SIZE,
                     PROT_READ | PROT_WRITE, MAP_SHARED,
                     mmap_fd,
                     RPI_GPIO_BASE);
    if (gpio_regs == MAP_FAILED) {
        close(mmap_fd);
        return -1;
    }
    return 0;
}

// Q3: pourquoi appeler munmap() ?
void gpio_teardown(void)
{
    munmap((void *) gpio_regs, RPI_BLOCK_SIZE);
}

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

/*******************************************************************************
 * LCD's Operations
 ******************************************************************************/

int ligne_actuelle = 0;
int decalage = 0;
/* generate E signal */
void lcd_strobe(void)
{
    gpio_write(E, 1);
    usleep(1);
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
    usleep(2000);               // certaines commandes sont lentes 
}

void lcd_data(int character)
{
    gpio_write(RS, 1);
    lcd_write4bits(character);
    usleep(1);
}

/* initialization : pour comprendre la séquence, il faut regarder le cours */
// Q4: Expliquer le rôle des masques : LCD_FUNCTIONSET, LCD_FS_4BITMODE, etc.
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

// Q5: Expliquez comment fonctionne cette fonction 
void lcd_message(const char *txt)
{
    int a[] = { 0, 0x40, 0x14, 0x54 };
    int len = 20-decalage;
    int i, l,cpt=0;
    for (i = 0,l=0; (l<4) && (i<strlen(txt)); l++){
            lcd_command(LCD_SETDDRAMADDR + a[ligne_actuelle+1]);	
        for (; (i < (l + 1) * len) && (i < strlen(txt)); i++) {
            lcd_data(txt[i]);
            cpt++;
            if((cpt == (len-1)) && (cpt < strlen(txt))){
                lcd_command(LCD_SETDDRAMADDR + a[ligne_actuelle+1]);
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

/*******************************************************************************
 * main : affichage d'un message
 ******************************************************************************/
int main(int argc, char **argv)
{
    /* arg */
    if (argc < 2) {
        fprintf(stderr, "ERROR: must take a string as argument\n");
        exit(1);
    }

    /* Retreive the mapped GPIO memory */
    if (gpio_setup() == -1) {
        perror("ERROR: gpio_setup\n");
        exit(1);
    }

    /* initialization */
    lcd_init();
    lcd_clear();

    /* Setting up GPIOs to output */
    gpio_config(RS, GPIO_OUTPUT);
    gpio_config(E,  GPIO_OUTPUT);
    gpio_config(D4, GPIO_OUTPUT);
    gpio_config(D5, GPIO_OUTPUT);
    gpio_config(D6, GPIO_OUTPUT);
    gpio_config(D7, GPIO_OUTPUT);

    /*test curseur*/
    //lcd_set_cursor(1,10);

    /* affichage */
    lcd_message(argv[1]);
    sleep(2);

    /* écriture sur 4 lignes*/
    //write4lines(argv[1]);
    //sleep(2);

    /* Release the GPIO memory mapping */
    gpio_teardown();

    return 0;
}
