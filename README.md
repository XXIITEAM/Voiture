# Projet Ardroid

[![N|Solid](https://avatars2.githubusercontent.com/u/10786005?s=400&u=0123d791bd59fcae2d5bd748c409cf4f03ea49ee&v=0)]()

Ardroid est une solution de contrôle véhicule/smartphone.

# Fonctions 

  - Contrôle véhicule
  - Parametrage véhicule
  - FPV
  - Autonomie

### Ressources

Pour utiliser ce projet nous vous conseillons de suivre les liens ci-dessous (Pour les achats ces produits sont proposés à titre d'exemple, vous êtes libre d'acheter vos cartes/périphérique ou vous le voulez) :

Logiciels :
* [Site officiel Arduino](https://www.arduino.cc/) - Site Arduino
* [Arduino IDE](https://www.arduino.cc/en/main/software) - IDE développement Arduino
* [Visual Studio](https://visualstudio.microsoft.com/fr/thank-you-downloading-visual-studio/?sku=Community&rel=15) - Alternative à l'IDE Arduino (plus confortable)
* [Android Studio](https://developer.android.com/studio/) - IDE développement Android

Matériel :
* [Véhicule pré-assemblé 4WD Smart Car 02](https://www.cdiscount.com/informatique/composants-informatiques/yier-4wd-bluetooth-controle-smart-voiture-kit-pour/f-1071311-yie0799085469979.html#cm_rr=FP:7583423:SP:CAR) - Lien chassi véhicule (CDISCOUNT), d'autres chassis existent
* [Carte Arduino Mega-2560 rev3](https://store.arduino.cc/arduino-mega-2560-rev3) - Carte de remplacement (Besoin d'E/S supplémentaires pour ce projet)


### Utilisation

Vous devez donc disposer d'un véhicule 4WD et d'un smartphone Android.

Côté Arduino :
* Clonez le dépôt [Voiture](https://github.com/XXIITEAM/Voiture.git) ou Télechargez l'[archive](https://github.com/XXIITEAM/Voiture/archive/1.0.0.zip)
* Lancez le projet dans votre IDE (Visual studio la team XXII)
* Compilez et envoyez le projet dans votre carte Arduino

Pour Anrdroid :
* Clonez le dépôt [BTArdroid](https://github.com/XXIITEAM/BTArdroid.git) ou Télechargez l'[archive](https://github.com/XXIITEAM/BTArdroid/archive/1.0.0.zip)
* Lancer le projet dans votre IDE (Android Studio la team XXII)
* Compilez et installez l'application sur votre smartphone

### Informations techniques

Carte Arduino Entrées/Sorties

#---------------------------------#
Capteurs US
#---------------------------------#
Num : Trig : Echo : Pos
* 1 : 22 : 23 : AvG
* 2 : 24 : 25 : AvC
* 3 : 26 : 27 : AvD
* 4 : 28 : 29 : ArD
* 5 : 30 : 31 : ArC
* 6 : 32 : 33 : ArG

#---------------------------------#
Strategie US
#---------------------------------#

* MAV : 1 2 3 Scan
* MAR : 4 5 6 Scan

#---------------Lv3---------------#

* 1 : Av + D
* 2 : Av + G/D
* 3 : Av + G
* 4 : Ar + G
* 5 : Ar + G/D
* 6 : Ar + D
* 
* 1+2 : S + D
* 1+3 : Continue
* 2+3 : S + G

* 1+2+3 : Inv Mvt

* 4+5 : S+G
* 4+6 : Continue
* 5+6 : S+D

* 4+5+6 : Inv Mvt

* Av+Ar : G/D 1 tour

#---------------Lv2---------------#

* 1 : S + D
* 2 : S + G/D
* 3 : S + G
* 4 : S + D
* 5 : S + G/D
* 6 : S + G

* 2x Idem Lv3

#---------------Lv1---------------#

* ALL : Inv Mvt
