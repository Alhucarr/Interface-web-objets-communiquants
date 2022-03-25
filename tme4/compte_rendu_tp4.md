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

* La boite à lettre s'appelle mb0 et initialisée à EMTPY.

* On a la structure de la tache Lum qui est definie par : 
```arduino
struct Lum {
  int timer;            // le numéro de timer
  unsigned long period; // le temps de scrutation / polling
  int pin;              // la broche du capteur de luminosité
};
```

sa fonction setup: 
```arduino
void setup_lum(struct Lum* lum, int pin, int timer, unsigned long period){
  lum->pin = pin;
  lum->timer = timer;
  lum->period = period;
  pinMode(pin, INPUT);
}
```

ainsi que sa fonction loop: 

```arduino
void loop_lum(struct Lum* lum, struct mailbox_s * mb, struct mailbox_s * mb1) {
  if (!(waitFor(lum->timer,lum->period))) return;
  if (mb->state != EMPTY) return; // attend que la mailbox soit vide
  mb->val = analogRead(lum->pin);
  mb->state = FULL;
  mb1->val = mb->val;
  mb1->state = FULL;
}
```
* On ajoute une mailbox afin que le capteur de luminosité puisse controler également la fréquence de clignotement de la led. 

```arduino
void loop_Led( struct Led_s * ctx, struct mailbox_s* mb) {
  
    if(waitFor(ctx->timer, ctx->period)) {
        digitalWrite(ctx->pin,ctx->etat);                       // ecriture
        ctx->etat = 1 - ctx->etat;                              // changement d'état
    }

    if(mb->state == FULL) {
        ctx->period = map(mb->val, 0, 4095, 100000, 1000000);
        mb->state = EMPTY;
    }

}
```
On map de la valeur de 0 à 4095 car la valeur maximale en sortie du capteur est cette derniere.


## Gestion des interruptions 

On écrit la fonction `serialEvent()` qui va être appelée dans le `void loop()`.

Nous avons decidé aussi de permettre de commander le démarrage du clinotement (avec "s") et son arrêt (avec "r").
```arduino
void serialEvent() {  
  char c;
  if ((c = Serial.read()) == -1) return;
  if (mb_serial.state != EMPTY) return; // attend que la mailbox soit vide
  if (c == 's') { 
    mb_serial.val = 1;
    Serial.println("lecture de s");
  }
  else if (c == 'r') {
    mb_serial.val = 0;
    Serial.println("lecture de r");
  }
  mb_serial.state = FULL;
}
```

Nous avons découvert que l'appel automatique de la fonction `serialEvent()` n'était pas implémenté sur la ESP32, on a donc ajouter à la fin de la fonction `loop()`.

```arduino
void loop()
  loop_serie(&mb0);                 // affichage sur le port série 
  loop_lum(&lum, &mb0, &mb1);       // capteur de luminosité
  loop_Led(&led, &mb1, &mb_serial); // gestion du clignotement de la led
  loop_oled(&oled);                 // gestion de l'écran OLED

  if (Serial.available())
    serialEvent();
```

On appelle `serialEvent()` que s'il y a des données disponibles.