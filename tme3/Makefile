CARD_NUMB       = 20
ROUTER          = peri
LOGIN           = tugaye_attal_dufeutrelle
LAB             = lab3

MODULE          = driver_lcd_atd

CROSSDIR        = /users/enseig/franck/IOC
KERNELDIR       = /dsk/l1/misc/tugaye/linux-rpi-3.18.y
CROSS_COMPILE   = $(CROSSDIR)/arm-bcm2708hardfp-linux-gnueabi/bin/bcm2708hardfp-
        
obj-m           += $(MODULE).o
default:;       make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) modules
clean:;         make -C $(KERNELDIR) ARCH=arm CROSS_COMPILE=$(CROSS_COMPILE) M=$(PWD) clean
upload:;        scp -P622$(CARD_NUMB) $(MODULE).ko pi@$(ROUTER):$(LOGIN)/$(LAB)