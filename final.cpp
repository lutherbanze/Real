#include <WiFi.h>
#include <Firebase_ESP_Client.h>

#define WIFI_SSID "Allrah"
#define WIFI_PASSWORD "1234566780"

#define API_KEY "AIzaSyAQCTdid6Ya9eOjkY4u8_HKPxqjJuADzXM"
#define DATABASE_URL "https://allrah-default-rtdb.firebaseio.com/"
#define USER_EMAIL "lutherbanze@gmail.com"
#define USER_PASSWORD "dfs123"

#define BUZZER_PIN 47

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

int leds[] = {10, 11, 12, 13, 14, 21};
const int totalLeds = sizeof(leds) / sizeof(leds[0]);

unsigned long previousMillis = 0;
bool toggleState = false;

// ---------- FUNÇÕES BASE ----------
void allOff() {
  for (int i = 0; i < totalLeds; i++) {
    digitalWrite(leds[i], LOW);
  }
  noTone(BUZZER_PIN);
}

void allOn() {
  for (int i = 0; i < totalLeds; i++) {
    digitalWrite(leds[i], HIGH);
  }
  tone(BUZZER_PIN, 1000);
}

// ---------- ALARME INTERMITENTE ----------
void alarmToggle(unsigned long interval) {
  if (millis() - previousMillis >= interval) {
    previousMillis = millis();
    toggleState = !toggleState;

    for (int i = 0; i < totalLeds; i++) {
      digitalWrite(leds[i], toggleState ? HIGH : LOW);
    }

    if (toggleState) {
      tone(BUZZER_PIN, 1000);
    } else {
      noTone(BUZZER_PIN);
    }
  }
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < totalLeds; i++) {
    pinMode(leds[i], OUTPUT);
    digitalWrite(leds[i], LOW);
  }

  pinMode(BUZZER_PIN, OUTPUT);
  noTone(BUZZER_PIN);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
  }

  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;

  Firebase.reconnectNetwork(true);
  Firebase.begin(&config, &auth);
}

void loop() {
  if (Firebase.ready() && millis() - sendDataPrevMillis > 300) {
    sendDataPrevMillis = millis();

    int state;
    if (Firebase.RTDB.getInt(&fbdo, "/led/state", &state)) {

      switch (state) {

        case 0: // Tudo desligado
          allOff();
          break;

        case 1: // Alarme contínuo
          allOn();
          break;

        case 2: // Intermitente normal
          alarmToggle(800);
          break;

        case 3: // Intermitente rápido
          alarmToggle(300);
          break;

        case 4: // Intermitente lento
          alarmToggle(1200);
          break;

        case 5: // Alarme máximo (rápido)
          alarmToggle(200);
          break;
      }
    }
  }
}
