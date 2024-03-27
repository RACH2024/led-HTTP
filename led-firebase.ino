#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h" 

// Pin definitions
#define BUTTON_PIN 33    // Broche du bouton-poussoir
#define LED_PIN 22      // Broche de la LED

// Variable pour stocker l'état du bouton
int etatBouton;
// Variable pour stocker l'état précédent du bouton
int etatBoutonPrec = LOW;

// Insert your network credentials
#define WIFI_SSID "BOX4G_Inwi_513F" 
#define WIFI_PASSWORD  "55165756"

// Insert Firebase project API Key
#define API_KEY "AIzaSyCDR_1jfZQ2X1IXA9LyFv3EmNjLv_a8nEM"

// Insert RTDB URL
#define DATABASE_URL "https://led-esp32-ad3ec-default-rtdb.europe-west1.firebasedatabase.app/"

// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
bool signupOK = false;

void setup() {
  pinMode(BUTTON_PIN, INPUT);  // Configuration du bouton-poussoir en entrée avec résistance de pull-up interne
  pinMode(LED_PIN, OUTPUT);           // Configuration de la LED en sortie
  
  Serial.begin(115200);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  } else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback=tokenStatusCallback;  
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  /*// Lire l'état du bouton-poussoir
  bool buttonState = digitalRead(BUTTON_PIN);

  // Si le bouton est enfoncé
  if (buttonState == LOW) {
    // Allumer la LED
    digitalWrite(LED_PIN, HIGH);
  } else {
    // Éteindre la LED
    digitalWrite(LED_PIN, LOW);
  }*/

  /*Serial.println(digitalRead(BUTTON_PIN));
    // Si bouton poussoir appuyé...
    if (digitalRead(BUTTON_PIN) == 1) // teste si le bouton a une valeur de 1
        // ...on allume la LED
    {
        digitalWrite(LED_PIN, HIGH);  // allume la LED
    }
    // Sinon...
    else
        // teste si le bouton a une valeur de 0
        // ...on éteint la LED
    {
        digitalWrite(LED_PIN, LOW);  // éteint la LED
    }*/


  // Envoyer l'état de la LED à la base de données Firebase
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

     // Lit l'état du bouton
  etatBouton = digitalRead(BUTTON_PIN);
  Serial.println(etatBouton);

  // Vérifie si le bouton est enfoncé
  if (etatBouton == HIGH) {
   /* Serial.println(etatBouton);*/
    // Allume la LED
    digitalWrite(LED_PIN, HIGH);
  } else {
    // Éteint la LED si le bouton n'est pas enfoncé
    digitalWrite(LED_PIN, LOW);
  }

   
    /*// Si bouton poussoir appuyé...
    if (digitalRead(BUTTON_PIN) == 1) // teste si le bouton a une valeur de 1
        // ...on allume la LED
    {
        digitalWrite(LED_PIN, HIGH);  // allume la LED
    }
    // Sinon...
    else
        // teste si le bouton a une valeur de 0
        // ...on éteint la LED
    {
        digitalWrite(LED_PIN, LOW);  // éteint la LED
    }
    int ledState = digitalRead(LED_PIN); // Lire l'état de la LED
    Serial.println(digitalRead(BUTTON_PIN));*/

    // Convertir l'état de la LED en une chaîne de caractères*
    int ledState = digitalRead(LED_PIN); // Lire l'état de la LED
    String stateString = (ledState == HIGH) ? "On" : "Off";

    // Envoyer l'état de la LED à la base de données
    if (Firebase.RTDB.setString(&fbdo, "LED_State", stateString.c_str())) {
      Serial.print("LED State: ");
      Serial.println(stateString);
    } else {
      Serial.println("Failed to send LED state to Firebase");
      Serial.println("Reason: " + fbdo.errorReason());
    }
  }
}
