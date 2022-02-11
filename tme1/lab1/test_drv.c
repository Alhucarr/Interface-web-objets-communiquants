#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
   char led, bp;
   int fdled0 = open("/dev/gpio_drv_atd", O_WR);
   int fdbp = open("/dev/gpio_drv_atd", O_RD);
   if ((fdled0 != 0)||(fdbp != 0)) {
      fprintf(stderr, "Erreur d'ouverture des pilotes LED ou Boutons\n");
      exit(1);
   }
   do { 
      led = (led == '0') ? '1' : '0'; 
      write( fdled0, &led, 1);
      sleep( 1);
      read( fdbp, &bp, 1);
   } while (bp == '0');
   return 0;
}