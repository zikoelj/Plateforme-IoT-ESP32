// Bibliothèques nécessaires
#include <Adafruit_Sensor.h>
#include <DHT_U.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <ESP32Servo.h> // Bibliothèque pour contrôler le servo-moteur
#include <FastLED.h> // Bibliothèque pour contrôler la bande LED WS2812

// Définition des broches
#define DHTPIN 12 // Broche du capteur DHT22
#define LED 26 // LED simple
#define SERVO_PIN 2 // Broche du servo-moteur
#define LED_PIN 4 // Broche de la bande LED WS2812
#define NUM_LEDS 16 // Nombre de LEDs dans la bande

// Paramètres du DHT
#define DHTTYPE DHT22 // Type de capteur (DHT22)
DHT_Unified dht(DHTPIN, DHTTYPE);
uint32_t delayMS;

// Objet pour contrôler le servo-moteur
Servo servo;

// Tableau pour contrôler les LEDs WS2812
CRGB leds[NUM_LEDS];

// Identifiants MQTT
const char* ssid = "Wokwi-GUEST"; // Nom du réseau WiFi
const char* password = ""; // Mot de passe WiFi
const char* mqttServer = "broker.hivemq.com"; // Serveur MQTT
const char* clientID = "ujaisldaaasdfgh;laslksdja1"; // ID unique du client
const char* topic = "Tempdata"; // Sujet pour publier les données

// Variables pour le délai non-bloquant
unsigned long previousMillis = 0;
const long interval = 1000; // Intervalle de 1 seconde
String msgStr = ""; // Buffer pour les messages MQTT
float temp, hum; // Variables pour température et humidité

// Initialisation des clients WiFi et MQTT
WiFiClient espClient;
PubSubClient client(espClient);

// Fonction de connexion au WiFi
void setup_wifi() {
  delay(10);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connecté");
  Serial.println("Adresse IP : ");
  Serial.println(WiFi.localIP());
}

// Fonction de reconnexion MQTT
void reconnect() {
  while (!client.connected()) {
    if (client.connect(clientID)) {
      Serial.println("MQTT connecté");
      client.subscribe("lights"); // S'abonner au sujet pour la LED simple
      client.subscribe("servo"); // S'abonner au sujet pour le servo-moteur
      client.subscribe("lights/neopixel"); // S'abonner au sujet pour les LEDs NeoPixel
      Serial.println("Sujets souscrits");
    }
    else {
      Serial.print("Échec, rc=");
      Serial.print(client.state());
      Serial.println(" Nouvelle tentative dans 5 secondes");
      delay(5000); // Attendre 5 secondes avant de réessayer
    }
  }
}

// Fonction de rappel pour les messages MQTT reçus
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message reçu sur le sujet : ");
  Serial.println(topic);
  Serial.print("Message : ");
  String data = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    data += (char)payload[i];
  }
  Serial.println();
  Serial.print("Taille du message : ");
  Serial.println(length);
  Serial.println();
  Serial.println("-----------------------");
  Serial.println(data);

  // Contrôle de la LED simple
  if (String(topic) == "lights") {
    if (data == "ON") {
      Serial.println("LED allumée");
      digitalWrite(LED, HIGH);
    }
    else {
      digitalWrite(LED, LOW);
    }
  }
  // Contrôle du servo-moteur
  else if (String(topic) == "servo") {
    int degree = data.toInt(); // Convertir les données en entier
    Serial.print("Déplacement du servo à l'angle : ");
    Serial.println(degree);
    servo.write(degree); // Déplacer le servo à l'angle spécifié
  }
  // Contrôle des LEDs NeoPixel
  else if (String(topic) == "lights/neopixel") {
    int red, green, blue;
    sscanf(data.c_str(), "%d,%d,%d", &red, &green, &blue); // Parser les valeurs RGB
    Serial.print("Réglage des NeoPixels à (R,V,B) : ");
    Serial.print(red);
    Serial.print(",");
    Serial.print(green);
    Serial.print(",");
    Serial.println(blue);
    fill_solid(leds, NUM_LEDS, CRGB(red, green, blue)); // Appliquer la couleur à toutes les LEDs
    FastLED.show(); // Mettre à jour la bande LED
  }
}

// Fonction d'initialisation
void setup() {
  Serial.begin(115200);
  
  // Initialiser le capteur DHT
  dht.begin();
  
  // Obtenir les informations du capteur
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  dht.humidity().getSensor(&sensor);
  
  // Configurer la LED simple
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  // Configurer le servo-moteur
  servo.attach(SERVO_PIN, 500, 2400);
  servo.write(0); // Position initiale à 0°

  // Configurer la bande LED WS2812
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

  // Établir les connexions
  setup_wifi();
  client.setServer(mqttServer, 1883); // Définir le serveur MQTT
  client.setCallback(callback); // Définir la fonction de rappel
}

// Boucle principale
void loop() {
  // Vérifier la connexion MQTT
  if (!client.connected()) {
    reconnect(); // Se reconnecter si nécessaire
  }
  client.loop();
  
  // Lecture périodique des capteurs (toutes les secondes)
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    
    // Lire la température
    sensors_event_t event;
    dht.temperature().getEvent(&event);
    if (isnan(event.temperature)) {
      Serial.println(F("Erreur de lecture de la température !"));
    }
    else {
      Serial.print(F("Température : "));
      temp = event.temperature;
      Serial.print(temp);
      Serial.println(F("°C"));
    }
    
    // Lire l'humidité
    dht.humidity().getEvent(&event);
    if (isnan(event.relative_humidity)) {
      Serial.println(F("Erreur de lecture de l'humidité !"));
    }
    else {
      Serial.print(F("Humidité : "));
      hum = event.relative_humidity;
      Serial.print(hum);
      Serial.println(F("%"));
    }
    
    // Préparer et publier le message MQTT
    msgStr = String(temp) + "," + String(hum) + ",";
    byte arrSize = msgStr.length() + 1;
    char msg[arrSize];
    Serial.print("DONNÉES PUBLIÉES : ");
    Serial.println(msgStr);
    msgStr.toCharArray(msg, arrSize);
    client.publish(topic, msg);
    msgStr = "";
    delay(1);
  }
}