#include <Wire.h>
#include <MPU6050_tockn.h>
#include <esp_now.h>
#include <WiFi.h>

// Obiect MPU6050
MPU6050 mpu(Wire);

// Structură pentru trimiterea datelor (include accelerometru + giroscop)
typedef struct {
    float gyroX, gyroY, gyroZ;
    float accX, accY, accZ;
} SensorData;

SensorData data;  // Variabilă pentru stocarea datelor

// Adresa MAC a receptorului (ESP32 receiver)
uint8_t receiverMAC[] = {0xEC, 0x62, 0x60, 0x1E, 0xDA, 0xB4};

// Callback pentru confirmarea trimiterii
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Status trimitere: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Succes" : "Eșec");
}

void setup() {
    Serial.begin(115200);
    Wire.begin();
    
    // Inițializare MPU6050
    mpu.begin();
    mpu.calcGyroOffsets(true);
    
    // Configurare WiFi în mod STA
    WiFi.mode(WIFI_STA);
    
    // Inițializare ESP-NOW
    if (esp_now_init() != ESP_OK) {
        Serial.println("Eroare la inițializarea ESP-NOW!");
        return;
    }
    
    // Adăugare peer (receptor)
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, receiverMAC, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Eroare la adăugarea receptorului!");
        return;
    }
    
    // Setare callback
    esp_now_register_send_cb(OnDataSent);
    
    Serial.println("Transmițător ESP-NOW gata!");
}

void loop() {
    mpu.update();

    // Citire valori giroscop
    data.gyroX = mpu.getGyroX();
    data.gyroY = mpu.getGyroY();
    data.gyroZ = mpu.getGyroZ();

    // Citire valori accelerometru
    data.accX = mpu.getAccX();
    data.accY = mpu.getAccY();
    data.accZ = mpu.getAccZ();

    // Trimitere date
    esp_now_send(receiverMAC, (uint8_t *) &data, sizeof(data));

    Serial.println("Date trimise:");
    Serial.print("Gyro X: "); Serial.print(data.gyroX);
    Serial.print(" | Gyro Y: "); Serial.print(data.gyroY);
    Serial.print(" | Gyro Z: "); Serial.print(data.gyroZ);
    Serial.print(" | Acc X: "); Serial.print(data.accX);
    Serial.print(" | Acc Y: "); Serial.print(data.accY);
    Serial.print(" | Acc Z: "); Serial.println(data.accZ);

    delay(50); // Trimitere la fiecare 50 ms pentru răspuns rapid
}
