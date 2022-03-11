# Compte rendu TP4 IOC
## Samy ATTAL, Théo TUGAYE, Hugo DUFEUTRELLE

### Gestion des taches standard périodiques

##### 1. Que contient le tableau `waitForTimer()` et à quoi sert-il ?
Ce tableau contient le nombre de périodes écoulées pour un timer depuis le dernier appel de la fonction `waitFor()`.

##### 2. Si on a deux tâches indépendantes avec la même période, pourquoi ne peut-on pas utiliser le même timer dans `waitFor()` ? 
Parce qu'on associe un timer à une tache et le timer aura sa valeur changée à chaque appel de la fonction. Donc on pourrait perdre l'information du temps écoulée pour une autre tache.

##### 3. Dans quel cas la fonction `waitFor()` peut rendre 2 ?
Lorsqu'on appelle la fonction à un intervalle de deux périodes.

### Utilisation de l'écran OLED 

fait.

### Communication inter-taches 

##### 1. Dans le texte précédent, quel est le nom de la boîte à lettre et comment est-elle initialisée ? 
La boite à lettre s'appelle mb0 et initialisée à EMTPY.
