# Cassiopee
TM4C1294 Gateway



## CR 07-08 (26/03-08/04)

Pas d'avancement car la semaine de Projet Challenge d'Endreprendre et les CFs qui suivent.



## CR 06 (19/03-25/03)

#### Tâches Réalisées

- Partie Hardware **BLOQUE**
  - Serveur + client nécessite plusieurs Threads, difficile à réaliser en utilisant LWIP RAW
  - Manque de tutoriaux de la part de RTOS

#### Tâches à venir

- Partie Hardware : Trouver une solution...



## CR 05 (12/03-18/03)

#### Tâches Réalisées

- Soumission du Livrable 1
- Etablissement d'un serveur TCP sur la carte TIVA

#### Tâches à venir

- Voir la possibilité d'établir plusieurs connexions (serveur+client)



## CR 04 (05/03-11/03)

#### Tâches Réalisées

- Finalisation de la partie Bluetooth


- Présentation de la liaison Bluetooth

#### Tâches à venir

- Revoir les livrables et les envoyer
- Etablir un seveur TCP sur la carte TIVA



## CR 03 (19/02-04/03)

#### Tâches Réalisées
- Réalisation de la liaison Bluetooth
- Enregistration des données dans EEPROM
- Echange des données via TCP entre le portable(comme le client) et le PC


#### Tâches à venir
- Echange des données via TCP sur la carte
- Amélioration de Layout d'app
- Etude de possibilité d'obtenir les données via HTTP



## CR 02 (05/02-18/02)

#### Tâches Réalisées

- Essaie avec le nouveau module HC05
- Etude du protocole LWIP
- Présentation d'un exemple sur LWIP
- Etude du protocole TCP
- Etude d'utilisation des sockets
- Début de programmation
- Planning provisoire (Livrable1)
- Description du projet (Livrable2)
- Mini compte-rendu (Livrable3)



#### Tâches à venir (19/02 - 04/03)

Muyao CHEN

- Etudier le mode du stockage des données
- Continuer à étudier LWIP par un exemple plus complexe
- Terminer la partie de Bluetooth

Jing YE
- Communication avec un Arduino par TCP/UDP
- Etude de la réalisation de HTTP sur Android


## CR 01 (01/02 - 04/02)

#### Tâches Réalisées

- Manipulation du module HC05 Bluetooth via UART 
- Etude des différentes possiblilités pour faire ce projet (avec RTOS ou sans RTOS)
- Apprentisage du fonctionnement de LwIP
- Pré-définition du résultat attendu de l'Appli Android
  - Consultation de données sur les conditions mesurés par les capteurs : Température, humidité, etc. Deux formes : Diagramme, chiffre.
  - Contrôle d’interrupteur de lumières : ON et OFF
  - Alerte quand un état dépasse la limite : Par la notification de l’application
  - Addition et suppression de périphériques : Bluetooth ou Internet  
- Etude de la possibilité de faire l'interface WEB 
- Première étude de mode de communication entre le matériel et l’application

#### Tâches à venir

Pour la semaine prochaine, nous aurons les tâches ci-dessuous : 

Muyao Chen : 

- Faire une exposé sur LwIP 	9/2 Vendredi (à fixer)
- Etudier le module ESP8266 Résultat attendu : Réaliser la communication via ESP8266 et un autre terminal (ex. le smartphone)    9/2 Vendredi

Jing Ye : 

- Réaliser de connexion de protocole TCP sur la plateforme Android

Ensemble :

- Définir le planning provisoire	9/2 Vendredi
- Rédiger le Cahier des charges    14/2 Mercredi prochain 

#### Difficultés

Bluetooth : HC05 on/off

WiFi : ESP8266

RTOS : Commence par sans RTOS, Apprendre RTOS au fil du temps 



## 12/1/2018

### Planning V2

**Etape 1** : Jouer avec la carte ()

1. La liaison **Bluetooth** entre la carte et un autre terminal Bluetooth (ex. une carte Arduino) (~19/1/2018)
2. Comprendre la structure du système **TI RTOS (SYS/BIOS)**  (~31/1/2018)
3. Comprendre finalement **l'exemple TCP**. (~31/1/2018)
4. Intégrer la communication **UDP** et sur l'exemple TCP pour evaluer la maîtrise de **SYS/BIOS** (9/2/2018)
5. Ecrire un **Cahier des charges** du projet (Version1 avant 9/2/2018) le **Deadline**(1/3/2018)

**Etape 2** : Intégration de La liaison Bluetooth

**Etape 3** : Algorithme pour finaliser la contrôle du terminal Bluetooth par l'internet

**Etape 4** : Amélioration de l'algorithme pour atteindre l'objectif et contrôler plusieurs terminaux

**Bonus 1** : Appli Android pour visualiser tous les fonctions mises en place



## 4/1/2018

### Planning
**Etape 1** : La liaison TCP et UDP entre la carte ARM et un autre terminal (avec fil ou sans fil)

**Etape 2** : La liaison Bluetooth entre la carte et un autre terminal Bluetooth (ex. une carte Arduino)

**Etape 3** : Algorithme pour finaliser la contrôle du terminal Bluetooth par l'internet

**Etape 4** : Amélioration de l'algorithme pour atteindre l'objectif et contrôler plusieurs terminaux

**Bonus 1** : Appli Android pour visualiser tous les fonctions mises en place
