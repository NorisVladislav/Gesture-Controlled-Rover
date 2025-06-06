#include <Servo.h>
#include <Wire.h>
#include <MPU6050_tockn.h>

Servo servoLeft;
Servo servoRight;

float accX = 0, accY = 0, accZ = 0;
float gyroX = 0, gyroY = 0, gyroZ = 0;
float gyroZ_offset = 0; // Offset pentru a corecta devierea implicită

const float MOVEMENT_THRESHOLD = 0.5;  // Praguri pentru mișcare pe axa X (înaintare/înapoi)
const float TURN_THRESHOLD = 0.3;      // Praguri pentru mișcare pe axa Y (stânga/dreapta)
const float STOP_THRESHOLD = 0.2;      // Praguri pentru a opri mașina când este "plat"

void setup() {
  Serial.begin(115200);

  servoLeft.attach(13);
  servoRight.attach(12);

  // Oprire inițială (setăm servo-urile la poziția neutră)
  servoLeft.write(93);  // Neutral position pentru motorul stâng
  servoRight.write(93); // Neutral position pentru motorul drept

  Wire.begin();
  
  // Inițializare MPU6050
  Serial.println("Calibrare giroscop...");
  calibrateGyro();
  Serial.println("Calibrare completă!");
  Serial.println("Aștept date prin UART...");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');

    Serial.print("Date primite: ");
    Serial.println(input);

    // Extragem valorile accelerometrului din stringul primit
    int accX_pos = input.indexOf("Acc X: ");
    int accY_pos = input.indexOf("Acc Y: ");
    int accZ_pos = input.indexOf("Acc Z: ");
    
    if (accX_pos != -1 && accY_pos != -1 && accZ_pos != -1) {
      accX = input.substring(accX_pos + 7, input.indexOf(" ", accX_pos + 7)).toFloat();
      accY = input.substring(accY_pos + 7, input.indexOf(" ", accY_pos + 7)).toFloat();
      accZ = input.substring(accZ_pos + 7).toFloat();
      
      // Afisare date pentru debugging
      Serial.print("Acc X: "); Serial.print(accX);
      Serial.print(" | Acc Y: "); Serial.print(accY);
      Serial.print(" | Acc Z: "); Serial.println(accZ);
    
      // Controlul mișcării robotului
      controlCar(accX, accY, accZ);
    }
  }

  delay(100);  // Delay pentru a evita suprasolicitarea procesorului
}

void controlCar(float accX, float accY, float accZ) {
  // Opriți mașina dacă accelerometrul este aproape plat (pe ambele axe)
  if (abs(accX) < STOP_THRESHOLD && abs(accY) < STOP_THRESHOLD) {
    stopMotors();
  }
  // Mișcarea înainte / înapoi pe baza accelerometrului (accX)
  else if (accX > MOVEMENT_THRESHOLD) {
    moveForward();
  } else if (accX < -MOVEMENT_THRESHOLD) {
    moveBackward();
  }
  
  // Mișcarea stânga / dreapta pe baza accelerometrului (accY)
  else if (accY > TURN_THRESHOLD) {
    turnRight();
  } else if (accY < -TURN_THRESHOLD) {
    turnLeft();
  }
  
  // Dacă nu sunt îndeplinite condițiile pentru mișcare, opriți motorul
  else {
    stopMotors();
  }
}

void moveForward() {
  servoLeft.write(180);  // Motorul stâng înainte
  servoRight.write(0);   // Motorul drept înainte
}

void moveBackward() {
  servoLeft.write(0);    // Motorul stâng înapoi
  servoRight.write(180); // Motorul drept înapoi
}

void turnRight() {
  servoLeft.write(180);  // Motorul stâng înainte
  servoRight.write(90);  // Motorul drept încet (dă impresia că se rotește)
}

void turnLeft() {
  servoLeft.write(90);   // Motorul stâng încet (dă impresia că se rotește)
  servoRight.write(0);   // Motorul drept înainte
}

void stopMotors() {
  servoLeft.write(93);   // Oprire neutrală
  servoRight.write(93);  // Oprire neutrală
}

void calibrateGyro() {
  int numSamples = 500;
  float sumZ = 0;

  for (int i = 0; i < numSamples; i++) {
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');

      int zPos = input.indexOf("Gyro Z: ");
      if (zPos != -1) {
        float z = input.substring(zPos + 8).toFloat();
        sumZ += z;
      }
    }
    delay(5);
  }

  gyroZ_offset = sumZ / numSamples;
  Serial.print("Offset gyroZ: ");
  Serial.println(gyroZ_offset);
}
