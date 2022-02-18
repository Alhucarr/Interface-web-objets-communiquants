# Compte-rendu TP3
### Samy Attal - Théo Tugaye - Hugo Dufeutrelle
----
#### Question LCD_user.c

1) Le mot clé "volatile" permet de dire au compilateur de ne rien optimiser concernant cette variable.
En effet, cette variable n'est pas seulement modifiée par le processeur mais aussi par des entrées/sorties, il faut donc empêcher le compilateur d'exécuter des commandes dans le désordre pour optimiser l'exécution, étant donnée que la valeur contenue dans cette variable peut changer sans avertir le compilateur.

2) Les flags dans "open" :
	- O_RDWR sert à pouvoir lire et écrire dans le fichier ouvert
	- O_SYNC sert à attendre que l'écriture soit faite dans la mémoire avant de continuer l'exécution.
Ces flags garantissent une exécution avec des données proprement actualisées.

3) On appelle "munmap()" car on doit désallouer la mémoire après le "mmap()" de la fonction "gpio_setup()".

4) "LCD_FUNCTIONSET", couplé aux flags "LCD_FS_4BITMODE | LCD_FS_2LINE | LCD_FS_5x8DOTS" permet de passer le composant en mode 4 bits (c'est-à-dire qu'on utilise que 4 ports GPIO pour les arguments des commandes) et de n'utiliser que les 40 pixels LCD du haut des cases.

"LCD_DISPLAYCONTROL", couplé aux flags "LCD_DC_DISPLAYON | LCD_DC_CURSOROFF" permet d'afficher des données sur le LCD et de ne pas afficher le curseur.

"LCD_ENTRYMODESET", couplé aux flags "LCD_EM_RIGHT | LCD_EM_DISPLAYNOSHIFT" positionne l'affichage à droite, avec aucun décalage de départ.

5) On a 2 boucles imbriqués, la première permet de passer d'une ligne du LCD à l'autre (le décalage d'une ou plusieurs lignes se fait à partir des indices contenus dans le tableau "a" avec la fonction lcd_command(LCD_SETDDRAMADDR + a[l]), la seconde gère l'écriture caractère par caractère du message à affficher.

