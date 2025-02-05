// Konfigurasi Blynk & Spreadsheet
#define BLYNK_TEMPLATE_ID "TMPL6i4IIV9tI"
#define BLYNK_TEMPLATE_NAME "Tugas Akhir"
#define BLYNK_AUTH_TOKEN "CjgUQ4I7bTXCKHfRTpRwgp2PdCRqBV3m"
#define Your_Google_Apps_Script_ID "AKfycbzL8nOG6yQ_7bu7UapV4_sjWPtTb1hV-MlmFSYdr6RUYrLRYJFS6svcpju3TyKKid_Dzg"

// Library yang diperlukan
#include <WiFiManager.h>
#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal_I2C.h>
#include <BlynkSimpleEsp32.h>

// Konfigurasi LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Konfigurasi Relay
const int relay1 = 25;
const int relay2 = 26;

// Konfigurasi sensor kelembaban tanah
const int soilMoisture1 = 34;
const int soilMoisture2 = 35;

// Kalibrasi sensor kelembaban tanah 
const int dry1 = 1800; 
const int wet1 = 1500; 
const int dry2 = 2400; 
const int wet2 = 1500; 

// Batas kelembaban tanah
const int moistureThresholdHigh = 80;
const int moistureThresholdLow = 20;

// Konfigurasi sensor suhu tanah
#define ONE_WIRE_BUS 15

// Konfigurasi OneWire dan DallasTemperature
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
// Alamat sensor DS18B20
DeviceAddress sensor1 = { 0x28, 0xF3, 0x23, 0x43, 0xD4, 0xEE, 0x69, 0x3C };
DeviceAddress sensor2 = { 0x28, 0xCB, 0xA1, 0x43, 0xD4, 0xEE, 0x18, 0x9E };

// Konfigurasi sensor DHT22
#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
// Batas suhu rumah kaca
const int temperatureThreshold  = 30;
const int humidityThreshold  = 50;

// Konfigurasi PWM Fan
#define FAN_PIN 5

// Konfigurasi Blynk & Spreadsheet
char auth[] = BLYNK_AUTH_TOKEN;
char GAS_ID[] = Your_Google_Apps_Script_ID;

unsigned long previousMillis = 0;
const long interval = 5000; // Interval pembacaan sensor

bool autoControlRelay1 = true; // Mode kontrol otomatis untuk relay 1
bool autoControlRelay2 = true; // Mode kontrol otomatis untuk relay 2

void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(115200);
  dht.begin();
  analogReadResolution(12);

  // Inisialisasi pin
  pinMode(soilMoisture1, INPUT);
  pinMode(soilMoisture2, INPUT);
  pinMode(DHTPIN, INPUT);
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);

  // Atur kecepatan kipas ke 50% saat start
  analogWrite(FAN_PIN, 128);

  // Inisialisasi relay
  digitalWrite(relay1, LOW);
  digitalWrite(relay2, LOW);

  // Inisialisasi sensor suhu
  sensors.begin();

  // Inisialisasi LCD
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(3, 1);
  lcd.print("TUGAS AKHIR SK");
  lcd.setCursor(8, 2);
  lcd.print("2024");
  delay(6000);
  lcd.clear();

  // WiFiManager untuk koneksi WiFi
  WiFiManager wm;
  bool res = wm.autoConnect("RumahKacaPintar", "1q2w3e4r");
  if (!res) {
    Serial.println("Failed to connect");
    ESP.restart(); // Restart ESP jika gagal koneksi
  } else {
    Serial.println("connected...yeey :)");
  }

  // Menghubungkan ke Blynk
  Blynk.config(auth);
  Blynk.connect();
}

void loop() {
  Blynk.run(); // Jalankan Blynk

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Baca kelembaban tanah
    int soilMoistureValue1 = analogRead(soilMoisture1);
    int soilMoistureValue2 = analogRead(soilMoisture2);
    // Merubah menjadi persen
    int percentageHumidity1 = map(soilMoistureValue1, dry1, wet1, 0, 100);
    int percentageHumidity2 = map(soilMoistureValue2, dry2, wet2, 0, 100);
    percentageHumidity1 = constrain(percentageHumidity1, 0, 100);
    percentageHumidity2 = constrain(percentageHumidity2, 0, 100);

    // Hitung nilai rata-rata kelembaban tanah
    int averageHumiditySoil = (percentageHumidity1 + percentageHumidity2) / 2;
    
    // Baca suhu tanah
    sensors.requestTemperatures();
    int tempValue1 = sensors.getTempC(sensor1);
    int tempValue2 = sensors.getTempC(sensor2);

    // Hitung nilai rata-rata suhu tanah
    int averageTempSoil = (tempValue1 + tempValue2) / 2;

    // Baca suhu dan kelembaban dari DHT22
    float dhtTemp = dht.readTemperature();
    int dhtHumidity = dht.readHumidity();

    // Validasi pembacaan sensor
    if (isnan(dhtHumidity) || isnan(dhtTemp)) {
      lcd.clear();
      lcd.setCursor(6,0);
      lcd.print("LOADING");
    } else {

    // Tampilkan data di LCD    
    // Ruangan
    lcd.clear();
    lcd.setCursor(1,0);
    lcd.print("RUANGAN");
    lcd.setCursor(1,2);
    lcd.print("T: ");
    lcd.setCursor(3,2);
    lcd.print(dhtTemp);
    lcd.setCursor(7,2);
    lcd.print("C"); 
    lcd.setCursor(1,3);
    lcd.print("H: ");
    lcd.setCursor(3,3);
    lcd.print(dhtHumidity);
    lcd.setCursor(5,3);
    lcd.print("%");
    // Tanah
    lcd.setCursor(14,0);
    lcd.print("TANAH");
    lcd.setCursor(14,2);
    lcd.print("T: ");
    lcd.setCursor(16,2);
    lcd.print(averageTempSoil);
    lcd.setCursor(18,2);
    lcd.print("C");
    lcd.setCursor(14,3);
    lcd.print("H: ");
    lcd.setCursor(16,3);
    lcd.print(averageHumiditySoil);
    lcd.setCursor(19,3);
    lcd.print("%");    
    }
    // Tampilkan data di Serial Monitor
    displaySerial(percentageHumidity1, percentageHumidity2, averageHumiditySoil, averageTempSoil, tempValue1, tempValue2, dhtTemp, dhtHumidity);

    // Atur kecepatan kipas berdasarkan suhu
    if (dhtTemp >= 30) {
      analogWrite(FAN_PIN, 255); // 100% kecepatan
    } else {
      analogWrite(FAN_PIN, 128); // 50% kecepatan
    }

    // Kontrol relay berdasarkan kelembaban tanah jika dalam mode otomatis
    if (autoControlRelay1) {
      controlRelay1(averageHumiditySoil, moistureThresholdHigh, moistureThresholdLow);
    }
    if (autoControlRelay2) {
      controlRelay2(dhtHumidity, humidityThreshold);
    }

    // Kirim data ke Google Spreadsheet
    sendToGoogleSheet(dhtTemp, dhtHumidity, averageTempSoil, averageHumiditySoil, digitalRead(relay1), digitalRead(relay2), percentageHumidity1, percentageHumidity2, tempValue1, tempValue2, soilMoistureValue1, soilMoistureValue2);

    // Kirim data ke Blynk
    Blynk.virtualWrite(V1, averageHumiditySoil);
    Blynk.virtualWrite(V2, averageTempSoil);
    Blynk.virtualWrite(V3, dhtTemp);
    Blynk.virtualWrite(V4, dhtHumidity);
  }
}

// Fungsi untuk menampilkan data di Serial Monitor
void displaySerial(int percentageHumidity1, int percentageHumidity2, int averageHumiditySoil, int averageTempSoil, int tempValue1, int tempValue2, float dhtTemp, int dhtHumidity) {
  Serial.print("Soil Moisture Value 1: ");
  Serial.print(percentageHumidity1);
  Serial.print("% | Soil Moisture Value 2: ");
  Serial.print(percentageHumidity2);
  Serial.print("% | Soil Moisture Average: ");
  Serial.print(averageHumiditySoil);  
  Serial.print("% | Temp Value 1: ");
  Serial.print(tempValue1);
  Serial.print("°C | Temp Value 2: ");
  Serial.print(tempValue2);
  Serial.print("°C | Soil Temp Average: ");
  Serial.print(averageTempSoil);
  Serial.print("°C | DHT22 Temp: ");
  Serial.print(dhtTemp);
  Serial.print("°C | DHT22 Humidity: ");
  Serial.print(dhtHumidity);
  Serial.println("%");
}

// Fungsi untuk mengontrol relay1 secara otomatis
void controlRelay1(int averageHumiditySoil, int moistureThresholdHigh, int moistureThresholdLow) {
  if (averageHumiditySoil <= moistureThresholdLow) {
    digitalWrite(relay1, HIGH); // Hidupkan Relay 1
    Serial.println("Relay 1 is ON");
  } else if (averageHumiditySoil >= moistureThresholdHigh) {
    digitalWrite(relay1, LOW); // Matikan Relay 1
    Serial.println("Relay 1 is OFF");
  }
}

// Fungsi untuk mengontrol relay2 secara otomatis
void controlRelay2(int dhtHumidity, int humidityThreshold) {
    if (dhtHumidity < humidityThreshold) {
    digitalWrite(relay2, HIGH); // Menghidupkan relay
    Serial.println("Relay 2 is ON");
  } else {
    digitalWrite(relay2, LOW); // Mematikan relay
    Serial.println("Relay 2 is OFF");
  }
}

// Subroutine for sending data to Google Sheets
void sendToGoogleSheet(float dhtTemp, int dhtHumidity, int averageTempSoil, int averageHumiditySoil, int relay1, int relay2, int percentageHumidity1, int percentageHumidity2, int tempValue1, int tempValue2, int soilMoistureValue1, int soilMoistureValue2) {
  HTTPClient http;
  String url = "https://script.google.com/macros/s/" + String(GAS_ID) + "/exec";
  String postData = "?temperature=" + String(dhtTemp) + 
                  "&humidity=" + String(dhtHumidity) + 
                  "&temptanah=" + String(averageTempSoil) + 
                  "&humitanah=" + String(averageHumiditySoil) + 
                  "&relay1=" + String(relay1) + 
                  "&relay2=" + String(relay2) +
                  "&tanahkiri=" + String(percentageHumidity1) +
                  "&tanahkanan=" + String(percentageHumidity2) +
                  "&suhukiri=" + String(tempValue1) +
                  "&suhukanan=" + String(tempValue2) +
                  "&soil1=" + String(soilMoistureValue1) +
                  "&soil2=" + String(soilMoistureValue2); 


  Serial.print("Sending data to Google Sheets: ");
  Serial.println(url + postData);

  http.begin(url + postData);
  int httpCode = http.GET();

  if (httpCode > 0) {
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}



// Fungsi untuk mengontrol relay dari Blynk (manual control)
BLYNK_WRITE(V5) {
  int value = param.asInt(); // Mendapatkan nilai dari Blynk
  autoControlRelay1 = false; // Matikan mode otomatis
  digitalWrite(relay1, value);
  Serial.print("Relay 1 is ");
  Serial.println(value == LOW ? "ON" : "OFF");
}

BLYNK_WRITE(V6) {
  int value = param.asInt(); // Mendapatkan nilai dari Blynk
  autoControlRelay2 = false; // Matikan mode otomatis
  digitalWrite(relay2, value);
  Serial.print("Relay 2 is ");
  Serial.println(value == LOW ? "ON" : "OFF");
}

// Fungsi untuk mengatur mode kontrol otomatis dari Blynk
BLYNK_WRITE(V7) {
  autoControlRelay1 = param.asInt();
  Serial.print("Auto control Relay 1 is ");
  Serial.println(autoControlRelay1 ? "enabled" : "disabled");
}

BLYNK_WRITE(V8) {
  autoControlRelay2 = param.asInt();
  Serial.print("Auto control Relay 2 is ");
  Serial.println(autoControlRelay2 ? "enabled" : "disabled");
}