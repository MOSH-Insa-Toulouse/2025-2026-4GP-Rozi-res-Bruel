# 2025-2026-4CP - Projet de capteur low-tech en graphite - Marilou Rozières & Adélaïs Bruel 

## SOMMAIRE
*** 
  - [I. Contexte et description du projet](#contexte-et-description-du)
  - [II. Livrables](#livrables)
  - [III. Matériel nécessaire](#matériel-nécessaire)
  - [IV. Conditionnement analogique](#conditionnement-analogique)
  - [V. Modélisaiotn et simulation](#modélisaiton-et-simulation)
  - [VI. Conception](#conception)
  - [VII. Fabrication](#fabrication)
  - [VIII. Développement firmware et software](#développement-firmware-et-software)
  - [IX. Analyse et caractérisation](#analyse-et-caractérisation)
  - [X. Datasheet](#datasheet)
  - [XI. Conclusion](#conclusion)

*** 

## Contexte et description du projet 

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Ce projet s'inscrit dans le cadre de l'UF "Du capteur au banc de test" de la 4ème année Génie Physique de l'INSA Toulouse. 
L'objectif de ce projet est de concevoir, modéliser, prototyper, caractériser et analyser un capteur de déformation low-tech.

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Le principe de foncitonnement du capteur repose sur la piézorésistivité d'un dépôt de graphite (réalisé à l'aide d'un simple crayon de bois) sur un support flexible (une feuille de papier). 
Microscopiquement, le trait de crayon est un agglomérat de feuillets de graphite. Dans ce milieu granulaire complexe, la conduction se fait principalement par effet tunnel : les nombreux grains sont séparés par de très fins interstices, la probabilité de passage des électrons d'un grain de graphite à l'autre par franchissement de l'interstice est non nulle si cette barrière est suffisament fine. Cette probabilité dépend exponentiellement de la distance *d* qui sépare les deux grains.
Ce paramètre physique est alors à l'origine de la variation de résistance : en tension, l'écart *d* entre les particules augmente, la probabilité de passage est plus faible, la résistance globale du capteur augmente. 
Tandis qu'en compression, la distance *d* diminue, les particules se rapprochent, la probabilité d'échange augmente, la résistance chute. 
La dépendance exponentielle de la résistance à la distance *d* explique pourquoi une très faible variation du capteur va nous permettre de lire des varirations de signal (résistance) importantes. 

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Pour passer d'un trait de crayon à un capteur fonctionnel, le projet s'est déroulé selon ces différentes phases :
- **Conditionnement analogique** : Conception d'un amplificateur transimpédance (conversion courant-tension) pour transformer les variations de courant (de l'ordre du nA) en un signal de tension exploitable (0−5V).
- **Modélisation et Simulation** : Utilisation de LTSpice pour dimensionner les filtres actifs et passifs afin d'éliminer le bruit parasite (notamment le 50 Hz du secteur).
- **Conception** : Routage d'un circuit imprimé (Shield PCB) sous KiCad pour intégrer l'électronique, l'écran OLED et le module Bluetooth.
- **Fabrication** : Réalisation physique du PCB par insolation, gravure chimique (perchlorure de fer), perçage et soudure des composants.
- **Développement Firmware & Software** : Programmation de la carte Arduino (gestion de l'I2C, SPI et UART) et création d'une interface mobile sous MIT App Inventor.
- **Analyse** : Caractérisation du capteur sur un banc de test dédié, comparaison avec un capteur commercial (Flex Sensor) et rédaction d'une datasheet technique.

Ce document a pour objectif d'expliciter ces différentes étapes.  

## Livrables 
Les livrables attendus sont les suivants : 
- **Un shield PCB connecté à une carte arduino UNO** avec différents composants : un capteur graphite, un amplificateur transimpédance, un module bluetooth, un écran OLED, un flex sensor commercial, un potentiomètre digital, un encodeur rotatoire. D'autres composants peuvent être implémentés. Nous avons opté pour l'ajout d'un servo motor ;
- **Un code arduino** qui gère les différents composants cités précédemments (mesures de contraintes, échanges bluetooth et OLED, potentiomètre digital et boutons) ;
- **Une application Android** (sous MIT App Inventor) interfaçant le PCB et le code Arduino correspondant;
- **Une datasheet du capteur** reprenant toutes ses caractéristiques ainsi que ses tests.

## Matériel nécessaire

Afin de réaliser notre dispositif électronique, nous avons eu besoin de :

**Pour le montage amplificateur transimpédance :**

- Résistances : une de 1 kΩ, une de 10 kΩ et deux de 100 kΩ - une troisième de 100 kΩ peut être prévue, mais peut aussi être substituée par une résistance variable (le potentiomètre digital), choix que l'on a fait ;
- Potentiomètre digital MCP41050 et son support ;
- Amplificateur opérationnel LTC1050 et son support ;
- Capacités : trois de 100 nF et une de 1 μF.

**Pour le reste du dispositif :**

- Arduino Uno et son câble d’alimentation ;
- Résistance : une de 47kΩ pour le flex sensor ;
- Module Bluetooth HC05 ;
- Ecran OLED de dimension 128*64 ;
- Flex sensor ;
- Servo motor ;
- Encodeur rotatoire ;
- Capteur graphite et 2 pinces crocodiles ;
- 20 sockets ;
- 35 headers.

## Conditionnement analogique 

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Le capteur en graphite présente une résistance extrêmement élevée (de l'ordre du GΩ), ce qui génère des courants infimes (pico- à nanoampères) sous une tension de 5 V. Pour transformer ce signal en une tension exploitable par l’ADC d'une Arduino UNO (0−5V), nous avons conçu un amplificateur transimpédance, réalisé comme suit : 

<img width="347" height="198" alt="Capture d’écran 2026-03-27 à 16 34 57" src="https://github.com/user-attachments/assets/8cea60ea-ad1a-4f46-b7bb-b49aaf62c9ab" />


Nous avons utilisé l’AOP LTC1050 car il possède un courant de biais d'entrée extrêmement faible et un offset quasi nul, évitant ainsi de fausser les mesures de courants très faibles. 
Nous avons placé une résistance de protection (R5) en entrée pour protéger l'AOP contre les décharges électrostatiques, ainsi qu'une résistance du shunt (R1) qui assure la référence à la masse. La résistance de rétroaction (R2) a été subsitué par un potentiomètre digital MCP41050 pour ajuster dynamiquement le gain du montage via le code Arduino. 
Les trois étages de filtrages de cet amplificateur permet de minimiser au maximum les bruits parasites. 

## Modélisation et simulation 

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Avant la fabrication, l'ensemble du circuit a été modélisé sous LTSpice pour vérifier que l'amplification convertit bien les nanoampères en une tension comprise entre 0 et 5V. Cette modélisation a également permis de valider l'efficacité de nos trois étage de filtrage :  
- Filtre passe-bas (16 Hz) : élimination des hautes fréquences parasites.
- Filtre passe-bas (1,6 Hz) : atténuation drastique du bruit secteur (50 Hz)
- Filtre passe-bas (1,6 kHz) : supression des interférences liées à la communication de l'ADC.

<img width="694" height="231" alt="Schema du montage transimpedance" src="https://github.com/user-attachments/assets/3a0ec1fb-3746-4982-9466-483922bc05db" />

## Conception 
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;La phase de conception a été réalisée sur Kicad. L'ojectif était de créer un Shield compatible avec le format de l'Arduino UNO. 
Cela nous demandé trois étapes : 
- Saisie des empreintes et des shématics associés pour nos différents composants : AOP, potentiomètre digital, écran OLED, encodeur rotatoire et module bluetooth. 
- Le routage du PCB : optimisation du placement des composants pour réduire les longueurs de pistes et visualisation 3D des composants pour vérifier l'abscence de conflits mécaniques entre tous les composants.
- Ajout d'un plan de masse pour minimiser le bruit électromagnétique et stabiliser les mesures de hautes impédances.
<img width="428" height="329" alt="pcb" src="https://github.com/user-attachments/assets/ee6a4bc2-b292-41ad-94a5-3cf82ca331bd" />

<img width="581" height="400" alt="sch" src="https://github.com/user-attachments/assets/58cea0d3-3633-4d91-bc43-700db6f227c7" />



## Fabrication 
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;La réalisation du prototype a été faite par ce processus de fabrication : 
- Insolation & gravure (réalisé par Catherine Crouzet) : transfert du masque de gravure sur une plauqe époxy et passage au perchlorure de fer.
- Nettoyage (réalisé par Catherine Crouzet) : Retrait de la résine photosensible à l'acétone pour révéler les pistes de cuivre.
- Percage & soudure : montage manuel des composants sur le PCB final.
- Assemblage : soudure des composants. 

## Développement Firmaware et Software 

Le système est piloté par deux pôles : 
- Firmware (Arduino) : utilisation des bibliothèques *Adafruit_SSD1306* (OLED), *SPI* (potentiomètre) et *SoftwareSerial* (Bluetooth). Le code génère un menu intéractif via l'encodeur rotatoire pour calibrer le capteur et lancer les mesures.
- Interface mobile : développement d'une application Android sur MIT App Inventor qui communique en Bluetooth pour afficher la résistance en temps réel et tracer la courbe de déformation directement sur le smartphone.
PHOTO

## Analyse et caractérisation

**Dispositif de mesure :** 

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Afin d’obtenir les caractéristiques spécifiques du capteur, nous avons utilisé un banc de test composé de six cylindres de diamètre différent, permettant d’étudier la résistance de notre capteur en tension et en compression pour six valeurs de contraintes connues.  
<img width="581" height="400" alt="sch" src="https://github.com/user-attachments/assets/ebb6174a-f0e7-4301-bc09-99fcc8dfd65a"/>

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Nous avons choisi d’étudier trios types de crayons à papier différent : le B, le 3B et le 6B. Nous avons donc conçu trois capteurs différents, et avons déterminé pour chacun d’entre eux douze valeurs : six en compression et six en tension. Ces mesures nous ont permises d’établir la variation relative de résistance en fonction de la déformation grâce aux formules suivantes : 


Calcul de la déformation ∶ ε=  e/D


Avec e l’épaisseur du papier utilisé pour le capteur et D le diamètre du cylindre sur lequel est effectué la mesure. 


Calcul de la variation relative de résistance∶ ∆R/R0 (%)=  (R_mesuré-R0)/R0

**Résultats des mesures :**

En tension, nous obtenons alors la variation relative de résistance en fonction de la déformation suivante : 











En compression, nous obtenons la variation relative de la résistance en fonction de la déformation suivante : 









&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Les allures des courbes sont pertinentes : les crayons à papier sont constitués de graphite et d’un liant isolant (argile). Afin de comprendre la différence entre les différents types de crayons (B, 3B, 6B, …) il faut se pencher sur le seuil de percolation de ceux-ci. Le seuil de percolation représente la fraction volumique nécessaire de charge conductrice (ici le graphite) afin que celles-ci forment un réseau continu interconnecté à travers la matrice isolante (l’argile). A partir de ce seuil de matière conductrice, les propriétés macroscopiques du matériau basculent d’un comportement isolant à un comportement conducteur. Un crayon 6B est saturé en graphite et contient très peu d’argile (seuil de percolation largement dépassé), il représente alors un milieu très conducteur. Quand une déformation est appliquée au capteur, des micro fissures sont créées, mais il existe une multitude de chemins secondaires conducteurs, la résistance globale du capteur n’est que très peu impactée. A contrario, le crayon B a une mine dure, il contient une forte proportion d’argile (très proche du seuil de percolation), les grains de graphite sont plus dispersés dans la matrice d’argile isolante, lorsqu’une déformation est appliquée, les rares chemins électriques entre les grains de graphite se brise très facilement, la résistance varie alors fortement. 

**Analyse :**

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;La sensibilité (facteur de jauge k) est définie par : k=  (∆R/R0)/ε
Le crayon B possède la meilleure sensibilité puisque pour une déformation donnée il génère une plus grande variation de résistance. Diminuer la concentration de charge conductrice (en passant de 6B à B) augmente alors fortement la piézorésistivité du matériau. 
Pour conclure, pour une application requérant une grande sensibilité un capteur réalisé avec un crayon de type B est le mieux, néanmoins celui-ci aura tendance à « s’user » plus vite puisque que sa composition est au niveau du seuil de percolation. Tandis que pour une application nécessitant moins de précision mais plus de robustesse, un crayon très gras tel que le 6B conviendra mieux, puisque qu’étant très loin du seuil de percolation il peut être déformé plus de fois avant de ne plus avoir chemins conducteurs possible entre les particules.  

## Datasheet

## Conclusion 
