# Embedded-IoT-Homework-2: Dual PWM LED Control with OLED Monitoring

**Name:** Abeesha Amin  **Reg#:** 24-NTU-CS-FL-1004  **Course:** Embedded IoT Systems

**Wokwi project link:** https://wokwi.com/projects/479072287358222337

## 1. Objective
Fade two LEDs with different PWM frequencies using the ESP32 LEDC peripheral, and show the live duty cycle and configured frequency of both LEDs on an SSD1306 I2C OLED. Tested in Wokwi and on hardware.

## 2. Project structure
```
Embedded-IoT-Homework-2/
├── diagram.json
├── wokwi.toml
├── platformio.ini
├── src/main.cpp
├── screenshots/
│   ├── wokwi_circuit.png
│   ├── wokwi_running_oled.png
│   ├── hardware_oled_output1.jpg
│   └── hardware_oled_output2.jpg
└── README.md
```

## 3. Wiring
| Component | ESP32 pin |
|---|---|
| LED1 anode (via own 220 ohm resistor) | GPIO 18 |
| LED2 anode (via own 220 ohm resistor) | GPIO 19 |
| LED1 and LED2 cathodes | GND |
| OLED SDA | GPIO 21 |
| OLED SCL | GPIO 22 |
| OLED VCC | 3V3 |
| OLED GND | GND |

Each LED has a separate current-limiting resistor (about 6 mA per LED).

## 4. PWM frequencies and duty-cycle control
| | LED1 | LED2 |
|---|---|---|
| GPIO | 18 | 19 |
| Frequency | 1000 Hz | 5000 Hz |
| Resolution | 8-bit (0-255) | 8-bit (0-255) |
| LEDC channel (core 2.x) | 0 | 2 |
| Fade step | 3 per 15 ms | 5 per 15 ms |
| Start | 0%, rising | 100%, falling |

- Duty cycle controls apparent brightness: higher duty = LED ON longer = brighter. Duty (%) = value x 100 / 255.
- Frequency is how often the ON/OFF cycle repeats. It does not change brightness; too low a frequency causes visible flicker.
- Fading uses `millis()` (non-blocking). The globals `duty1` and `duty2` are the values written to the LEDs and are also the values shown on the OLED.

## 5. LEDC technical check (resource sharing)
- ESP32 LEDC has 4 timers per speed mode; each PWM channel uses one timer, and channels on the same timer share one frequency.
- Arduino-ESP32 core 2.x maps channel N to timer N/2, so channels 0 and 1 share a timer. LED1 uses channel 0 and LED2 uses channel 2, so they run on different timers and can have different frequencies.
- Core 3.x allocates channels and timers automatically from `ledcAttach(pin, freq, resolution)`.
- Resolution limit: max bits = log2(80 MHz / frequency). At 5000 Hz this is 13 bits, so 8-bit is valid for both LEDs.
- The code uses `ESP_ARDUINO_VERSION_MAJOR` to pick the LEDC API for core 2.x or 3.x.

## 6. OLED output
```
PWM MONITOR
LED1: 25%  F:1000Hz
LED2: 75%  F:5000Hz
```
Values update every 100 ms, with a bar graph under each line.

## 7. Test results
| Test | Wokwi | Hardware |
|---|---|---|
| LED1 fades smoothly | Pass | Pass |
| LED2 fades smoothly | Pass | Pass |
| Different frequencies applied | Pass | Pass |
| OLED shows duty % and frequency | Pass | Pass |
| OLED matches program values | Pass | Pass |

## 8. Wokwi vs hardware
| Item | Wokwi | Hardware | Explanation |
|---|---|---|---|
| GPIO / I2C pins | 18, 19, 21, 22 | <same or changed> | <reason> |
| OLED address | 0x3C | <0x3C or other> | <reason> |
| Brightness / flicker | Ideal | <observation> | Real LEDs and camera exposure differ |

## 9. Evidence
Files in `screenshots/`: Wokwi circuit, Wokwi running simulation with OLED, hardware circuit photo, hardware OLED-output photo.

## 10. Commits
1. `task1 completed` (LED PWM only)
2. `complete homework` (OLED, evidence, README)