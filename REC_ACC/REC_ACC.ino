#include <WiFi.h>
#include <esp_now.h>

// Structura pentru datele primite (inclusiv datele de la accelerometru)
typedef struct {
  float gyroX;
  float gyroY;
  float gyroZ;
  float accX;
  float accY;
  float accZ;
} SensorData;

SensorData receivedData;  // Variabilă pentru a stoca datele primite

// Callback la primirea datelor
void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  memcpy(&receivedData, data, sizeof(receivedData));  // Copiem datele primite

  // Trimitem datele prin UART la Arduino
  Serial.print("Gyro X: "); Serial.print(receivedData.gyroX);
  Serial.print(" | Gyro Y: "); Serial.print(receivedData.gyroY);
  Serial.print(" | Gyro Z: "); Serial.print(receivedData.gyroZ);
  Serial.print(" | Acc X: "); Serial.print(receivedData.accX);
  Serial.print(" | Acc Y: "); Serial.print(receivedData.accY);
  Serial.print(" | Acc Z: "); Serial.println(receivedData.accZ);
}

void setup() {
  Serial.begin(115200);  // Inițializare monitor serial pentru UART (comunicare cu Arduino)
  WiFi.mode(WIFI_STA);   // Setăm ESP32-ul în modul STA (stație)

  // Inițializare ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Eroare la inițializarea ESP-NOW!");
    return;
  }

  // Setăm callback-ul pentru primirea datelor
  esp_now_register_recv_cb(OnDataRecv);

  Serial.println("Receiver ESP-NOW gata!");
}

void loop() {
  // Aici nu avem nevoie de cod suplimentar, deoarece receiver-ul așteaptă datele
  // care vor fi procesate de funcția de callback OnDataRecv
}
