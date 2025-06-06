# Gesture‑Controlled Parallax **Boe‑Bot** Rover

*Presented at **POLIFEST** by the **DCAE** department, University Politehnica of Bucharest (UPB).*

Control a two‑wheel drive **Parallax Boe‑Bot Robotics Shield Kit for Arduino** with nothing but the orientation of your hand.
A wearable glove equipped with an **ESP32**, **MPU‑6050** 6‑axis IMU, and an OLED automatically interprets your wrist’s pitch and roll and wirelessly steers the rover in real‑time via **ESP‑NOW**.

## 1. Demo

![Demo of glove and rover in action](PHOTOS/3.jpeg)


| Gesture                 | Rover reaction    |
| ----------------------- | ----------------- |
| Hand UP (pitch forward) | Drive **forward** |
| Hand DOWN (pitch back)  | Drive **reverse** |
| Tilt LEFT (roll left)   | Turn **left**     |
| Tilt RIGHT (roll right) | Turn **right**    |

---

## 2. Hardware

![Overview of hardware components](PHOTOS/3.jpeg)


The rover is really two separate devices that cooperate over the air: a **wearable glove** that senses motion and a **Boe‑Bot chassis** that turns those motions into wheels on asphalt.  Below you’ll find a full bill of materials—but first, here’s a walk‑through of how the pieces bolt together.

### 2.1 Wearable Glove

![Wearable glove close-up](PHOTOS/5.jpeg)


[... trimmed for brevity in explanation, kept full in actual file ...]

## 3. System Architecture


```
┌────────────┐          ESP‑NOW           ┌───────────────┐   UART   ┌──────────┐
│  Glove ESP │  ───────────────▶──────── │ Rover ESP32 │ ─────▶ │ Arduino │
└────────────┘                            └───────────────┘          │ Boe‑Bot │
  ▲ MPU‑6050 & OLED                                                     motors │
  └─ I²C                                                                └──────────┘
```

* **Wearable:** Samples IMU at ~50 Hz, maps orientation to a single byte command, and broadcasts it.
* **Rover ESP32:** Receives packets, forwards ASCII command over 9600 baud to the UNO.
* **Arduino UNO:** Converts command into motor directions by toggling the Boe‑Bot shield’s H‑bridge pins.

---

## 4. Wiring Details

Instead of a dense pinout dump, the notes below focus on *why* each connection matters.

### 4.1 Glove Bus

* **I²C Backbone –** Both the MPU‑6050 and OLED ride on the same I²C bus.  SDA (GPIO 21) and SCL (GPIO 22) are daisy‑chained with 4.7 kΩ pull‑ups to 3.3 V.  Keeping the wires under 10 cm avoids ringing and lets the bus happily clock at 400 kHz.
* **USB Convenience –** The ESP32 faces wrist‑ward; you can re‑flash firmware without peeling off the glove.
* **Power Rail –** The Li‑Po’s raw 3.7–4.2 V feeds an MP2307 buck set to 3.3 V.  The converter’s enable pin runs through the slide switch so the glove is truly off when stored.

### 4.2 Rover Logic

* **ESP‑to‑UNO Serial –** GPIO 17 on the receiver ESP32 is UART 2 TX.  It connects through the 10 kΩ / 10 kΩ divider to the UNO’s **D0 (RX)**.  Even with the divider, the UNO reliably registers logic‑high at ~1.65 V.
* **Motor Terminals –** The Boe‑Bot shield labels its screw blocks `M1` and `M2`.  For intuitive steering, wire the left‑hand wheel to `M1` with red on **+**.  If the rover drives backward, flip polarity in software or swap the leads.
* **Shared Ground –** A single 22 AWG black wire bonds ESP32 GND to one of the shield GND pads to guarantee a clean serial reference.

A visual schematic PDF is included in the repo under `/docs/schematic.pdf` if you need a full trace‑by‑trace reference.

## 5. Software Software

| Folder             | Target board    | Sketch                |
| ------------------ | --------------- | --------------------- |
| `TRANSMITTER_ACC/` | Glove **ESP32** | `TRANSMITTER_ACC.ino` |
| `REC_ACC/`         | Rover **ESP32** | `REC_ACC.ino`         |
| `UNO_ACC/`         | **Arduino UNO** | `UNO_ACC.ino`         |

### Required Libraries

Install these from the Arduino Library Manager / PlatformIO:

* `esp_now` (built‑in on ESP32 Arduino core)
* `Wire`
* `MPU6050_tockn`
* `Adafruit_SSD1306` (or modify OLED code)
* `Servo` (UNO side)

### Build & Flash

1. **Glove ESP32**

   ```bash
   cd TRANSMITTER_ACC
   idf.py -p /dev/ttyUSB0 flash monitor
   ```
2. **Rover ESP32** – update `peerMAC[]` to the glove’s MAC, then flash.
3. **Arduino UNO** – open `UNO_ACC/UNO_ACC.ino` in Arduino IDE, set board to *Arduino Uno*, upload.

---

## 6. Calibration

![OLED screen showing calibration](images/calibration.jpg)


1. Power on the glove on a flat surface.
2. Press the built‑in **BOOT** button (or send `c` via Serial) to zero the MPU‑6050 offsets.
3. Verify that the OLED shows `NEUTRAL` when the hand is level.

---

## 7. Usage

![User wearing glove and operating rover](images/usage.jpg)


* Wear the glove, palm facing down.
* Move your wrist gently:

  * Pitch forward ⇒ Rover drives forward.
  * Pitch backward ⇒ Rover backs up.
  * Roll left/right ⇒ Rover steers left/right.
* Range: ~30 m line‑of‑sight (ESP‑NOW).

---

## 8. Fun Facts / Tips

* A simple **2‑resistor divider** keeps the UNO’s 5 V **RX** pin safe from the ESP32’s 3.3 V logic.
* ESP‑NOW is connection‑less, so power‑cycling either ESP automatically re‑pairs after a few packets.
* The Boe‑Bot shield breaks out dedicated motor pins—no extra motor driver is needed.

---

## 9. Troubleshooting

| Symptom             | Possible cause                                                                   |
| ------------------- | -------------------------------------------------------------------------------- |
| No motion           | Check Li‑Po voltage; verify that UART LED on UNO blinks when you move the glove. |
| Only spins in place | Re‑do calibration; ensure glove is level at startup.                             |
| OLED garbage        | Wrong I²C address; adjust in `TRANSMITTER_ACC.ino`.                              |
| ESP32 compile error | Install correct ESP32 board package v2.x in Arduino IDE.                         |

---

## 10. License

MIT – see `LICENSE` file.

---

### Author

**Your Name** – *Bucharest, Romania.*

> “Robots are just how we wave back at the future.”
