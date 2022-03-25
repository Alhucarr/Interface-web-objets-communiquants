# Compte rendu TP4 IOC
## Samy ATTAL, Théo TUGAYE, Hugo DUFEUTRELLE

## Gestion des taches standard périodiques

### 1. Que contient le tableau `waitForTimer()` et à quoi sert-il ?

    Ce tableau contient le nombre de périodes écoulées pour un timer depuis le dernier appel de la fonction `waitFor()`.

### 2. Si on a deux tâches indépendantes avec la même période, pourquoi ne peut-on pas utiliser le même timer dans `waitFor()` ? 

    Parce qu'on associe un timer à une tache et le timer aura sa valeur changée à chaque appel de la fonction. Donc on pourrait perdre l'information du temps écoulée pour une autre tache.

### 3. Dans quel cas la fonction `waitFor()` peut rendre 2 ?

    Lorsqu'on appelle la fonction à un intervalle de deux périodes.

## Utilisation de l'écran OLED 

    L'utilisation de l'écran a été possible dans notre code en gardant une partie de la fonction setup (nous avons choisi de garder l'affichage du logo au début). 

    Nous nous sommes également servi de la fonction waitFor pour compter une seconde avant un nouveau affichage sans bloquer les autres taches.

    Au début l'écran n'affichait pas le compteur, car nous avions oublié d'initialiser la couleur d'écriture, que nous avons ensuite initialisé en blanc. Ensuite nous avons remarqué que le compteur n'évoluait pas, et ce à cause de deux problèmes :

    - Le compteur ne comptait pas du tout car nous avions oublié d'augmenter de 1 le define MAX_WAIT_FOR_TIMER , associé au nombre de timer utilisés.

    - Nous étions donc ensuite figés sur 1 car l'initialisation à 0 de la variable de compteur DANS la fonction loop, ce qui la passait constamment à 0, puis à 1.

    Nous avons rajouté du texte avant et après le compteur en utilisant la formule F("Text") dans d'autres print.

    Nous avons ensuite décidé de positionner le curseur à x = 25 et y = 10 pour centrer le message complet. La fonction était de toute manière utilisée car ce n'est pas fait automatiquement, et qu'il était nécessaire de repositionner le curseur au début du message pour écraser l'ancien message.

    /!\ Ne pas oublier d'effectuer display.display() à la fin de la boucle pour mettre à jour l'écran. et display.clearDIsplay() au début de la boucle pour nettoyer le buffer
    

## Communication inter-taches 

### 1. Dans le texte précédent, quel est le nom de la boîte à lettre et comment est-elle initialisée ? 

    La boite à lettre s'appelle mb0 et initialisée à EMTPY.
