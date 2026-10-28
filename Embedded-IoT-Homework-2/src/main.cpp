// Homework 2 - Dual LED PWM + SSD1306 OLED monitor
// Name: Abeesha Amin   Reg#: 24-NTU-CS-FL-1004


#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------- Pins ----------
#define LED1_PIN   18
#define LED2_PIN   19
#define OLED_SDA   21
#define OLED_SCL   22

// ---------- PWM settings ----------
// Core 2.x only: use channels on DIFFERENT timers (0-1 share timer 0, 2-3 share timer 1)
#define LED1_CH    0
#define LED2_CH    2
#define LED1_FREQ  1000      // Hz
#define LED2_FREQ  5000      // Hz
#define PWM_RES    8         // 8-bit -> duty 0..255
#define MAX_DUTY   255

// ---------- OLED ----------
#define SCREEN_W   128
#define SCREEN_H   64
#define OLED_ADDR  0x3C
Adafruit_SSD1306 display(SCREEN_W, SCREEN_H, &Wire, -1);

// ---------- Fade state ----------
int duty1 = 0,        step1 = 2;    // LED1: slower fade (about 8 s per full cycle)
int duty2 = MAX_DUTY, step2 = -3;   // LED2: faster fade (about 5 s per cycle), opposite start
unsigned long lastFade = 0, lastOled = 0;
const unsigned long FADE_INTERVAL_MS = 30;
const unsigned long OLED_INTERVAL_MS = 100;

void setDuty(int led, int duty) {
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(led == 1 ? LED1_PIN : LED2_PIN, duty);
#else
  ledcWrite(led == 1 ? LED1_CH : LED2_CH, duty);
#endif
}

int toPercent(int duty) { return duty * 100 / MAX_DUTY; }

void drawBar(int y, int percent) {
  display.drawRect(0, y, 128, 8, SSD1306_WHITE);
  display.fillRect(0, y, percent * 128 / 100, 8, SSD1306_WHITE);
}

void updateOled() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print("PWM MONITOR");

  // Same duty values that are written to the LEDs -> display always matches output
  display.setCursor(0, 16);
  display.printf("LED1: %d%%  F:%dHz", toPercent(duty1), LED1_FREQ);
  drawBar(26, toPercent(duty1));

  display.setCursor(0, 40);
  display.printf("LED2: %d%%  F:%dHz", toPercent(duty2), LED2_FREQ);
  drawBar(50, toPercent(duty2));

  display.display();
}

void setup() {
  Serial.begin(115200);

#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(LED1_PIN, LED1_FREQ, PWM_RES);
  ledcAttach(LED2_PIN, LED2_FREQ, PWM_RES);
#else
  ledcSetup(LED1_CH, LED1_FREQ, PWM_RES);
  ledcAttachPin(LED1_PIN, LED1_CH);
  ledcSetup(LED2_CH, LED2_FREQ, PWM_RES);
  ledcAttachPin(LED2_PIN, LED2_CH);
#endif
  setDuty(1, duty1);
  setDuty(2, duty2);

  Wire.begin(OLED_SDA, OLED_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("SSD1306 not found - check wiring/address");
    while (true) delay(1000);
  }
  updateOled();
}

void loop() {
  unsigned long now = millis();

  if (now - lastFade >= FADE_INTERVAL_MS) {
    lastFade = now;

    duty1 += step1;
    if (duty1 >= MAX_DUTY) { duty1 = MAX_DUTY; step1 = -step1; }
    if (duty1 <= 0)        { duty1 = 0;        step1 = -step1; }

    duty2 += step2;
    if (duty2 >= MAX_DUTY) { duty2 = MAX_DUTY; step2 = -step2; }
    if (duty2 <= 0)        { duty2 = 0;        step2 = -step2; }

    setDuty(1, duty1);
    setDuty(2, duty2);
  }

  if (now - lastOled >= OLED_INTERVAL_MS) {
    lastOled = now;
    updateOled();
    Serial.printf("LED1 %3d%% @ %d Hz | LED2 %3d%% @ %d Hz\n",
                  toPercent(duty1), LED1_FREQ, toPercent(duty2), LED2_FREQ);
  }
}