# Compte rendu TP1 IOC
## Samy Attal, Théo Tugaye, Hugo Dufeutrelle

## 2

On doit utiliser la redirection des ports car nous sommes plusieurs à utiliser la même carte.

Il faut créer un repertoire propre pour éviter que des fichiers ne soient écrasés et que vous sachiez qui a fait quoi.

## 5

Il est dangereux de se tromper de broche car il y a des tensions différentes aux bornes des broches et des fonctionnalités différentes (entrées/sorties, GPIO, Gnd, etc...).

L'adresse BCM2835_GPIO_BASE correspond à l'adresse physique de la structure des périphériques, qui sera mappée dans la mémoire virtuelle.

La structure "gpio_s" correspond aux regsitres des GPIOs mappés dans la mémoire virtuelle.

"gpio_regs_virt" se situe dans la mémoire virtuelle du processus courant.

Dans "f_sel" la variable "reg" correspond à la case du tableau "gpfsel" de registres qui contient le registre du GPIO qu'on va vouloir manipuler.

On écrit dans gpset ou gpclr pour soit mettre la valeur entrée du GPIO à 1, soit à 0 respectivement.

Les flags de "open" :
	- O_RDWR permet de lire et d'écrire sur le fichier.
	- O_SYNC permet de ne pas revenir de l'écriture avant que toutes les données aient été écrites.

(source man mmap)
Les arguments de "mmap" :
	- "NULL" dit au noyau qu'il peut choisir où mapper.
	- "RPI_BLOCK_SIZE" correspond à la taille des données à mapper, 4096 octets en l'occurence.
	- "PROT_READ" et "PROT_WRITE" permet de lire et d'écrire sur la zone mémoire mappée.
	- "MAP_SHARED" permet aux autres processus d'accéder à la zone mappée.
	- "mmap_fd" est le fichier qui sera utilisé pour mapper.
	- "BCM2835_GPIO_BASE" contient l'"offset", içi le décalage qui indique la zone correspondant aux périphériques.
	







