# Compte rendu TP2 IOC
## Samy Attal, Théo Tugaye, Hugo Dufeutrelle

## Etape 1

La fonction exécutée lorsqu'on insère le module du noyau est "mon_module_init()".
La fonction exécutée lorsqu'on enlève le module du noyau est "mon_module_cleanup()".

## Etape 2

On a suivi les étapes, c'était fluide.

## Etape 3

On a d'abord cru qu'il fallait faire un nouveau type de fichier sans utiliser la structure de l'étape 1.

Puis on est parti du fichier du module de l'étape 2, auquel on ajoute les fonctions "open_LED", "read_LED", etc... et la structure "fops_led".

On a ensuite ajouté les commandes pour enregistrer le driver et lui attribuer un numéro de "major" dans la fonction "init".

On a fait "mknod" pour créer un noeud pour relier les fonctions utilisateur aux fonctions du kernel.

Nous pouvons savoir que le device à été créé on voit (en faisant "cat /proc/devices") que notre device à été ajouté avec son propre numéro de "major".

On a testé avec les méthodes "echo" et "dd", on voit que les fonctions définies sont appellées car la console affiche "open()", "write()", "close()", "open()", "read()", "close()". Il écrit en utilisant le driver("echo") puis lis en utilisant le driver ("dd").

"insdev" récupère le numéro de "major" avec la ligne "awk ...", qui va dans "/proc/devices" et trouve le nom de notre driver et le numéro dde "major" associé.

## Etape 4

Pour les fonctions "read" et "write" on utilise la structure "buf" en paramètre.

On caste "buf" en un tableau de 2 entiers : le premier entier choisit la LED destination et le deuxième choisit la valeur à écrire (1 pour allumer et 0 pour eteindre).

Le bouton poussoir est à front descendant, c'est à prendre en compte lorsqu'on teste sa valeur.

On a eu des problèmes car les flags dans "open" sont O_WRONLY et non pas O_WR (O_RDONLY au lieu de O_RD).
