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
Microscopiquement, le trait de crayon est un agglomérat de feuillets de graphite, à cette échelle. Dans ce milieu granulaire complexe, la conduction se fait principalement par effet tunnel : les nombreux grains sont séparés par de très fins interstices (le vide ou des fibres de papier).
La probabilité de passage des électrons d'un grain de graphite à l'autre par franchissement de l'interstice est non nulle si cette barrière est suffisament fine. Cette probabilité dépend exponentiellement de la distance d qui sépare les deux grains.
Ce paramètre physique est alors à l'origine de la variation de résistance : en tension, l'écart d entre les particules augmente, la probabilité de passage est plus faible, la résistance globale du capteur augmente. 
Tandis qu'en compression, la distance d diminue, les particules se rapprochent, la probabilité d'échange augmente, la résistance chute. 
La dépendance exponentielle de la résistance à la distance d explique pourquoi une très faible variation du capteur va nous permettre de lire des varirations de signal (de résistance) importantes. 

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Pour passer d'un simple trait de crayon à un instrument de mesure fiable, le projet a été découpé en plusieurs phases d'ingénierie :
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

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Le capteur en graphite présente une résistance extrêmement élevée (de l'ordre du GΩ), ce qui génère des courants infimes (pico- à nanoampères) sous une tension de 5 V. Pour transformer ce signal en une tension exploitable par l’ADC d'une Arduino UNO (0−5V), nous avons conçu un amplificateur transimpédance.
PHOTO
Nous avons utilisé l’AOP LTC1050 car il possède un courant de biais d'entrée extrêmement faible et un offset quasi nul, évitant ainsi de fausser les mesures de courants très faibles. 
Nous avons également placé une résistance de protection (R5) en entrée pour protéger l'AOP contre les décharges électrostatiques, une résistance du shunt (R1) assure la référence à la masse. La résistance de rétroaction (R2) a été subsitué par un potentiomètre digital MCP41050 pour ajuster dynamiquement le gain du montage via le code Arduino. 

## Modélisation et simulation 

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;Avant la fabrication, l'ensemble du circuit a été modélisé sous LTSpice pour vérifier que l'amplification convertit bien les nanoampères en une tension comprise entre 0 et 5V. Cette modélisation a également permis de valider l'efficacité de nos trois étage de filtrage :  
- Filtre passe-bas (16 Hz) : élimination des hautes fréquences parasites.
- Filtre passe-bas (1,6 Hz) : atténuation drastique du bruit secteur (50 Hz)
- Filtre passe-bas (1,6 kHz) : supression des interférences liées à la communication de l'ADC.
PHOTO
## Conception 
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;La phase de conception a été réalisée sur Kicad. L'ojectif était de créer un Shield compatible avec le format de l'Arduino UNO. 
Cela nous demandé trois étapes : 
- Saisie des empreintes et des shématics associés pour nos différents composants : AOP, potentiomètre digital, écran OLED, encodeur rotatoire et module bluetooth. 
- Le routage du PCB : optimisation du placement des composants pour réduire les longueurs de pistes et visualisation 3D des composants pour vérifier l'abscence de conflits mécaniques entre tous les composants.
- Ajout d'un plan de masse pour minimiser le bruit électromagnétique et stabiliser les mesures de hautes impédances.
PHOTO
## Fabrication 
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;La réalisation du prototype a été faite par ce processus de fabrication : 
- Insolation & gravure (réalisé par Catherine Crouzet) : transfert du masque de gravure sur une plauqe époxy et passage au perchlorure de fer.
- Nettoyage (réalisé par Catherine Crouzet) : Retrait de la résine photosensible à l'acétone pour révéler les pistes de cuivre.
- Percage & soudure : montage manuel des composants sur le PCB final.
- Assemblage : soudure des composants. 

## Développemebt Firmaware et Software 

Le système est piloté par deux pôles : 
- Firmware (Arduino) : utilisation des bibliothèques *Adafruit_SSD1306* (OLED), *SPI* (potentiomètre) et *SoftwareSerial* (Bluetooth). Le code génère un menu intéractif via l'encodeur rotatoire pour calibrer le capteur et lancer les mesures.
- Interface mobile : développement d'une application Android sur MIT App Inventor qui communique en Bluetooth pour afficher la résistance en temps réel et tracer la courbe de déformation directement sur le smartphone.
PHOTO
## Analyse et caractérisation

## Datasheet

## Conclusion 
