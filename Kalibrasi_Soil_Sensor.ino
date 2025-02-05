#include <Arduino.h>

const int soilMoisturePin1 = 34; // Pin untuk sensor kelembaban tanah 1
const int soilMoisturePin2 = 35; // Pin untuk sensor kelembaban tanah 2

void setup() {
  analogReadResolution(12);
  Serial.begin(115200); // Inisialisasi komunikasi serial
  pinMode(soilMoisturePin1, INPUT); // Atur pin sebagai input
  pinMode(soilMoisturePin2, INPUT); // Atur pin sebagai input
}

void loop() {
  // Baca nilai analog dari sensor kelembaban tanah
  int soilMoistureValue1 = analogRead(soilMoisturePin1);
  int soilMoistureValue2 = analogRead(soilMoisturePin2);

  // Tampilkan nilai ke Serial Monitor
  Serial.print("Soil Moisture Sensor 1: ");
  Serial.println(soilMoistureValue1);
  Serial.print("Soil Moisture Sensor 2: ");
  Serial.println(soilMoistureValue2);

  // Tambahkan sedikit penundaan sebelum pembacaan berikutnya
  delay(1000); // Delay 1 detik
}
