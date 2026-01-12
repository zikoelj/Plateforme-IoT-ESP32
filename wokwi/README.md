# **Projet ESP32 : Station Météo Connectée avec Contrôle MQTT**

## 📋 Description du Projet
Ce programme permet à un ESP32 de collecter des données environnementales (température et humidité) et de les publier sur un serveur MQTT tout en permettant le contrôle à distance de périphériques connectés.

## 🎯 Fonctionnalités Principales

### 1. **Collecte de Données Environnementales**
- Mesure de température avec un capteur DHT22
- Mesure d'humidité ambiante
- Publication automatique toutes les secondes

### 2. **Contrôle à Distance via MQTT**
- **LED simple** : Allumage/extinction
- **Servo-moteur** : Contrôle de position (0-180°)
- **Bande LED RGB (WS2812)** : Changement de couleur

### 3. **Communication Sans Fil**
- Connexion WiFi automatique
- Communication MQTT avec broker public
- Reconnexion automatique en cas de perte

## 🛠️ Matériel Requis

### Composants :
- **ESP32** (carte de développement)
- **Capteur DHT22** (température/humidité)
- **LED simple** (avec résistance)
- **Servo-moteur** (standard 9g)
- **Bande LED WS2812** (16 LEDs)
- Câbles de connexion

### Brochage :
- **DHT22 → GPIO12**
- **LED → GPIO26**
- **Servo → GPIO2**
- **WS2812 → GPIO4**

## 📡 Configuration MQTT

### Serveur utilisé :
- **Broker** : broker.hivemq.com (public)
- **Port** : 1883
- **Client ID** : unique (modifiable)

### Sujets MQTT :
- **Publication** : `Tempdata` → données capteurs
- **Souscription** : 
  - `lights` → contrôle LED simple
  - `servo` → contrôle servo (0-180)
  - `lights/neopixel` → contrôle LED RGB (format: "R,G,B")

## 🚀 Installation et Utilisation

### 1. Prérequis Logiciels
- Arduino IDE 1.8+ ou VS Code avec PlatformIO
- Bibliothèques Arduino (voir code source)

### 2. Configuration WiFi
```cpp
const char* ssid = "VOTRE_SSID";
const char* password = "VOTRE_MDP";
```
### 3. Téléversement

- Sélectionner la carte "ESP32 Dev Module"
- Configurer le port COM approprié
- Téléverser le code

## Exemples de Commandes MQTT
- Pour contrôler les périphériques :
### 1. LED simple :
```cpp
Sujet: lights
Message: ON  (allume)
Message: OFF (éteint)
```
### 2. Servo-moteur :
```cpp
Sujet: servo
Message: 90  (position à 90°)
```

### 3. LEDs RGB :
```cpp
Sujet: lights/neopixel
Message: 255,0,0  (rouge)
Message: 0,255,0  (vert)
Message: 0,0,255  (bleu)
```
## Problèmes fréquents :
- Pas de connexion WiFi → Vérifier SSID/mot de passe
- Données capteur invalides → Vérifier le câblage DHT22
- MQTT non connecté → Vérifier l'accès Internet
- LEDs non fonctionnelles → Vérifier l'alimentation 5V

## 📊 Format des Données Publiques
- Les données environnementales sont publiées au format :
```cpp
"25.50,60.30,"
```
**(Température, Humidité)**

## ⚠️ Notes Importantes

### Sécurité :
- Ce code utilise un broker MQTT public
- Pour une utilisation production, utiliser un broker privé
- Ajouter une authentification MQTT

### Performances :
- Intervalle de lecture : 1 seconde
- Buffer MQTT optimisé pour éviter les dépassements
- Gestion non-bloquante pour maintenir la réactivité

## 🔄 Améliorations Possibles
- Ajout d'un écran OLED pour affichage local
- Stockage des données en local (SD card)
- Interface web de contrôle
- Notifications par email/SMS