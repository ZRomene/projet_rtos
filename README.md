# Projet RTOS
#### Réalisé par Zaynab ROMENE
[![N|Solid](https://hackster.imgix.net/uploads/cover_image/file/128512/ArduinoFreeRTOSLogo.png?auto=compress&w=900&h=675&fit=min&fm=jpg)]()

# Présentation du projet 

Ce projet est un programme sur Arduino en utilisant FreeRTOS. Il permet de créer et synchroniser les 5 tâches suivantes: 
- Récuperation d'une valeur analogique (entre 0 et 1023) d'un potentiomètre (A0) puis l’envoie à la tâche 3.
- Récuperation d'une valeur numérique (entre 0 et 2) la somme des deux valeurs numériques 3 et 4 des boutons poussoirs en montage pull down) puis l'envoie à la tâche 3.
- Réception des deux valeurs, les mettre dans une structure et les envoyer à tâche 4.
-  Réception et affichage de la structure. 
- Réception de la structure et conversion puis affichage de la valeur du temps en minutes.

Le but est de faire passer les données d'une tâche à une autre via les files d'attente "Queues" tout en controlant le partage des ressources "port série" en utilisant un sémaphore.


# Simulation
Aprés la compilation de code sur le logiciel d'Arduino, une simulation doit être fait pour tester le bon fonctionnement du code. 
Wokwi simulateur se présente comme une solution pertinente. 
[![N|Solid](https://www.mouser.fr/images/suppliers/logos/wokwi.png)]()
Sur la platforme , un prototypage du circuit réel est fait. Et sur lequel le code va être testé. 

> La carte utilisée est l'Arduino Mega 
L'entrée de potentiométre est relié à l'entrée analogique A0.
L'entrées des boutons poussoirs sur les pins digitales 2 et 3 . Les boutons poussoirs sont cablés en pull down.


[![N|Solid](https://s3-eu-west-1.amazonaws.com/sdz-upload/prod/upload/boutonPullDown.png)]()

Ce circuit alors a été réalisé pour tester le bon fonctionnement du programme : 
![Circuit.png]



# Dépendance requise
La bibliothéque freeRTOS doit être installée dans le logiciel Arduino et dans le simulateur afin de bien compiler le code.
[![N|Solid](https://hackster.imgix.net/uploads/image/file/128501/screenshot-from-2016-03-01-110338.png?auto=compress%2Cformat&w=1280&h=960&fit=max)]()
Puis on l'appelle on insérant `--#include <Arduino_FreeRTOS.h>` au début du code.

## Démarche 
Afin de mettre en place le programme demandé, on commence par les librairies pour assurer le lancement de FreeRtos et l'utilisation de sémaphore mutex.
```sh
#include <Arduino_FreeRTOS.h>                  
#include <semphr.h>   
```
Deuxièmement, il faut faire l'initialisation et la déclaration des variables ainsi que la création des tâches , le sémaphore et les files d'attente.
```sh
const int Button1 = 2 ; 
```
```sh
int ValPotentiometer = 0;
int ValButtons = 0;
```
```sh
struct valeurCapteursInStructure {               
int analogique;                                  
int numerique;
double tempsEnMillisecondes;
};
```
```sh
xTaskCreate(FirstTask, "Buttons's value Task", 100, NULL, 1, NULL );       
ValeurBoutonsQueue = xQueueCreate(5,sizeof(int32_t));
xSerialSemaphore = xSemaphoreCreateMutex()
```

En troisième lieu, on a écrit les tâches une par une en testant à chaque fois le bon fonctionnement de chacune individuellement. 
La première tâche permet de lire la valeur de potentiometre et l'envoyer à la troisième tâche .
```sh
ValPotentiometer = analogRead(Potentiometer);
qStatus1 =xQueueSend(ValeursPotentiometreQueue,&ValPotentiometer,0);
```
La deuxième tâche permet de lire les valeurs des boutons et faire la somme des deux puis les envoyer à la troisième tâche.
```sh
ValButtons = digitalRead(Button1)+digitalRead(Button2);  
qStatus2 =xQueueSend(ValeurBoutonsQueue,&ValButtons,0);
```
La troisième tâche reçoit les valeurs des tâches 1 et 2 et les met dans une structure. Puis elle envoie la structure à la tâche 4. Il faut bien prendre en compte l'utilisation d'un sémaphore dans cette partie et la partie qui suit pour protèger le port série. La fonction millis() est aussi utilisé pour déterminer le temps par rapport l'activation de la carte.
```sh
S1.analogique = ValPotentiometer;                                                    S1.numerique = ValButtons;
S1.tempsEnMillisecondes = millis();
xQueueSend(StructureQueue,&S1,0); 
```
La quatrième tâche, quant à elle, fait l'affichage de la structure faite à la tâche 3 et envoie cettre dernière à la cinquème tâche.
```sh
xQueueReceive(StructureQueue, &S1,0); 
   if ( xSemaphoreTake( xSerialSemaphore, ( TickType_t ) 5 ) == pdTRUE )                 
    {
         Serial.print("La valeur de potentiometre est : ");
         Serial.println(S1.analogique);                                                 
         Serial.print("La valeur réultante des boutons est : ");
         Serial.println(S1.numerique);                                                      
         Serial.print("Le temps en millisecondes dés l'activation de la carte est : ");
         Serial.println(S1.tempsEnMillisecondes);                                       
         xSemaphoreGive( xSerialSemaphore ); 
    }
    xQueueSend(NewStructureQueue, &S1, 0);                                                  
```

La cinquéme et la dernière tâche convertit de le temps en milliSecondes en Minutes en la mutlipliant par 0.0000167 c'est la meme chose que diviser par 1000 et 60 .  
1 Millisecondes = 1.7×10-5 Minutes
```sh
tempsEnMinutes = (S1.tempsEnMillisecondes) * 0.0000167;                                     xQueueReceive(NewStructureQueue, &S1, 0);  
Serial.print(tempsEnMinutes);
```
## Résultat finale
Le résultat final est alors sous la forme suivante: 
![Résultat.png]



