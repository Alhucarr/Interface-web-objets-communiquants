# Compte rendu TP5 IOC
## Samy ATTAL, Théo TUGAYE, Hugo DUFEUTRELLE

## Communication par FIFO

### Dans quel répertoire est créée la fifo ?

    La fifo est crée dans le répertoire tmp, de sorte que son contenu soit temporaire.

### Quelle différence mkfifo et open ?

    mkfifo créé la fifo, alors que open permet de configurer son ouverture avec les différents flags.

### Pourquoi tester que la fifo existe ?

    Pour éviter de créer une fifo qui existe déjà. Comme on utilise deux codes différent, on veut éviter que les deux puissent créer la même fifo, l'un après l'autre. Donc le premier code va créer la fifo puisqu'il n'y en avait pas, et le second va détecter cette fifo et l'utiliser sans en créer une autre.

### À quoi sert flush ?

    Ça sert à vider le buffer pour écrire dans le pipe.

### Pourquoi ne ferme-t-on pas la fifo ?

    Puisque la fifo est dans le repertoire tmp, elle est considérée comme un fichier temporaire et donc sera supprimée par l'OS.

### Que fait readline ?

    Elle est similaire à la fonction read() en c et effectue une lecture d'un ligne dans un fichier.

### Expliquer le phénomène

    Lorsque la première écriture du writer se produit, l'écrivain vide son buffer avec flush et écrit sur la fifo, la remplissant. Cependant il ne peut pas écrire dans la fifo à nouveau tant qu'elle est pleine, il attend donc qu'une lecture ai lieu, pour réécrire à nouveau.
    
## Création d'un serveur fake 

    