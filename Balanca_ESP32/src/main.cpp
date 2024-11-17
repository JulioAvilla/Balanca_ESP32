#include <Arduino.h>

// Informações ThingSpeak
// Channel ID = 2720474
// API Write Key = MKU7RWUMQO4ZGJPG
// API Read Key = 021LVCYCL0K8NYGJ


#include <WiFi.h>
// Biblioteca da Balança
#include <HX711.h>
#include <ThingSpeak.h>
// Biblioteca do Display
#include <LiquidCrystal_I2C.h>

// Pinos da Balança
const int DT_PIN = 19;
const int SCK_PIN = 18;

// WiFi
const char* WIFI_NAME = "Wokwi-GUEST";
const char* WIFI_PASSWORD = "";

// ThingSpeak
const int myChannelNumber = 2720474 ;
const char* myApiKey = "MKU7RWUMQO4ZGJPG";
const char* server = "api.thingspeak.com";

// Inicializa o objeto HX711 (Balança)
HX711 scale;
WiFiClient client;
//Inicia o Display
LiquidCrystal_I2C lcd(0x27, 16, 2);

void push(float);

void setup() {
  Serial.begin(115200);

  // Inicia a Balança
  scale.begin(DT_PIN, SCK_PIN);
  // Parâmetros de Configuração
  scale.set_gain(128);
  scale.set_scale(420);
  // Reseta o Valor da Balança
  scale.tare();

  // Inicia WiFi
  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Wifi not connected");
  }
  Serial.println("Wifi connected !");
  Serial.println("Local IP: " + String(WiFi.localIP()));
  ThingSpeak.begin(client);

  // Inicia o Display
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Peso: ");
  lcd.setCursor(12, 0);
  lcd.print("kg");

  Serial.println("Balança iniciada!");
}

int i = 0;
float aux = 0;

// Loop de 1 segundo
void loop() {
  // Lê o peso atual
  float weight = scale.get_units(10);
  String msg = String(weight) + " kg";

  // Mostra o Peso no Display
  lcd.setCursor(6, 0);
  lcd.print(String(weight));

  // Aumenta "i" caso o peso seja igual ao loop passado
  if (weight == aux) {
    i++;
  }
  else {
    i = 0;
  }

  // Caso peso seja o mesmo por 3 segundos manda para o ThigSpeak
  if (i == 3) {
    push(weight);
  }

  aux = weight;

  delay(1000);
}

// Da Push no ThingSpeak
void push(float weight) {

  ThingSpeak.setField(1, weight);

  int x = ThingSpeak.writeFields(myChannelNumber,myApiKey);

  if(x == 200){
    Serial.println("Data pushed successfull");
  }else{
    Serial.println("Push error" + String(x));
  }
  Serial.println("---");
}