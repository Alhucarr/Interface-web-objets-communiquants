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

    Pour ce faire, nous avons repris le code Fake, en modifiant chaque "fw" par nos initiales, c'est à dire "atd" avec un Ctrl + F.

    Ensuite nous avons modifié le select dans fake. Nous avons commencé par essayer de trouver comment accéder aux fifos stdin et stdout. Nous aurions du avoir à éplucher chacun des bibliothèques pour trouver les define stdin et stdout si nous n'avions pas utilisé VSCode, qui nous a bien simplifié la tâche en nous proposant automatiquement "STDIN_FILNEO" et "STDOUT_FILENO" après avoir tapé "std". Ces defines se trouvent donc dans la bibliothèque "unistd.h".

    Ensuite nous avons ajouté le masque "FD_SET(STDIN_FILENO, &rfds);" après le masque sur f2s, pour ajouter également la fifo stdin dans les descripteurs de fichier attendus en lecture, puis nous avons LITTÉRALEMENT copié collé le code qui est exécuté lorsque le flux de la fifo s2f est bien actif, pour qu'il soit aussi executé lorsque le flux de la fifo stdin est actif. Ensuite nous avons évidemment modifié la ligne de lecture pour lire la fifo "STDIN_FILENO", et laissé inchangé la ligne d'écriture. Car ce que nous voulons, c'est que ce soit les données de la fifo s2f qui soient écritent sur la fifo stdout. Donc nous avons donc modifié en conséquence la ligne d'écriture du code de la fifo s2f pour qu'elle écrive sur la fifo "STDOUT_FILENO", ce qui correspond à la fifo std_out.

    Enfin nous avons donc modifié le code du serveur en python, en initialisant une variable, pour y stoquer les caractères écrits aux clavier grâce à la fonction "raw_input()". Nous avons ensuite remplacé le "w hello\n" par la variable. 

    Note : pour éviter que les caractères s'écrivent à la suite du message envoyé par raw_input(), alors il faut ajouter dedans le \n pour sauter la ligne.

## Création d'un server web

    